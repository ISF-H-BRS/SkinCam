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

#include "delay.h"
#include "ltc2945.h"

// ---------------------------------------------------------------------------------------------- //

namespace Registers
{
    constexpr uint8_t Control = 0x00;
    constexpr uint8_t Alert = 0x01;

    constexpr uint8_t Voltage = 0x1e;
    constexpr uint8_t VoltageMaxThreshold = 0x24;
    constexpr uint8_t VoltageMinThreshold = 0x26;

    constexpr uint8_t Current = 0x14;
    constexpr uint8_t CurrentMaxThreshold = 0x1a;
    constexpr uint8_t CurrentMinThreshold = 0x1c;

    constexpr uint8_t Power = 0x05;
    constexpr uint8_t PowerMaxThreshold = 0x0e;
    constexpr uint8_t PowerMinThreshold = 0x11;
}

// ---------------------------------------------------------------------------------------------- //

namespace Masks
{
    constexpr uint8_t DefaultControl = 0x05;

    constexpr uint8_t ShutdownEnable = (1<<1);
    constexpr uint8_t TestModeEnable = (1<<4);

    constexpr uint8_t VoltageMaxAlert = (1<<3);
    constexpr uint8_t VoltageMinAlert = (1<<2);

    constexpr uint8_t CurrentMaxAlert = (1<<5);
    constexpr uint8_t CurrentMinAlert = (1<<4);

    constexpr uint8_t PowerMaxAlert = (1<<7);
    constexpr uint8_t PowerMinAlert = (1<<6);
}

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr float MaximumVoltage = 102.4F;
    constexpr float ReferenceVoltage = 0.1024F;

    constexpr float MaximumVoltageValue = (1<<12) - 1;
    constexpr float MaximumCurrentValue = (1<<12) - 1;
    constexpr float MaximumPowerValue = (1<<24) - 1;

    constexpr uint32_t Timeout = 100;
}

// ---------------------------------------------------------------------------------------------- //

Ltc2945::Ltc2945(I2C_HandleTypeDef* i2c, Address address, float resistance)
    : m_i2c(i2c),
      m_address(static_cast<uint8_t>(address)),
      m_resistance(resistance)
{
}

// ---------------------------------------------------------------------------------------------- //

void Ltc2945::reset()
{
    writeRegister8(Registers::Control, Masks::DefaultControl);
}

// ---------------------------------------------------------------------------------------------- //

void Ltc2945::setEnabled(bool enable)
{
    const uint8_t reg = Registers::Control;
    const uint8_t mask = Masks::ShutdownEnable | Masks::TestModeEnable;

    const uint8_t oldValue = readRegister8(reg);
    const uint8_t newValue = enable ? (oldValue & ~mask) : (oldValue | mask);

    if (newValue != oldValue)
    {
        writeRegister8(reg, newValue);

        // Chip only samples at approx. 7.5 Hz, so give it time to
        // complete the first conversion to avoid reading garbage
        if (enable)
            Delay::wait(200ms);
    }
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::isEnabled() const -> bool
{
    const uint8_t val = readRegister8(Registers::Control);
    return !(val & Masks::ShutdownEnable);
}

// ---------------------------------------------------------------------------------------------- //

void Ltc2945::setVoltageLimits(float lower, float upper)
{
    writeRegister12(Registers::VoltageMinThreshold, voltageToValue(lower));
    writeRegister12(Registers::VoltageMaxThreshold, voltageToValue(upper));

    const uint8_t val = readRegister8(Registers::Alert);
    writeRegister8(Registers::Alert, val | Masks::VoltageMinAlert | Masks::VoltageMaxAlert);
}

// ---------------------------------------------------------------------------------------------- //

void Ltc2945::setCurrentLimits(float lower, float upper)
{
    writeRegister12(Registers::CurrentMinThreshold, currentToValue(lower));
    writeRegister12(Registers::CurrentMaxThreshold, currentToValue(upper));

    const uint8_t val = readRegister8(Registers::Alert);
    writeRegister8(Registers::Alert, val | Masks::CurrentMinAlert | Masks::CurrentMaxAlert);
}

// ---------------------------------------------------------------------------------------------- //

void Ltc2945::setPowerLimits(float lower, float upper)
{
    writeRegister24(Registers::PowerMinThreshold, powerToValue(lower));
    writeRegister24(Registers::PowerMaxThreshold, powerToValue(upper));

    const uint8_t val = readRegister8(Registers::Alert);
    writeRegister8(Registers::Alert, val | Masks::PowerMinAlert | Masks::PowerMaxAlert);
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::getVoltage() const -> float
{
    return valueToVoltage(readRegister12(Registers::Voltage));
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::getCurrent() const -> float
{
    return valueToCurrent(readRegister12(Registers::Current));
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::getPower() const -> float
{
    return valueToPower(readRegister24(Registers::Power));
}

// ---------------------------------------------------------------------------------------------- //

void Ltc2945::writeRegister(uint8_t reg, std::span<const uint8_t> buffer)
{
    auto status = HAL_I2C_Mem_Write(m_i2c, m_address, reg, I2C_MEMADD_SIZE_8BIT,
                                    const_cast<uint8_t*>(buffer.data()), buffer.size(), Timeout);
    if (status != HAL_OK)
        throw WriteError();
}

// ---------------------------------------------------------------------------------------------- //

void Ltc2945::readRegister(uint8_t reg, std::span<uint8_t> buffer) const
{
    auto status = HAL_I2C_Mem_Read(m_i2c, m_address, reg, I2C_MEMADD_SIZE_8BIT,
                                   buffer.data(), buffer.size(), Timeout);
    if (status != HAL_OK)
        throw ReadError();
}

// ---------------------------------------------------------------------------------------------- //

void Ltc2945::writeRegister8(uint8_t reg, uint8_t data)
{
    const std::array<uint8_t, 1> buffer = { data };
    writeRegister(reg, buffer);
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::readRegister8(uint8_t reg) const -> uint8_t
{
    std::array<uint8_t, 1> buffer = {};
    readRegister(reg, buffer);

    return buffer[0];
}

// ---------------------------------------------------------------------------------------------- //

void Ltc2945::writeRegister12(uint8_t reg, uint16_t data)
{
    const std::array<uint8_t, 2> buffer = {
        static_cast<uint8_t>(data>>4),
        static_cast<uint8_t>(data<<4)
    };

    writeRegister(reg, buffer);
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::readRegister12(uint8_t reg) const -> uint16_t
{
    std::array<uint8_t, 2> buffer = {};
    readRegister(reg, buffer);

    return (buffer[0]<<4) | (buffer[1]>>4);
}

// ---------------------------------------------------------------------------------------------- //

void Ltc2945::writeRegister24(uint8_t reg, uint32_t data)
{
    const std::array<uint8_t, 3> buffer = {
        static_cast<uint8_t>(data>>16),
        static_cast<uint8_t>(data>>8),
        static_cast<uint8_t>(data)
    };

    writeRegister(reg, buffer);
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::readRegister24(uint8_t reg) const -> uint32_t
{
    std::array<uint8_t, 3> buffer = {};
    readRegister(reg, buffer);

    return (buffer[0]<<16) | (buffer[1]<<8) | buffer[2];
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::voltageToValue(float voltage) const -> uint16_t
{
    return static_cast<uint16_t>(MaximumVoltageValue / MaximumVoltage * voltage);
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::valueToVoltage(uint16_t value) const -> float
{
    return MaximumVoltage / MaximumVoltageValue * static_cast<float>(value);
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::currentToValue(float current) const -> uint16_t
{
    const float maxCurrent = ReferenceVoltage / m_resistance;
    return static_cast<uint16_t>(MaximumCurrentValue / maxCurrent * current);
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::valueToCurrent(uint16_t value) const -> float
{
    const float maxCurrent = ReferenceVoltage / m_resistance;
    return maxCurrent / MaximumCurrentValue * static_cast<float>(value);
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::powerToValue(float power) const -> uint32_t
{
    const float maxCurrent = ReferenceVoltage / m_resistance;
    const float maxPower = MaximumVoltage * maxCurrent;

    return static_cast<uint32_t>(MaximumPowerValue / maxPower * power);
}

// ---------------------------------------------------------------------------------------------- //

auto Ltc2945::valueToPower(uint32_t value) const -> float
{
    const float maxCurrent = ReferenceVoltage / m_resistance;
    const float maxPower = MaximumVoltage * maxCurrent;

    return maxPower / MaximumPowerValue * static_cast<float>(value);
}

// ---------------------------------------------------------------------------------------------- //
