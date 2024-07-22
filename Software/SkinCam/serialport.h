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

#include "namespace.h"

#include <array>
#include <chrono>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------- //

using namespace std::chrono_literals;

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

struct SerialPortSettings
{
    size_t baudrate = 115200;
};

// ---------------------------------------------------------------------------------------------- //

class SerialPort
{
public:
    static constexpr std::chrono::milliseconds DefaultTimeout = 500ms;

    using Error = std::runtime_error;

public:
    SerialPort(const std::string& port, const SerialPortSettings& settings = {});

    SerialPort(const SerialPort&) = delete;
    SerialPort(SerialPort&& other);

    ~SerialPort();

    auto operator=(const SerialPort& other) = delete;
    auto operator=(SerialPort&& other) -> SerialPort&;

    auto port() const -> std::string { return m_port; }

    void sendData(std::span<const uint8_t> data);

    auto waitForDataAvailable(std::chrono::milliseconds timeout = DefaultTimeout) -> bool;
    auto readAllData() -> std::vector<uint8_t>;

    auto getNumberOfBytesAvailable() const -> size_t;

private:
    void move(SerialPort&& other);
    void close();

    void throwSystemError(std::string error, int code = 0) const;

private:
    std::string m_port;

    struct Private;
    std::unique_ptr<Private> d;
};

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
