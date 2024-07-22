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

#include "ledcontrol.h"

#include <avr/io.h>

// ---------------------------------------------------------------------------------------------- //

#define LED_DDR     DDRB
#define LED_PORT    PORTB
#define LED_STATUS  (1 << PB1)
#define LED_ERROR   (1 << PB2)
#define LED_MASK    (LED_STATUS | LED_ERROR)

// ---------------------------------------------------------------------------------------------- //

LedControl::LedControl()
{
    LED_DDR |= LED_MASK;
    LED_PORT |= LED_MASK;
}

// ---------------------------------------------------------------------------------------------- //

void LedControl::setEnabled(Channel channel, bool enable)
{
    if (channel == Channel::Status)
    {
        if (enable)
            LED_PORT &= ~LED_STATUS;
        else
            LED_PORT |= LED_STATUS;
    }
    else if (channel == Channel::Error)
    {
        if (enable)
            LED_PORT &= ~LED_ERROR;
        else
            LED_PORT |= LED_ERROR;
    }
}

// ---------------------------------------------------------------------------------------------- //
