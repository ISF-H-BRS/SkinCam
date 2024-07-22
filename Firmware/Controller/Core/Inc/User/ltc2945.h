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

#include "main.h"

#include <exception>
#include <span>

class Ltc2945
{
public:
    enum class Address : uint8_t
    {
        A0 = 0xce,
        A1 = 0xd0,
        A2 = 0xd2,
        A3 = 0xd4,
        A4 = 0xd6,
        A5 = 0xd8,
        A6 = 0xda,
        A7 = 0xdc,
        A8 = 0xde
    };

    static constexpr uint8_t AddressCount = 9;

    using ReadError = std::exception;
    using WriteError = std::exception;

public:
    Ltc2945(I2C_HandleTypeDef* i2c, Address address, float resistance);

    void reset();

    void setEnabled(bool enable);
    auto isEnabled() const -> bool;

    void setVoltageLimits(float lower, float upper);
    void setCurrentLimits(float lower, float upper);
    void setPowerLimits(float lower, float upper);

    auto getVoltage() const -> float;
    auto getCurrent() const -> float;
    auto getPower() const -> float;

private:
    void writeRegister(uint8_t reg, std::span<const uint8_t> buffer);
    void readRegister(uint8_t reg, std::span<uint8_t> buffer) const;

    void writeRegister8(uint8_t reg, uint8_t data);
    auto readRegister8(uint8_t reg) const -> uint8_t;

    void writeRegister12(uint8_t reg, uint16_t data);
    auto readRegister12(uint8_t reg) const -> uint16_t;

    void writeRegister24(uint8_t reg, uint32_t data);
    auto readRegister24(uint8_t reg) const -> uint32_t;

    auto voltageToValue(float voltage) const -> uint16_t;
    auto valueToVoltage(uint16_t value) const -> float;

    auto currentToValue(float current) const -> uint16_t;
    auto valueToCurrent(uint16_t value) const -> float;

    auto powerToValue(float power) const -> uint32_t;
    auto valueToPower(uint32_t value) const -> float;

private:
    I2C_HandleTypeDef* m_i2c;
    const uint8_t m_address;
    const float m_resistance;
};
