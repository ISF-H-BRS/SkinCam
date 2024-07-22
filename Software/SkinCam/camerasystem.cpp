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

#include "camerasystem.h"
#include "vimbaloader.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

CameraSystem::CameraSystem(const Setup& setup)
    : m_system(&AVT::VmbAPI::VimbaSystem::GetInstance())
{
    ASSERT(VimbaLoader::isInitialized());

    const Camera::Setup& cameraSetup = setup.cameraSetup;

    if (!isCameraAvailable(cameraSetup.serial))
        throw Exception("No supported camera with serial number " + cameraSetup.serial + " found.");

#ifdef SKINCAM_BUILD_LEGACY
    m_controller = std::make_unique<Controller>(setup.controllerPort);
#else
    m_controller = std::make_unique<Controller>();
    setPowerLevels(setup.powerLevel0, setup.powerLevel1, setup.powerLevel2);
#endif

    setSequenceConfiguration(setup.refreshRate, setup.illuminationTime);

#ifdef SKINCAM_USE_DUMMY_CAMERA
    m_camera = std::make_unique<DummyCamera>();
#else
    m_camera = std::make_unique<Camera>(cameraSetup, m_system);
#endif // SKINCAM_USE_DUMMY_CAMERA
}

// ---------------------------------------------------------------------------------------------- //

void CameraSystem::start()
{
    std::lock_guard lock(m_mutex);

    m_camera->start();
    m_controller->runSequence();
}

// ---------------------------------------------------------------------------------------------- //

void CameraSystem::stop()
{
    std::lock_guard lock(m_mutex);

    m_controller->waitSequenceComplete();
    m_camera->stop();
}

// ---------------------------------------------------------------------------------------------- //

auto CameraSystem::getMinRefreshRate() const -> int
{
    return MinimumRefreshRate;
}

// ---------------------------------------------------------------------------------------------- //

auto CameraSystem::getMaxRefreshRate() const -> int
{
    return MaximumRefreshRate;
}

// ---------------------------------------------------------------------------------------------- //

void CameraSystem::setRefreshRate(int rate)
{
    setSequenceConfiguration(rate, m_illuminationTime);
}

// ---------------------------------------------------------------------------------------------- //

auto CameraSystem::getRefreshRate() const -> int
{
    return m_refreshRate;
}

// ---------------------------------------------------------------------------------------------- //

void CameraSystem::getNextFrame(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                                MemoryBuffer1w* channel1, MemoryBuffer1w* channel2)
{
    // In order to achieve the maximum possible frame rate, the next sequence can
    // already be started while the previous sequence is being read from the buffer

    std::lock_guard lock(m_mutex);

    m_controller->runSequence();
    m_camera->getNextFrame(dark, channel0, channel1, channel2);
}

// ---------------------------------------------------------------------------------------------- //

void CameraSystem::setIlluminationTime(std::chrono::microseconds us)
{
    setSequenceConfiguration(m_refreshRate, us);
}

// ---------------------------------------------------------------------------------------------- //

auto CameraSystem::getIlluminationTime() const -> std::chrono::microseconds
{
    std::lock_guard lock(m_mutex);

    Controller::SequenceConfiguration config = m_controller->getSequenceConfiguration();
    return config.illuminationTime;
}

// ---------------------------------------------------------------------------------------------- //

void CameraSystem::setSequenceConfiguration(int refreshRate,
                                            std::chrono::microseconds illuminationTime)
{
    CLAMP_TO_RANGE(refreshRate, MinimumRefreshRate, MaximumRefreshRate);
    CLAMP_TO_RANGE(illuminationTime, MinimumIlluminationTime, MaximumIlluminationTime);

    m_refreshRate = refreshRate;
    m_illuminationTime = illuminationTime;

    const auto sequenceDuration = static_cast<long>(Camera::ImagesPerFrame) * illuminationTime;
    const auto sequenceDelay = std::max(1'000'000us / refreshRate - sequenceDuration, 0us);

    Controller::SequenceConfiguration config = {
        illuminationTime,
        sequenceDelay
    };

    std::lock_guard lock(m_mutex);
    m_controller->setSequenceConfiguration(config);
}

// ---------------------------------------------------------------------------------------------- //

#ifndef SKINCAM_BUILD_LEGACY

// ---------------------------------------------------------------------------------------------- //

void CameraSystem::setPowerLevels(int level0, int level1, int level2)
{
    CLAMP_TO_RANGE(level0, Controller::MinimumPowerLevel, Controller::MaximumPowerLevel);
    CLAMP_TO_RANGE(level1, Controller::MinimumPowerLevel, Controller::MaximumPowerLevel);
    CLAMP_TO_RANGE(level2, Controller::MinimumPowerLevel, Controller::MaximumPowerLevel);

    std::lock_guard lock(m_mutex);
    m_controller->setPowerConfiguration({ level0, level1, level2 });
}

// ---------------------------------------------------------------------------------------------- //

auto CameraSystem::getPowerLevels() const -> std::tuple<int,int,int>
{
    std::lock_guard lock(m_mutex);

    Controller::PowerConfiguration config = m_controller->getPowerConfiguration();

    return {
        config.powerLevel0,
        config.powerLevel1,
        config.powerLevel2
    };
}

// ---------------------------------------------------------------------------------------------- //

auto CameraSystem::getPowerStatus() const -> Controller::PowerStatus
{
    std::lock_guard lock(m_mutex);
    return m_controller->getPowerStatus();
}

// ---------------------------------------------------------------------------------------------- //

#endif // !SKINCAM_BUILD_LEGACY

// ---------------------------------------------------------------------------------------------- //

void CameraSystem::setExposureTime(std::chrono::microseconds time)
{
    std::lock_guard lock(m_mutex);
    m_camera->setExposureTime(time);
}

// ---------------------------------------------------------------------------------------------- //

void CameraSystem::setGain(Camera::Gain gain)
{
    std::lock_guard lock(m_mutex);
    m_camera->setGain(gain);
}

// ---------------------------------------------------------------------------------------------- //

void CameraSystem::setTemperatureSetpoint(Camera::TemperatureSetpoint point)
{
    std::lock_guard lock(m_mutex);
    m_camera->setTemperatureSetpoint(point);
}

// ---------------------------------------------------------------------------------------------- //

auto CameraSystem::getCameraStatus() const -> Camera::Status
{
    return m_camera->getStatus();
}

// ---------------------------------------------------------------------------------------------- //

auto CameraSystem::getAvailableCameras() -> std::vector<std::string>
{
    ASSERT(VimbaLoader::isInitialized());

    AVT::VmbAPI::VimbaSystem& system = AVT::VmbAPI::VimbaSystem::GetInstance();

    AVT::VmbAPI::CameraPtrVector cameras;
    VmbErrorType result = system.GetCameras(cameras);

    if (result != VmbErrorSuccess)
        throw CameraException("Unable to get list of available cameras.", result);

    std::vector<std::string> serials;

    for (const auto& camera : cameras)
    {
        VmbInterfaceType interfaceType = {};
        result = camera->GetInterfaceType(interfaceType);

        if (result != VmbErrorSuccess)
            throw CameraException("Unable to get interface type of connected camera.", result);

        std::string name;
        result = camera->GetName(name);

        if (result != VmbErrorSuccess)
            throw CameraException("Unable to get name of connected camera.", result);

        if (interfaceType != VmbInterfaceEthernet || name != "Goldeye")
            continue;

        std::string serial;
        result = camera->GetSerialNumber(serial);

        if (result != VmbErrorSuccess)
            throw CameraException("Unable to get serial number of connected camera.", result);

        serials.push_back(serial);
    }

    return serials;
}

// ---------------------------------------------------------------------------------------------- //

auto CameraSystem::isCameraAvailable(const std::string& serial) -> bool
{
    std::vector<std::string> serials = getAvailableCameras();
    return std::find(serials.begin(), serials.end(), serial) != serials.end();
}

// ---------------------------------------------------------------------------------------------- //
