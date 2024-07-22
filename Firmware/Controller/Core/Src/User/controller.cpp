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

#include "assert.h"
#include "controller.h"
#include "delay.h"

#include <algorithm>
#include <numeric>

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr auto PowerMonitorHandle = Config::PowerMonitorHandle;
    constexpr auto PowerMonitorAddress = Config::PowerMonitorAddress;
    constexpr auto PowerMonitorResistance = Config::PowerMonitorResistance;

    volatile bool g_monitorPeriodElapsed = false;

    volatile bool g_activityTimerActive = false;
    volatile bool g_activityFlag = false;
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    ASSERT(htim == Config::MonitorTimerHandle || htim == Config::ActivityTimerHandle);

    if (htim == Config::MonitorTimerHandle)
        g_monitorPeriodElapsed = true;
    else if (htim == Config::ActivityTimerHandle)
    {
        if (g_activityFlag)
        {
            HAL_GPIO_TogglePin(STATUS_GOOD_GPIO_Port, STATUS_GOOD_Pin);
            g_activityFlag = false;
        }
        else
        {
            HAL_GPIO_WritePin(STATUS_GOOD_GPIO_Port, STATUS_GOOD_Pin, GPIO_PIN_RESET);
            HAL_TIM_Base_Stop_IT(htim);
            g_activityTimerActive = false;
        }
    }
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto inRange(T x, T min, T max) -> bool
{
    return x >= min && x <= max;
}

// ---------------------------------------------------------------------------------------------- //

Controller::Controller()
    : m_hostInterface(this),
      m_powerMonitor(PowerMonitorHandle, PowerMonitorAddress, PowerMonitorResistance)
{
}

// ---------------------------------------------------------------------------------------------- //

void Controller::exec()
{
    HAL_GPIO_WritePin(LED_ENABLE_GPIO_Port, LED_ENABLE_Pin, GPIO_PIN_SET);

    // Wait for LED power to become stable and
    // power monitor to complete first sample
    Delay::wait(500ms);

    HAL_TIM_Base_Start_IT(Config::MonitorTimerHandle);

    HAL_GPIO_WritePin(STATUS_GOOD_GPIO_Port, STATUS_GOOD_Pin, GPIO_PIN_RESET);

    while (true)
    {
        switch (m_currentTask)
        {
        case Task::ApplySequenceConfiguration:
            processApplySequenceConfiguration();
            break;

        case Task::ApplyPowerConfiguration:
            processApplyPowerConfiguration();
            break;

        case Task::RunSequence:
            processRunSequence();
            break;

        case Task::WaitSequenceComplete:
            processWaitSequenceComplete();
            break;

        case Task::ClearErrors:
            processClearErrors();
            break;

        default:
            break;
        }

        if (g_monitorPeriodElapsed)
        {
            g_monitorPeriodElapsed = false;
            updatePowerStatus();
        }
    }
}

// ---------------------------------------------------------------------------------------------- //

void Controller::onInitComplete()
{
    m_status = Status::Ready;
}

// ---------------------------------------------------------------------------------------------- //

void Controller::onControlOutReceived(uint8_t request, const uint8_t* data, uint16_t length)
{
    if (m_status == Status::Busy)
        return;

    m_status = Status::Busy;
    m_error = Error::NoError;

    if (request == ControlRequest::SetSequenceConfiguration)
    {
        if (length == sizeof(SequenceConfiguration))
        {
            const auto& config = *reinterpret_cast<const SequenceConfiguration*>(data);

            if (configurationValid(config))
            {
                m_sequenceConfiguration = config;
                m_currentTask = Task::ApplySequenceConfiguration;
            }
            else
                m_error = Error::InvalidParameter;
        }
        else
            m_error = Error::InvalidLength;
    }
    else if (request == ControlRequest::SetPowerConfiguration)
    {
        if (length == sizeof(PowerConfiguration))
        {
            const auto& config = *reinterpret_cast<const PowerConfiguration*>(data);

            if (configurationValid(config))
            {
                m_powerConfiguration = config;
                m_currentTask = Task::ApplyPowerConfiguration;
            }
            else
                m_error = Error::InvalidParameter;
        }
        else
            m_error = Error::InvalidLength;
    }
    else if (request == ControlRequest::RunSequence)
    {
        if (length == 0)
            m_currentTask = Task::RunSequence;
        else
            m_error = Error::InvalidLength;
    }
    else if (request == ControlRequest::WaitSequenceComplete)
    {
        if (length == 0)
            m_currentTask = Task::WaitSequenceComplete;
        else
            m_error = Error::InvalidLength;
    }
    else if (request == ControlRequest::ClearErrors)
    {
        if (length == 0)
            m_currentTask = Task::ClearErrors;
        else
            m_error = Error::InvalidLength;
    }
    else
        m_error = Error::InvalidCommand;

    if (m_error != Error::NoError)
        m_status = Status::Error;
}

// ---------------------------------------------------------------------------------------------- //

void Controller::onControlInReceived(uint8_t request, uint8_t* data, uint16_t length)
{
    if (request == ControlRequest::GetStatus)
    {
        auto status = static_cast<uint8_t>(m_status);

        if (length == sizeof(status))
        {
            std::copy_n(&status, length, data);
            return;
        }
    }
    else if (request == ControlRequest::GetError)
    {
        auto error = static_cast<uint8_t>(m_error);

        if (length == sizeof(error))
        {
            std::copy_n(&error, length, data);
            return;
        }
    }
    else if (request == ControlRequest::GetSequenceConfiguration)
    {
        if (length == sizeof(m_sequenceConfiguration))
        {
            std::copy_n(reinterpret_cast<uint8_t*>(&m_sequenceConfiguration), length, data);
            return;
        }
    }
    else if (request == ControlRequest::GetPowerConfiguration)
    {
        if (length == sizeof(m_powerConfiguration))
        {
            std::copy_n(reinterpret_cast<uint8_t*>(&m_powerConfiguration), length, data);
            return;
        }
    }
    else if (request == ControlRequest::GetPowerStatus)
    {
        if (length == sizeof(m_powerStatus))
        {
            std::copy_n(reinterpret_cast<uint8_t*>(&m_powerStatus), length, data);
            return;
        }
    }
    else // Invalid request, or invalid length
        std::fill_n(data, length, 0);
}

// ---------------------------------------------------------------------------------------------- //

void Controller::processApplySequenceConfiguration()
{
    m_illuminationTime = std::chrono::microseconds(m_sequenceConfiguration.illuminationTime);
    m_sequenceDelay = std::chrono::microseconds(m_sequenceConfiguration.sequenceDelay);

    m_currentTask = Task::None;
    m_status = Status::Ready;
}

// ---------------------------------------------------------------------------------------------- //

void Controller::processApplyPowerConfiguration()
{
    auto handle = Config::DimmingTimerHandle;

    __HAL_TIM_SET_COMPARE(handle, TIM_CHANNEL_1, m_powerConfiguration.powerLevel0);
    __HAL_TIM_SET_COMPARE(handle, TIM_CHANNEL_2, m_powerConfiguration.powerLevel1);
    __HAL_TIM_SET_COMPARE(handle, TIM_CHANNEL_3, m_powerConfiguration.powerLevel2);
    __HAL_TIM_SET_COMPARE(handle, TIM_CHANNEL_4, m_powerConfiguration.powerLevel2);

    m_currentTask = Task::None;
    m_status = Status::Ready;
}

// ---------------------------------------------------------------------------------------------- //

void Controller::processRunSequence()
{
    static constexpr std::array<uint32_t, 0> Dark = {};
    static constexpr std::array<uint32_t, 1> Wavelength0 = { TIM_CHANNEL_1 };
    static constexpr std::array<uint32_t, 1> Wavelength1 = { TIM_CHANNEL_2 };
    static constexpr std::array<uint32_t, 2> Wavelength2 = { TIM_CHANNEL_3, TIM_CHANNEL_4 };

    m_currentTask = Task::None;

    if (m_hardwareFaultOccurred)
    {
        m_error = Error::HardwareFault;
        m_status = Status::Error;
        return;
    }

    m_status = Status::Ready;

    try {
        captureChannels(Dark);
        captureChannels(Wavelength0);
        captureChannels(Wavelength1);
        captureChannels(Wavelength2);

        g_activityFlag = true;

        if (!g_activityTimerActive)
        {
            HAL_TIM_Base_Start_IT(Config::ActivityTimerHandle);
            g_activityTimerActive = true;
        }

        Delay::wait(m_sequenceDelay);
    }
    catch (const std::exception&) {
        m_hardwareFaultOccurred = true;
    }
}

// ---------------------------------------------------------------------------------------------- //

void Controller::processWaitSequenceComplete()
{
    m_currentTask = Task::None;

    if (m_hardwareFaultOccurred)
    {
        m_error = Error::HardwareFault;
        m_status = Status::Error;
    }
    else
        m_status = Status::Ready;
}

// ---------------------------------------------------------------------------------------------- //

void Controller::processClearErrors()
{
    m_hardwareFaultOccurred = false;

    m_currentTask = Task::None;
    m_status = Status::Ready;
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::configurationValid(const SequenceConfiguration& config) const -> bool
{
    return inRange(config.illuminationTime, MinimumIlluminationTime, MaximumIlluminationTime) &&
           inRange(config.sequenceDelay, MinimumSequenceDelay, MaximumSequenceDelay);
}

// ---------------------------------------------------------------------------------------------- //

auto Controller::configurationValid(const PowerConfiguration& config) const -> bool
{
    return inRange(config.powerLevel0, MinimumPowerLevel, MaximumPowerLevel) &&
           inRange(config.powerLevel1, MinimumPowerLevel, MaximumPowerLevel) &&
           inRange(config.powerLevel2, MinimumPowerLevel, MaximumPowerLevel);
}

// ---------------------------------------------------------------------------------------------- //

void Controller::waitTriggerReady(std::chrono::microseconds us)
{
    const auto ready = []{
        return HAL_GPIO_ReadPin(FEEDBACK_TTL_GPIO_Port, FEEDBACK_TTL_Pin) == GPIO_PIN_SET;
    };

    if (!Delay::wait(us, ready))
        throw Exception();
}

// ---------------------------------------------------------------------------------------------- //

void Controller::captureChannels(std::span<const uint32_t> channels)
{
    static constexpr std::chrono::milliseconds TriggerWaitDuration = 10ms;
    static constexpr std::chrono::milliseconds TriggerHighDuration = 1ms;

    waitTriggerReady(TriggerWaitDuration);

    for (uint32_t channel : channels)
        HAL_TIM_PWM_Start(Config::DimmingTimerHandle, channel);

    HAL_GPIO_WritePin(TRIGGER_TTL_GPIO_Port, TRIGGER_TTL_Pin, GPIO_PIN_SET);
    Delay::wait(TriggerHighDuration);

    HAL_GPIO_WritePin(TRIGGER_TTL_GPIO_Port, TRIGGER_TTL_Pin, GPIO_PIN_RESET);
    Delay::wait(m_illuminationTime - TriggerHighDuration);

    for (uint32_t channel : channels)
        HAL_TIM_PWM_Stop(Config::DimmingTimerHandle, channel);
}

// ---------------------------------------------------------------------------------------------- //

void Controller::updatePowerStatus()
{
    constexpr uint16_t MaximumVoltage = 13000; // mV
    constexpr uint16_t MaximumCurrent =   500; // mA

    constexpr int RetryCount = 3;

    for (int i = 0; i < RetryCount; ++i)
    {
        try {
            float voltage = m_powerMonitor.getVoltage();
            float current = m_powerMonitor.getCurrent();

            m_voltageBuffer[m_powerBufferPosition] = voltage;
            m_currentBuffer[m_powerBufferPosition] = current;

            if (++m_powerBufferPosition >= PowerBufferSize)
                m_powerBufferPosition = 0;

            voltage = std::accumulate(m_voltageBuffer.begin(), m_voltageBuffer.end(), 0.0F);
            current = std::accumulate(m_currentBuffer.begin(), m_currentBuffer.end(), 0.0F);

            m_powerStatus.voltage = static_cast<uint16_t>(1000.0F * voltage / PowerBufferSize);
            m_powerStatus.current = static_cast<uint16_t>(1000.0F * current / PowerBufferSize);

            if (m_powerStatus.voltage > MaximumVoltage || m_powerStatus.current > MaximumCurrent)
                break;

            return;
        }
        catch (const std::exception&) {
            ++m_powerErrorCount;
        }
    }

    HAL_GPIO_WritePin(LED_ENABLE_GPIO_Port, LED_ENABLE_Pin, GPIO_PIN_RESET);
    ASSERT(false);
}

// ---------------------------------------------------------------------------------------------- //
