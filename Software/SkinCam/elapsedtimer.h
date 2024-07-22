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

#include <chrono>
#include <string>

SKINCAM_BEGIN_NAMESPACE();

class ElapsedTimer
{
public:
    ElapsedTimer();

    void restart();

    auto getElapsed() const -> std::chrono::nanoseconds;

    auto getNanoseconds() const -> long long;
    auto getMicroseconds() const -> long long;
    auto getMilliseconds() const -> long long;

    auto getNanosecondsString() const -> std::string;
    auto getMicrosecondsString() const -> std::string;
    auto getMillisecondsString() const -> std::string;

private:
    std::chrono::time_point<std::chrono::steady_clock> m_start;
};

SKINCAM_END_NAMESPACE();
