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

#include "controller.h"
#include "delay.h"
#include "usermain.h"

#include <array>
#include <new>

// ---------------------------------------------------------------------------------------------- //

static
void terminate()
{
    HAL_GPIO_WritePin(STATUS_GOOD_GPIO_Port, STATUS_GOOD_Pin, GPIO_PIN_SET);

    while (true)
    {
        HAL_GPIO_WritePin(STATUS_BAD_GPIO_Port, STATUS_BAD_Pin, GPIO_PIN_RESET);
        HAL_Delay(100);

        HAL_GPIO_WritePin(STATUS_BAD_GPIO_Port, STATUS_BAD_Pin, GPIO_PIN_SET);
        HAL_Delay(100);
    }
}

// ---------------------------------------------------------------------------------------------- //

namespace __gnu_cxx
{
    // Strip unused name-demangling code from libstdc++
    void __verbose_terminate_handler() { terminate(); }
}

// ---------------------------------------------------------------------------------------------- //

namespace {
    std::array<char, sizeof(Controller)> g_buffer;
}

// ---------------------------------------------------------------------------------------------- //

void user_main()
{
    Delay::init();

    Controller* controller = nullptr;

    try {
        controller = new (g_buffer.data()) Controller;
        controller->exec();
    }
    catch (...)
    {
        if (controller)
            controller->~Controller();

        terminate();
    }
}

// ---------------------------------------------------------------------------------------------- //
