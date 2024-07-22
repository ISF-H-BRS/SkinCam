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

#include "config.h"

#include <chrono>
using namespace std::chrono_literals;

class Delay
{
public:
    static void init()
    {
        HAL_TIM_Base_Start(Config::DelayTimerHandle);
    }

    template <typename Predicate>
    static auto wait(std::chrono::microseconds us, Predicate pred) -> bool
    {
        uint32_t count = 0;
        uint16_t last = now();

        while (count < us.count())
        {
            if (pred())
                return true;

            uint16_t current = now();
            uint16_t diff = current - last;

            count += diff;
            last = current;
        }

        return false;
    }

    static void wait(std::chrono::microseconds us)
    {
        wait(us, []{ return false; });
    }

private:
    static inline auto now() -> uint32_t
    {
        return __HAL_TIM_GET_COUNTER(Config::DelayTimerHandle);
    }
};
