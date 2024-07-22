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

#include "ltc2945.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim15;

namespace Config
{
    constexpr TIM_HandleTypeDef* DelayTimerHandle = &htim1;
    constexpr TIM_HandleTypeDef* DimmingTimerHandle = &htim2;
    constexpr TIM_HandleTypeDef* ActivityTimerHandle = &htim6;
    constexpr TIM_HandleTypeDef* MonitorTimerHandle = &htim15;

    constexpr I2C_HandleTypeDef* PowerMonitorHandle = &hi2c1;
    constexpr Ltc2945::Address PowerMonitorAddress = Ltc2945::Address::A8;
    constexpr float PowerMonitorResistance = 0.2f;
}
