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

#include "hostinterface.h"
#include "ledcontrol.h"
#include "nircontrol.h"
#include "trigger.h"

class Ringlight : public HostInterface::Owner
{
public:
    Ringlight();

    void exec();

private:
    void onCommandReceived(const char* command) override;

    void protocolRunSequence();
    void protocolAnswerPing();
    void protocolSetSequenceDelay(const char* string);

    void sendAck();
    void sendError();

    void captureDark();
    void captureChannel(size_t channel);

    void delayTicks(size_t ticks);

private:
    HostInterface m_interface;

    LedControl m_led;
    NirControl m_nir;
    Trigger m_trigger;

    size_t m_sequenceDelay = 0;
};
