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

#pragma once

#include "exception.h"

#ifdef SKINCAM_BUILD_LEGACY
#include "serialport.h"
#else
#include <libusb-1.0/libusb.h>
#endif

#include <chrono>
#include <memory>

// ---------------------------------------------------------------------------------------------- //

using namespace std::chrono_literals;

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

class Controller;
using ControllerPtr = std::unique_ptr<Controller>;

// ---------------------------------------------------------------------------------------------- //

#ifdef SKINCAM_BUILD_LEGACY

// ---------------------------------------------------------------------------------------------- //

class Controller
{
public:
    static constexpr std::chrono::microseconds MinimumIlluminationTime = 10ms;
    static constexpr std::chrono::microseconds MaximumIlluminationTime = 10ms;
    static constexpr std::chrono::microseconds DefaultIlluminationTime = 10ms;

    static constexpr std::chrono::microseconds MinimumSequenceDelay =  0ms;
    static constexpr std::chrono::microseconds MaximumSequenceDelay = 60ms;
    static constexpr std::chrono::microseconds DefaultSequenceDelay =  0ms;

    static constexpr std::chrono::microseconds MinimumSequenceDuration =  40ms;
    static constexpr std::chrono::microseconds MaximumSequenceDuration = 100ms;
    static constexpr std::chrono::microseconds DefaultSequenceDuration =  40ms;

    static constexpr std::chrono::milliseconds MaximumSerialLatency = 4ms;

    struct SequenceConfiguration
    {
        std::chrono::microseconds illuminationTime = DefaultIlluminationTime;
        std::chrono::microseconds sequenceDelay = DefaultSequenceDelay;
    };

public:
    Controller(const std::string& port);

    void setSequenceConfiguration(SequenceConfiguration config);
    auto getSequenceConfiguration() const -> SequenceConfiguration;

    void runSequence();
    void waitSequenceComplete();

    auto testLatency() -> std::chrono::milliseconds;

private:
    void write(std::string message);
    auto read() -> std::string;

    void sendCommand(const std::string& command);

    static auto makeSettings() -> SerialPortSettings;

private:
    SerialPort m_serialPort;
    SequenceConfiguration m_configuration;
};

// ---------------------------------------------------------------------------------------------- //

#else // !SKINCAM_BUILD_LEGACY

// ---------------------------------------------------------------------------------------------- //

class Controller
{
public:
    static constexpr std::chrono::microseconds MinimumIlluminationTime =  4ms;
    static constexpr std::chrono::microseconds MaximumIlluminationTime = 10ms;
    static constexpr std::chrono::microseconds DefaultIlluminationTime =  5ms;

    static constexpr std::chrono::microseconds MinimumSequenceDelay =   0ms;
    static constexpr std::chrono::microseconds MaximumSequenceDelay = 100ms;
    static constexpr std::chrono::microseconds DefaultSequenceDelay =   5ms;

    static constexpr std::chrono::microseconds MinimumSequenceDuration =  25ms;
    static constexpr std::chrono::microseconds MaximumSequenceDuration = 100ms;
    static constexpr std::chrono::microseconds DefaultSequenceDuration =  25ms;

    static constexpr int MinimumPowerLevel =   0;
    static constexpr int MaximumPowerLevel = 100;
    static constexpr int DefaultPowerLevel =  60;

    struct SequenceConfiguration
    {
        std::chrono::microseconds illuminationTime = DefaultIlluminationTime;
        std::chrono::microseconds sequenceDelay = DefaultSequenceDelay;
    };

    struct PowerConfiguration
    {
        int powerLevel0 = DefaultPowerLevel;
        int powerLevel1 = DefaultPowerLevel;
        int powerLevel2 = DefaultPowerLevel;
    };

    struct PowerStatus
    {
        double voltage = 0.0;
        double current = 0.0;
    };

public:
    Controller();
    ~Controller();

    void setSequenceConfiguration(SequenceConfiguration config);
    auto getSequenceConfiguration() const -> SequenceConfiguration;

    void setPowerConfiguration(PowerConfiguration config);
    auto getPowerConfiguration() const -> PowerConfiguration;

    auto getPowerStatus() const -> PowerStatus;

    void runSequence();
    void waitSequenceComplete();

    void clearErrors();

private:
    template <typename T>
    void write(uint8_t request, const T& data);

    template <typename T>
    void read(uint8_t request, T* data) const;

    void sendCommand(uint8_t request);

    void waitReady() const;

    auto getStatus() const -> uint8_t;
    auto getError() const -> uint8_t;

    static auto toErrorString(uint8_t error) -> std::string;

private:
    libusb_context* m_context = nullptr;
    libusb_device_handle* m_handle = nullptr;
};

// ---------------------------------------------------------------------------------------------- //

#endif // SKINCAM_BUILD_LEGACY

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
