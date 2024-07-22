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

#include "hostinterface.h"
#include "ltc2945.h"

#include <array>
#include <chrono>
#include <span>

using namespace std::chrono_literals;

class Controller : public HostInterface::Owner
{
public:
    enum class Status : uint8_t
    {
        Ready = 0,
        Busy  = 1,
        Error = 2
    };

    enum class Error : uint8_t
    {
        NoError          = 0x00,
        InvalidCommand   = 0x01,
        InvalidLength    = 0x02,
        InvalidParameter = 0x03,
        HardwareFault    = 0x04
    };

    static constexpr size_t ChannelCount = 4; // 3 wavelengths + dark

    static constexpr uint32_t MinimumIlluminationTime =  4'000;
    static constexpr uint32_t MaximumIlluminationTime = 10'000;
    static constexpr uint32_t DefaultIlluminationTime =  5'000;

    static constexpr uint32_t MinimumSequenceDelay =       0;
    static constexpr uint32_t MaximumSequenceDelay = 100'000;
    static constexpr uint32_t DefaultSequenceDelay =       0;

    static constexpr uint8_t MinimumPowerLevel =   0;
    static constexpr uint8_t MaximumPowerLevel = 100;
    static constexpr uint8_t DefaultPowerLevel =  60;

    struct SequenceConfiguration
    {
        uint32_t illuminationTime = DefaultIlluminationTime;
        uint32_t sequenceDelay = DefaultSequenceDelay;
    } __attribute__((__packed__));

    struct PowerConfiguration
    {
        uint8_t powerLevel0 = DefaultPowerLevel;
        uint8_t powerLevel1 = DefaultPowerLevel;
        uint8_t powerLevel2 = DefaultPowerLevel;
    } __attribute__((__packed__));

    struct PowerStatus
    {
        uint16_t voltage = 0;
        uint16_t current = 0;
    } __attribute__((__packed__));

public:
    Controller();

    void exec();

private:
    void onInitComplete() override;

    void onControlOutReceived(uint8_t request, const uint8_t* data, uint16_t length) override;
    void onControlInReceived(uint8_t request, uint8_t* data, uint16_t length) override;

    void processApplySequenceConfiguration();
    void processApplyPowerConfiguration();
    void processRunSequence();
    void processWaitSequenceComplete();
    void processClearErrors();

    auto configurationValid(const SequenceConfiguration& config) const -> bool;
    auto configurationValid(const PowerConfiguration& config) const -> bool;

    void waitTriggerReady(std::chrono::microseconds us);
    void captureChannels(std::span<const uint32_t> channels);

    void updatePowerStatus();

private:
    using Exception = std::exception;

private:
    HostInterface m_hostInterface;
    Ltc2945 m_powerMonitor;

    enum class Task
    {
        None,
        ApplySequenceConfiguration,
        ApplyPowerConfiguration,
        RunSequence,
        WaitSequenceComplete,
        ClearErrors

    } m_currentTask = Task::None;

    bool m_hardwareFaultOccurred = false;

    Status m_status = Status::Busy;
    Error m_error = Error::NoError;

    SequenceConfiguration m_sequenceConfiguration;
    PowerConfiguration m_powerConfiguration;
    PowerStatus m_powerStatus;

    std::chrono::microseconds m_illuminationTime { DefaultIlluminationTime };
    std::chrono::microseconds m_sequenceDelay { DefaultSequenceDelay };

    static constexpr size_t PowerBufferSize = 5;
    std::array<float, PowerBufferSize> m_voltageBuffer = {};
    std::array<float, PowerBufferSize> m_currentBuffer = {};

    uint32_t m_powerBufferPosition = 0;
    uint32_t m_powerErrorCount = 0;
};
