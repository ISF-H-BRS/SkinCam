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

#include "ringlight.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr size_t ChannelIndex1050 = 0;
    constexpr size_t ChannelIndex1300 = 2;
    constexpr size_t ChannelIndex1550 = 3;

    constexpr size_t TriggerDuration = 30;
    constexpr size_t FrameDuration = 100;
    constexpr size_t SequenceDuration = 400;

    constexpr size_t MinimumSequenceDelay =   0;
    constexpr size_t MaximumSequenceDelay = 600;

    constexpr double TickDurationMs = 0.1;
}

// ---------------------------------------------------------------------------------------------- //

Ringlight::Ringlight()
    : m_interface(this)
{
}

// ---------------------------------------------------------------------------------------------- //

void Ringlight::exec()
{
    sei(); // Enable interrupts

    while (true)
        m_interface.update();
}

// ---------------------------------------------------------------------------------------------- //

void Ringlight::onCommandReceived(const char* command)
{
    m_led.setEnabled(LedControl::Channel::Status, true);

    if (strcmp(command, "S") == 0)
        protocolRunSequence();
    else if (strcmp(command, "P") == 0)
        protocolAnswerPing();
    else if (command[0] == 'D')
        protocolSetSequenceDelay(&command[1]);
    else
        sendError();

    m_led.setEnabled(LedControl::Channel::Status, false);
}

// ---------------------------------------------------------------------------------------------- //

void Ringlight::protocolRunSequence()
{
    sendAck();

    captureDark();
    captureChannel(ChannelIndex1050);
    captureChannel(ChannelIndex1300);
    captureChannel(ChannelIndex1550);

    delayTicks(m_sequenceDelay);
}

// ---------------------------------------------------------------------------------------------- //

void Ringlight::protocolAnswerPing()
{
    sendAck();
}

// ---------------------------------------------------------------------------------------------- //

void Ringlight::protocolSetSequenceDelay(const char* string)
{
    long int delay = atol(string);

    if (delay < MinimumSequenceDelay || delay > MaximumSequenceDelay)
        sendError();
    else
    {
        m_sequenceDelay = static_cast<size_t>(delay);
        sendAck();
    }
}

// ---------------------------------------------------------------------------------------------- //

inline
void Ringlight::sendAck()
{
    m_interface.sendMessage("A");
}

// ---------------------------------------------------------------------------------------------- //

inline
void Ringlight::sendError()
{
    m_interface.sendMessage("E");
}

// ---------------------------------------------------------------------------------------------- //

void Ringlight::captureDark()
{
    m_trigger.setActive(true);

    delayTicks(TriggerDuration);

    m_trigger.setActive(false);

    delayTicks(FrameDuration - TriggerDuration);
}

// ---------------------------------------------------------------------------------------------- //

void Ringlight::captureChannel(size_t channel)
{
    m_nir.enableChannel(channel);
    m_trigger.setActive(true);

    delayTicks(TriggerDuration);

    m_trigger.setActive(false);

    delayTicks(FrameDuration - TriggerDuration);

    m_nir.disableChannel(channel);
}

// ---------------------------------------------------------------------------------------------- //

inline
void Ringlight::delayTicks(size_t ticks)
{
    for (size_t i = 0; i < ticks; ++i)
        _delay_ms(TickDurationMs);
}

// ---------------------------------------------------------------------------------------------- //
