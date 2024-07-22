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

#include "nircontrol.h"

#include <avr/io.h>

// ---------------------------------------------------------------------------------------------- //

#define NIR_DDR   DDRD
#define NIR_PORT  PORTD
#define NIR_PIN1  (1 << PD4)
#define NIR_PIN2  (1 << PD5)
#define NIR_PIN3  (1 << PD6)
#define NIR_PIN4  (1 << PD7)
#define NIR_PIN5  (1 << PD2)
#define NIR_PIN6  (1 << PD3)
#define NIR_MASK  (NIR_PIN1 | NIR_PIN2 | NIR_PIN3 | NIR_PIN4 | NIR_PIN5 | NIR_PIN6)

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr int Pins[NirControl::ChannelCount] = {
        NIR_PIN1, NIR_PIN2, NIR_PIN3, NIR_PIN4, NIR_PIN5, NIR_PIN6
    };
}

// ---------------------------------------------------------------------------------------------- //

NirControl::NirControl()
{
    // Set to output
    NIR_DDR |= NIR_MASK;

    disableAll();
}

// ---------------------------------------------------------------------------------------------- //

void NirControl::enableChannel(size_t index)
{
    if (index >= ChannelCount)
        return;

    NIR_PORT |= Pins[index];
}

// ---------------------------------------------------------------------------------------------- //

void NirControl::disableChannel(size_t index)
{
    if (index >= ChannelCount)
        return;

    NIR_PORT &= ~Pins[index];
}

// ---------------------------------------------------------------------------------------------- //

void NirControl::disableAll()
{
    NIR_PORT &= ~NIR_MASK;
}

// ---------------------------------------------------------------------------------------------- //
