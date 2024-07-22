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

#include "assertions.h"
#include "controller.h"

#ifdef SKINCAM_BUILD_LEGACY
#include "elapsedtimer.h"
#include "logger.h"
#include <cmath>
#else
#include "endianess.h"
#include <thread>
#endif

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;
using namespace std::string_literals;

// ---------------------------------------------------------------------------------------------- //

#ifdef SKINCAM_BUILD_LEGACY

// ---------------------------------------------------------------------------------------------- //

Controller::Controller(const std::string& port)
    : m_serialPort(port, makeSettings())
{
    std::chrono::milliseconds latency = testLatency();

    if (latency > MaximumSerialLatency)
    {
        std::string msg = "The controller port's serial latency appears to be set to "
                          + std::to_string(latency.count()) + " ms. "
                          "For proper operation the latency timer should be set to the lowest "
                          "value supported by the Controller driver, but to no more than 4 ms.";
        throw Exception(msg);
    }
}

// ---------------------------------------------------------------------------------------------- //

void Controller::setSequenceConfiguration(SequenceConfiguration config)
{
    CLAMP_TO_RANGE(config.illuminationTime, MinimumIlluminationTime, MaximumIlluminationTime);
    CLAMP_TO_RANGE(config.sequenceDelay, MinimumSequenceDelay, MaximumSequenceDelay);

    m_configuration = config;

    const auto sequenceDelayTicks = config.sequenceDelay.count() / 100; // tick = 0.1 ms

    try {
        sendCommand("D" + std::to_string(sequenceDelayTicks));
    }
    catch (const std::exception& e) {
        throw Exception("Unable to set sequence configuration. " + std::string(e.what()));
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::getSequenceConfiguration() const -> SequenceConfiguration
{
    return m_configuration;
}

// ---------------------------------------------------------------------------------------------- //

void Controller::runSequence()
{
    try {
        sendCommand("S");
    }
    catch (const std::exception& e) {
        throw Exception("Unable to run acquisition sequence. " + std::string(e.what()));
    }
}

// ---------------------------------------------------------------------------------------------- //

void Controller::waitSequenceComplete()
{
    try {
        sendCommand("P");
    }
    catch (const std::exception& e) {
        throw Exception("Unable to wait for sequence completion. " + std::string(e.what()));
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::testLatency() -> std::chrono::milliseconds
{
    try {
        int maxLatency = 0;

        for (int i = 0; i < 10; ++i)
        {
            ElapsedTimer timer;
            sendCommand("P");

            auto us = static_cast<int>(timer.getMicroseconds());
            auto ms = static_cast<int>(std::round(us * 0.001));

            maxLatency = std::max(ms, maxLatency);
        }

        Logger::info("Maximum controller latency is " + std::to_string(maxLatency) + " ms.");
        return std::chrono::milliseconds(maxLatency);
    }
    catch (const std::exception& e) {
        throw Exception("Unable to ping controller device. " + std::string(e.what()));
    }
}

// ---------------------------------------------------------------------------------------------- //

void Controller::write(std::string message)
{
    message += '\n';

    const std::vector<uint8_t> data(message.begin(), message.end());
    m_serialPort.sendData(data);
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::read() -> std::string
{
    std::string message;

    while (true)
    {
        m_serialPort.waitForDataAvailable(100ms);

        std::vector<uint8_t> data = m_serialPort.readAllData();

        for (uint8_t c : data)
        {
            if (c == '\n')
                return message;

            message += static_cast<char>(c);
        }

        const size_t maxLength = 16; // Really just 1 ("A" or "E")

        if (message.length() > maxLength)
            break;
    }

    throw Exception("Message received from controller device exceeds maximum length.");
}

// ---------------------------------------------------------------------------------------------- //

void Controller::sendCommand(const std::string& command)
{
    write(command);

    const std::string ack = read();

    if (ack != "A")
    {
        throw Exception("No valid acknowledgement received from controller device. "
                        "Got \"" + ack + "\" instead.");
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::makeSettings() -> SerialPortSettings
{
    SerialPortSettings settings = {};
    settings.baudrate = 1'000'000;

    return settings;
}

// ---------------------------------------------------------------------------------------------- //

#else // !SKINCAM_BUILD_LEGACY

// ---------------------------------------------------------------------------------------------- //

namespace Hardware
{
    constexpr uint16_t VendorId  = 0x0483;
    constexpr uint16_t ProductId = 0x5750;

    constexpr int ConfigurationNumber = 1;
    constexpr int InterfaceNumber     = 0;
    constexpr int AlternateSetting    = 0;

    constexpr uint8_t RequestTypeWrite = 0x21; // Type: class, recipient: interface
    constexpr uint8_t RequestTypeRead  = 0xa1;

    constexpr int TimeoutMilliseconds = 1000;
}

// ---------------------------------------------------------------------------------------------- //

namespace ControlRequest
{
    constexpr uint8_t GetStatus = 0x01;
    constexpr uint8_t GetError = 0x02;

    constexpr uint8_t SetSequenceConfiguration = 0x03;
    constexpr uint8_t GetSequenceConfiguration = 0x04;

    constexpr uint8_t SetPowerConfiguration = 0x05;
    constexpr uint8_t GetPowerConfiguration = 0x06;

    constexpr uint8_t GetPowerStatus = 0x07;

    constexpr uint8_t RunSequence = 0x08;
    constexpr uint8_t WaitSequenceComplete = 0x09;

    constexpr uint8_t ClearErrors = 0x10;
}

// ---------------------------------------------------------------------------------------------- //

namespace Status
{
    constexpr uint8_t Ready = 0;
    constexpr uint8_t Error = 2;
}

// ---------------------------------------------------------------------------------------------- //

namespace ErrorCode
{
    constexpr uint8_t NoError          = 0x00;
    constexpr uint8_t InvalidCommand   = 0x01;
    constexpr uint8_t InvalidLength    = 0x02;
    constexpr uint8_t InvalidParameter = 0x03;
    constexpr uint8_t HardwareFault    = 0x04;
}

// ---------------------------------------------------------------------------------------------- //

static void freeContext(libusb_context* context)
{
    if (context)
        libusb_exit(context);
}

// ---------------------------------------------------------------------------------------------- //

static void freeHandle(libusb_device_handle* handle)
{
    if (handle)
    {
        libusb_release_interface(handle, Hardware::InterfaceNumber);
        libusb_close(handle);
    }
}

// ---------------------------------------------------------------------------------------------- //

using ContextGuard = std::unique_ptr<libusb_context, decltype(&freeContext)>;
using HandleGuard = std::unique_ptr<libusb_device_handle, decltype(&freeHandle)>;

// ---------------------------------------------------------------------------------------------- //

struct PackedSequenceConfiguration
{
    uint32_t illuminationTime;
    uint32_t sequenceDelay;
} __attribute__((__packed__));

// ---------------------------------------------------------------------------------------------- //

struct PackedPowerConfiguration
{
    uint8_t powerLevel0;
    uint8_t powerLevel1;
    uint8_t powerLevel2;
} __attribute__((__packed__));

// ---------------------------------------------------------------------------------------------- //

struct PackedPowerStatus
{
    uint16_t voltage;
    uint16_t current;
} __attribute__((__packed__));

// ---------------------------------------------------------------------------------------------- //

class ControllerException : public std::runtime_error
{
public:
    ControllerException(std::string msg, int error = 0)
        : std::runtime_error(makeMessage(msg, error)) {}

private:
    static auto makeMessage(std::string msg, int error) -> std::string
    {
        if (error < 0)
        {
            msg += "\n\nResult: " + std::to_string(error) +
                   " (" + std::string(libusb_error_name(error)) +
                   ")\nDetails: " + std::string(libusb_strerror(static_cast<libusb_error>(error)));
        }

        return msg;
    }
};

// ---------------------------------------------------------------------------------------------- //

Controller::Controller()
{
    // Initialize libusb
    if (libusb_init(&m_context) < 0)
        throw ControllerException("Unable to initialize libusb.");

    ContextGuard context(m_context, freeContext);

    libusb_set_option(m_context, LIBUSB_OPTION_LOG_LEVEL, 3);

    // Try to open the device
    m_handle = libusb_open_device_with_vid_pid(m_context, Hardware::VendorId, Hardware::ProductId);

    if (!m_handle)
        throw ControllerException("Unable to open the controller device.");

    HandleGuard handle(m_handle, freeHandle);

    // Set a valid configuration
    int result = libusb_set_configuration(m_handle, Hardware::ConfigurationNumber);

    if (result < 0)
        throw ControllerException("Unable to set a configuration.", result);

    // Claim the interface
    result = libusb_claim_interface(m_handle, Hardware::InterfaceNumber);

    if (result < 0)
        throw ControllerException("Unable to claim the interface.", result);

    // Set alternate setting
    result = libusb_set_interface_alt_setting(m_handle, Hardware::InterfaceNumber,
                                                        Hardware::AlternateSetting);
    if (result < 0)
        throw ControllerException("Unable to set an alternate setting.", result);

    clearErrors();

    handle.release();
    context.release();
}

// ---------------------------------------------------------------------------------------------- //

Controller::~Controller()
{
    freeHandle(m_handle);
    freeContext(m_context);
}

// ---------------------------------------------------------------------------------------------- //

void Controller::setSequenceConfiguration(SequenceConfiguration config)
{
    CLAMP_TO_RANGE(config.illuminationTime, MinimumIlluminationTime, MaximumIlluminationTime);
    CLAMP_TO_RANGE(config.sequenceDelay, MinimumSequenceDelay, MaximumSequenceDelay);

    PackedSequenceConfiguration packed = {
        cpu2le<uint32_t>(config.illuminationTime.count()),
        cpu2le<uint32_t>(config.sequenceDelay.count())
    };

    try {
        write(ControlRequest::SetSequenceConfiguration, packed);
        waitReady();
    }
    catch (const std::exception& e) {
        throw ControllerException("Unable to set a new sequence configuration. "s + e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::getSequenceConfiguration() const -> SequenceConfiguration
{
    PackedSequenceConfiguration packed = {};

    try {
        read(ControlRequest::GetSequenceConfiguration, &packed);

        return {
            std::chrono::microseconds(le2cpu(packed.illuminationTime)),
            std::chrono::microseconds(le2cpu(packed.sequenceDelay))
        };
    }
    catch (const std::exception& e) {
        throw ControllerException("Unable to get current sequence configuration. "s + e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void Controller::setPowerConfiguration(PowerConfiguration config)
{
    CLAMP_TO_RANGE(config.powerLevel0, MinimumPowerLevel, MaximumPowerLevel);
    CLAMP_TO_RANGE(config.powerLevel1, MinimumPowerLevel, MaximumPowerLevel);
    CLAMP_TO_RANGE(config.powerLevel2, MinimumPowerLevel, MaximumPowerLevel);

    PackedPowerConfiguration packed = {
        static_cast<uint8_t>(config.powerLevel0),
        static_cast<uint8_t>(config.powerLevel1),
        static_cast<uint8_t>(config.powerLevel2)
    };

    try {
        write(ControlRequest::SetPowerConfiguration, packed);
        waitReady();
    }
    catch (const std::exception& e) {
        throw ControllerException("Unable to set a new power configuration. "s + e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::getPowerConfiguration() const -> PowerConfiguration
{
    PackedPowerConfiguration packed = {};

    try {
        read(ControlRequest::GetPowerConfiguration, &packed);

        return {
            packed.powerLevel0,
            packed.powerLevel1,
            packed.powerLevel2
        };
    }
    catch (const std::exception& e) {
        throw ControllerException("Unable to get current power configuration. "s + e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::getPowerStatus() const -> PowerStatus
{
    PackedPowerStatus packed = {};

    try {
        read(ControlRequest::GetPowerStatus, &packed);

        return {
            le2cpu(packed.voltage) * 0.001,
            le2cpu(packed.current) * 0.001
        };
    }
    catch (const std::exception& e) {
        throw ControllerException("Unable to get current power status. "s + e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void Controller::runSequence()
{
    try {
        sendCommand(ControlRequest::RunSequence);
        waitReady();
    }
    catch (const std::exception& e) {
        throw ControllerException("Unable to start a new sequence. "s + e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void Controller::waitSequenceComplete()
{
    try {
        sendCommand(ControlRequest::WaitSequenceComplete);
        waitReady();
    }
    catch (const std::exception& e) {
        throw ControllerException("Previous sequence failed. "s + e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void Controller::clearErrors()
{
    try {
        sendCommand(ControlRequest::ClearErrors);
        waitReady();
    }
    catch (const std::exception& e) {
        throw ControllerException("Unable to clear errors. "s + e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::getStatus() const -> uint8_t
{
    uint8_t status = 0;

    try {
        read(ControlRequest::GetStatus, &status);
        return status;
    }
    catch (const std::exception& e) {
        throw ControllerException("Unable to get current status. "s + e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::getError() const -> uint8_t
{
    uint8_t error = 0;

    try {
        read(ControlRequest::GetError, &error);
        return error;
    }
    catch (const std::exception& e) {
        throw ControllerException("Unable to get current error code. "s + e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void Controller::write(uint8_t request, const T& data)
{
    auto buffer = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(&data));

    int result = libusb_control_transfer(m_handle, Hardware::RequestTypeWrite,
                                         request, 0, 0, buffer, sizeof(T),
                                         Hardware::TimeoutMilliseconds);
    if (result != sizeof(T))
        throw ControllerException("USB control transfer failed.", result);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void Controller::read(uint8_t request, T* data) const
{
    auto buffer = reinterpret_cast<unsigned char*>(data);

    int result = libusb_control_transfer(m_handle, Hardware::RequestTypeRead,
                                         request, 0, 0, buffer, sizeof(T),
                                         Hardware::TimeoutMilliseconds);
    if (result != sizeof(T))
        throw ControllerException("USB control transfer failed.", result);
}

// ---------------------------------------------------------------------------------------------- //

void Controller::sendCommand(uint8_t request)
{
    int result = libusb_control_transfer(m_handle, Hardware::RequestTypeWrite,
                                         request, 0, 0, nullptr, 0,
                                         Hardware::TimeoutMilliseconds);
    if (result < 0)
        throw ControllerException("USB control transfer failed.", result);
}

// ---------------------------------------------------------------------------------------------- //

void Controller::waitReady() const
{
    constexpr std::chrono::microseconds Delay = 500us;
    std::chrono::microseconds timeout = 100ms;

    while (timeout.count() > 0)
    {
        uint8_t status = getStatus();

        if (status == Status::Ready)
            return;

        if (status == Status::Error)
        {
            const auto error = toErrorString(getError());
            throw ControllerException("An internal device error occurred (" + error + ").");
        }

        std::this_thread::sleep_for(Delay);
        timeout += Delay;
    }

    throw ControllerException("An operation took longer than expected. This should never happen.");
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::toErrorString(uint8_t error) -> std::string
{
    if (error == ErrorCode::NoError)
        return "no error";

    if (error == ErrorCode::InvalidCommand)
        return "invalid command";

    if (error == ErrorCode::InvalidLength)
        return "invalid length";

    if (error == ErrorCode::InvalidParameter)
        return "invalid parameter";

    if (error == ErrorCode::HardwareFault)
        return "hardware fault";

    return "unknown error";
}

// ---------------------------------------------------------------------------------------------- //

#endif // SKINCAM_BUILD_LEGACY

// ---------------------------------------------------------------------------------------------- //
