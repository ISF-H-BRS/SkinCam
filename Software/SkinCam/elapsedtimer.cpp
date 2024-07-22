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

#include "elapsedtimer.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

ElapsedTimer::ElapsedTimer()
    : m_start(std::chrono::steady_clock::now())
{
}

// ---------------------------------------------------------------------------------------------- //

void ElapsedTimer::restart()
{
    m_start = std::chrono::steady_clock::now();
}

// ---------------------------------------------------------------------------------------------- //

auto ElapsedTimer::getElapsed() const -> std::chrono::nanoseconds
{
    auto diff = std::chrono::steady_clock::now() - m_start;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(diff);
}

// ---------------------------------------------------------------------------------------------- //

auto ElapsedTimer::getNanoseconds() const -> long long
{
    return getElapsed().count();
}

// ---------------------------------------------------------------------------------------------- //

auto ElapsedTimer::getMicroseconds() const -> long long
{
    return getNanoseconds() / 1000;
}

// ---------------------------------------------------------------------------------------------- //

auto ElapsedTimer::getMilliseconds() const -> long long
{
    return getMicroseconds() / 1000;
}

// ---------------------------------------------------------------------------------------------- //

auto ElapsedTimer::getNanosecondsString() const -> std::string
{
    return std::to_string(getNanoseconds()) + " ns";
}

// ---------------------------------------------------------------------------------------------- //

auto ElapsedTimer::getMicrosecondsString() const -> std::string
{
    return std::to_string(getMicroseconds()) + " µs";
}

// ---------------------------------------------------------------------------------------------- //

auto ElapsedTimer::getMillisecondsString() const -> std::string
{
    return std::to_string(getMilliseconds()) + " ms";
}

// ---------------------------------------------------------------------------------------------- //
