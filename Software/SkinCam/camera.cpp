// ============================================================================================== //
//                                                                                                //
//  This file is part of the ISF SkinCam project.                                                 //
//                                                                                                //
//  Author:                                                                                       //
//  Marcel Hasler <mahasler@gmail.com>                                                            //
//                                                                                                //
//  Copyright (c) 2020 - 2024                                                                     //
//  Bonn-Rhein-Sieg University of Applied Sciences                                                //
//                                                                                                //
//  This program is free software: you can redistribute it and/or modify it under the terms       //
//  of the GNU General Public License as published by the Free Software Foundation, either        //
//  version 3 of the License, or (at your option) any later version.                              //
//                                                                                                //
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     //
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     //
//  See the GNU General Public License for more details.                                          //
//                                                                                                //
//  You should have received a copy of the GNU General Public License along with this program.    //
//  If not, see <https://www.gnu.org/licenses/>.                                                  //
//                                                                                                //
// ============================================================================================== //

#include "camera.h"
#include "logger.h"

#ifdef __linux__
#include <sys/socket.h>
#elif defined(_WIN32)
#include <winsock.h>
#endif

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr int ReceiveBufferSizeMiB = 32;
    constexpr int ReceiveBufferSize = ReceiveBufferSizeMiB * 1024 * 1024;

    constexpr int IncompleteFrameScoreIncrement = 5;
    constexpr int MaximumIncompleteFrameScore = 100;
}

// ---------------------------------------------------------------------------------------------- //

class Camera::FeatureObserver : public AVT::VmbAPI::IFeatureObserver
{
public:
    FeatureObserver(Camera* owner)
        : m_owner(owner) {}

private:
    void FeatureChanged(const AVT::VmbAPI::FeaturePtr& feature) override
    {
        m_owner->handleFeatureChanged(feature);
    }

private:
    Camera* m_owner;
};

// ---------------------------------------------------------------------------------------------- //

class Camera::FrameObserver : public AVT::VmbAPI::IFrameObserver
{
public:
    FrameObserver(Camera* owner, const AVT::VmbAPI::CameraPtr& camera)
        : IFrameObserver(camera), m_owner(owner) {}

private:
    void FrameReceived(const AVT::VmbAPI::FramePtr frame) override
    {
        m_owner->handleFrameReceived(frame);
    }

private:
    Camera* m_owner;
};

// ---------------------------------------------------------------------------------------------- //

Camera::Camera(const Setup& setup, AVT::VmbAPI::VimbaSystem* system)
    : m_serial(setup.serial),
      m_system(system)
{
    checkReceiveBufferSize();

    VmbErrorType result = m_system->OpenCameraByID(m_serial.c_str(), VmbAccessModeFull, m_camera);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to open the camera.", m_serial, result);

    runFeatureCommand("GVSPAdjustPacketSize");

    checkCameraModel();
    checkPacketSize();

    configureDevice(setup);
    setupFeatureObserver();
}

// ---------------------------------------------------------------------------------------------- //

Camera::~Camera()
{
    if (m_running)
        m_camera->StopContinuousImageAcquisition();

    m_camera->Close();
}

// ---------------------------------------------------------------------------------------------- //

void Camera::start()
{
    RETURN_IF(m_running);

    m_currentFrameIndex = 0;
    m_incompleteFrameScore = 0;
    m_frameAvailable = false;

    const int frameCount = 4 * ImagesPerFrame;
    AVT::VmbAPI::IFrameObserverPtr frameObserver(new FrameObserver(this, m_camera));

    VmbErrorType result = m_camera->StartContinuousImageAcquisition(frameCount, frameObserver);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to start continuous acquisition.", m_serial, result);

    m_running = true;
}

// ---------------------------------------------------------------------------------------------- //

void Camera::stop()
{
    RETURN_IF_NOT(m_running);

    m_camera->StopContinuousImageAcquisition();

    m_running = false;
    m_frameAvailable = false;
}

// ---------------------------------------------------------------------------------------------- //

void Camera::getNextFrame(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                          MemoryBuffer1w* channel1, MemoryBuffer1w* channel2)
{
    ASSERT_NOT_NULL(dark);
    ASSERT_NOT_NULL(channel0);
    ASSERT_NOT_NULL(channel1);
    ASSERT_NOT_NULL(channel2);

    RETURN_IF_NOT(m_running);

    const auto timeout = std::chrono::milliseconds(500);
    const auto done = [this]{ return m_frameAvailable || m_exception; };

    std::unique_lock lock(m_mutex);

    if (!m_condition.wait_for(lock, timeout, done))
        throw Exception("Wait for next frame timed out.");

    if (m_exception)
    {
        std::exception_ptr exception = m_exception;
        m_exception = nullptr;

        std::rethrow_exception(exception);
    }

    if (m_incompleteFrameScore >= MaximumIncompleteFrameScore)
    {
        throw Exception("Too many incomplete frames received. This can indicate problems "
                        "with the ethernet connection or insufficient processing power. "
                        "Try decreasing the target frame rate to reduce the network load.");
    }

    m_currentFrames.at(0).swap(dark);
    m_currentFrames.at(1).swap(channel0);
    m_currentFrames.at(2).swap(channel1);
    m_currentFrames.at(3).swap(channel2);

    m_frameAvailable = false;
}

// ---------------------------------------------------------------------------------------------- //

void Camera::setExposureTime(std::chrono::microseconds time)
{
    CLAMP_TO_RANGE(time, MinimumExposureTime, MaximumExposureTime);
    setFeatureValue("ExposureTime", static_cast<double>(time.count()));
}

// ---------------------------------------------------------------------------------------------- //

void Camera::setGain(Gain gain)
{
    setFeatureValue("SensorGain", indexOf<VmbInt64_t>(gain));
}

// ---------------------------------------------------------------------------------------------- //

void Camera::setTemperatureSetpoint(TemperatureSetpoint point)
{
    const bool automatic = (point == TemperatureSetpoint::Automatic);
    setFeatureValue("SensorTemperatureSetpointMode", automatic ? 1 : 0);

    if (!automatic)
    {
        setFeatureValue("SensorTemperatureSetpointSelector", indexOf<VmbInt64_t>(point));
        runFeatureCommand("SensorTemperatureSetpointActivate");
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Camera::getStatus() const -> Status
{
    if (m_exception)
    {
        std::exception_ptr exception = m_exception;
        m_exception = nullptr;

        std::rethrow_exception(exception);
    }

    return m_status;
}

// ---------------------------------------------------------------------------------------------- //

void Camera::setupFeatureObserver()
{
    const std::vector<std::string> names = {
        "SensorTemperatureSetpointActive",
        "DeviceTemperature",
        "DeviceRelativeHumidity",
        "SensorCoolingPower",
        "StatPacketReceived",
        "StatPacketRequested"
    };

    AVT::VmbAPI::IFeatureObserverPtr observer(new FeatureObserver(this));

    for (const auto& name : names)
    {
        AVT::VmbAPI::FeaturePtr feature;
        VmbErrorType result = m_camera->GetFeatureByName(name.c_str(), feature);

        if (result != VmbErrorSuccess)
            throw CameraException("Unable to retrieve feature " + name + ".", m_serial, result);

        feature->RegisterObserver(observer);
        handleFeatureChanged(feature);
    }
}

// ---------------------------------------------------------------------------------------------- //

void Camera::handleFeatureChanged(const AVT::VmbAPI::FeaturePtr& feature)
{
    try {
        VmbFeatureDataType type = VmbFeatureDataUnknown;
        VmbErrorType result = feature->GetDataType(type);

        if (result != VmbErrorSuccess)
            throw Exception("Unable to retrieve feature type.");

        std::string name;
        result = feature->GetName(name);

        if (result != VmbErrorSuccess)
            throw Exception("Unable to retrieve feature name.");

        if (type == VmbFeatureDataInt)
        {
            VmbInt64_t value = 0;
            result = feature->GetValue(value);

            if (result != VmbErrorSuccess)
                throw Exception("Unable to retrieve feature value.");

            if (name == "SensorTemperatureSetpointActive")
                m_status.temperatureSetpoint = toTemperatureSetpoint(value);
            else if (name == "SensorCoolingPower")
                m_status.coolingPower = static_cast<int>(value);
            else if (name == "StatPacketReceived")
                m_status.packetsReceived = value;
            else if (name == "StatPacketRequested")
                m_status.packetsRequested = value;
            else
                throw Exception("Spurious feature-change event detected.");
        }
        else if (type == VmbFeatureDataFloat)
        {
            double value = 0;
            result = feature->GetValue(value);

            if (result != VmbErrorSuccess)
                throw Exception("Unable to retrieve feature value.");

            if (name == "DeviceTemperature")
                m_status.temperature = value;
            else if (name == "DeviceRelativeHumidity")
                m_status.humidity = value;
            else
                throw Exception("Spurious feature-change event detected.");
        }
    }
    catch (const std::exception& e)
    {
        std::lock_guard lock(m_mutex);
        m_exception = std::make_exception_ptr(e);
    }
}

// ---------------------------------------------------------------------------------------------- //

void Camera::handleFrameReceived(const AVT::VmbAPI::FramePtr& frame)
{
    try {
        std::lock_guard lock(m_mutex);

        VmbFrameStatusType status = getReceiveStatus(frame);

        if (status == VmbFrameStatusComplete)
        {
            VmbUchar_t* image = getImagePointer(frame);

            auto data = reinterpret_cast<Vec1w*>(image);
            m_currentFrames.at(m_currentFrameIndex).copyFrom(data);

            if (m_incompleteFrameScore > 0)
                --m_incompleteFrameScore;
        }
        else
        {
            const auto code = std::to_string(status);
            Logger::error("Incomplete frame received with status code " + code + ".");

            m_incompleteFrameScore += IncompleteFrameScoreIncrement;
        }

        if (++m_currentFrameIndex >= m_currentFrames.size())
        {
            m_currentFrameIndex = 0;
            m_frameAvailable = true;
        }
    }
    catch (const CameraException& e)
    {
        std::lock_guard lock(m_mutex);
        m_exception = std::make_exception_ptr(e);
    }

    m_camera->QueueFrame(frame);

    if (m_frameAvailable || m_exception)
        m_condition.notify_one();
}

// ---------------------------------------------------------------------------------------------- //

auto Camera::getReceiveStatus(const AVT::VmbAPI::FramePtr& frame) const -> VmbFrameStatusType
{
    VmbFrameStatusType status = {};
    VmbErrorType result = frame->GetReceiveStatus(status);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to get frame receive status.", m_serial, result);

    return status;
}

// ---------------------------------------------------------------------------------------------- //

auto Camera::getImagePointer(const AVT::VmbAPI::FramePtr& frame) const -> VmbUchar_t*
{
    VmbUchar_t* buffer = nullptr;
    VmbErrorType result = frame->GetImage(buffer);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to get frame image.", m_serial, result);

    return buffer;
}

// ---------------------------------------------------------------------------------------------- //

void Camera::runFeatureCommand(const std::string& name)
{
    AVT::VmbAPI::FeaturePtr feature;
    VmbErrorType result = m_camera->GetFeatureByName(name.c_str(), feature);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to retrieve feature " + name + ".", m_serial, result);

    result = feature->RunCommand();

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to run command for feature " + name + ".", m_serial, result);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void Camera::setFeatureValue(const std::string& name, const T& value)
{
    AVT::VmbAPI::FeaturePtr feature;
    VmbErrorType result = m_camera->GetFeatureByName(name.c_str(), feature);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to retrieve feature " + name + ".", m_serial, result);

    result = feature->SetValue(value);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to set value for feature " + name + ".", m_serial, result);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto Camera::getFeatureValue(const std::string& name) const -> T
{
    AVT::VmbAPI::FeaturePtr feature;
    VmbErrorType result = m_camera->GetFeatureByName(name.c_str(), feature);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to retrieve feature " + name + ".", m_serial, result);

    T value;
    result = feature->GetValue(value);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to get value for feature " + name + ".", m_serial, result);

    return value;
}

// ---------------------------------------------------------------------------------------------- //

void Camera::configureDevice(const Setup& setup)
{
    // Reset all settings to their default values
    setFeatureValue("UserSetSelector", 0);
    runFeatureCommand("UserSetLoad");

    setFeatureValue("GVSPDriver", 0); // Socket
    setFeatureValue("GVSPHostReceiveBufferSize", ReceiveBufferSize);

    setFeatureValue("Width", ImageWidth);
    setFeatureValue("Height", ImageHeight);
    setFeatureValue("PixelFormat", VmbPixelFormatMono14);

    setFeatureValue("TriggerMode", 1);    // Enabled
    setFeatureValue("TriggerOverlap", 2); // PreviousFrame

#ifdef SKINCAM_BUILD_LEGACY
    setFeatureValue("TriggerSource", 2); // Line 2
#else
    setFeatureValue("TriggerSource", 1); // Line 1
#endif

    setFeatureValue("LineOutSource", 2); // FrameTriggerReady

    setExposureTime(setup.exposureTime);
    setTemperatureSetpoint(setup.temperatureSetpoint);
    setGain(setup.gain);
}

// ---------------------------------------------------------------------------------------------- //

void Camera::checkCameraModel()
{
#ifdef SKINCAM_BUILD_LEGACY
    constexpr const char* RequiredModelName = "Goldeye G-032";
#else
    constexpr const char* RequiredModelName = "Goldeye G-033";
#endif

    std::string model;
    VmbErrorType result = m_camera->GetModel(model);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to verify the camera's model name.", m_serial, result);

    if (!model.starts_with(RequiredModelName))
        throw Exception("The connected camera model is not supported by this software.");
}

// ---------------------------------------------------------------------------------------------- //

void Camera::checkPacketSize()
{
    constexpr VmbInt64_t RequiredPacketSize = 8228; // Camera default

    const auto packetSize = getFeatureValue<VmbInt64_t>("GVSPPacketSize");

    if (packetSize < RequiredPacketSize)
    {
        const auto sizeString = std::to_string(packetSize);
        const auto requiredString = std::to_string(RequiredPacketSize);

        throw Exception("The maximum transmission unit of the current ethernet link "
                        "appears to be " + sizeString + " bytes. "
                        "Jumbo frames of " + requiredString + " bytes or more are required.");
    }
}

// ---------------------------------------------------------------------------------------------- //

void Camera::checkReceiveBufferSize()
{
    int fd = ::socket(AF_INET, SOCK_DGRAM, 0);

    auto optval = ReceiveBufferSize;
    auto optvalPtr = reinterpret_cast<char*>(&optval);

#ifdef _WIN32
    int optlen = sizeof(optval);
#else
    socklen_t optlen = sizeof(optval);
#endif

    int result = ::setsockopt(fd, SOL_SOCKET, SO_RCVBUF, optvalPtr, optlen);

    if (result == 0)
        result = ::getsockopt(fd, SOL_SOCKET, SO_RCVBUF, optvalPtr, &optlen);

    if (result < 0)
        throw Exception("Unable to determine UDP receive-buffer size.");

#ifdef __linux__
    optval /= 2; // Kernel doubles the value internally
#endif

    if (optval < ReceiveBufferSize)
    {
        std::string unit = " bytes";

        if (optval % (1024*1024) == 0)
        {
            optval /= 1024*1024;
            unit = " MiB";
        }
        else if (optval % 1024 == 0)
        {
            optval /= 1024;
            unit = " KiB";
        }

        const auto sizeString = std::to_string(optval) + unit;
        const auto requiredString = std::to_string(ReceiveBufferSizeMiB);

        throw Exception("The maximum UDP receive-buffer size currently supported "
                        "by this system appears to be " + sizeString + ". "
                        "A buffer size of " + requiredString + " MiB is required.");
    }
}

// ---------------------------------------------------------------------------------------------- //
