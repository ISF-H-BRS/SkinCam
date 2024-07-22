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

#include "hostinterface.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include <string.h>

// ---------------------------------------------------------------------------------------------- //

#define USART_DDR  DDRD
#define USART_RX   (1 << PD0)
#define USART_TX   (1 << PD1)

// ---------------------------------------------------------------------------------------------- //

inline
void usartInterruptHandler()
{
    if (HostInterface::s_instance)
        HostInterface::s_instance->onCharacterReceived(UDR0);
}

// ---------------------------------------------------------------------------------------------- //

SIGNAL(USART_RX_vect)
{
    usartInterruptHandler();
}

// ---------------------------------------------------------------------------------------------- //

HostInterface* HostInterface::s_instance = nullptr;

// ---------------------------------------------------------------------------------------------- //

HostInterface::HostInterface(Owner* owner)
    : m_owner(owner)
{
    s_instance = this;

    USART_DDR |= USART_TX;  // Output
    USART_DDR &= ~USART_RX; // Input

    const uint32_t baudrate = 1'000'000;
    const auto ubrr = static_cast<uint16_t>((F_CPU / 16 / baudrate) - 1);

    UBRR0H = static_cast<uint8_t>(ubrr >> 8);
    UBRR0L = static_cast<uint8_t>(ubrr);

    // Enable RX & TX, RX interrupt, 8-bit data, 1 stop bit, no parity
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (3 << UCSZ00);
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::update()
{
    if (m_commandReceived)
    {
        strcpy(m_command, m_buffer);

        m_bufferPosition = 0;
        m_commandReceived = false;

        m_owner->onCommandReceived(m_command);
    }
}

// ---------------------------------------------------------------------------------------------- //

void HostInterface::sendMessage(const char* message)
{
    while (*message != '\0')
        sendCharacter(*message++);

    sendCharacter('\n');
}

// ---------------------------------------------------------------------------------------------- //

inline
void HostInterface::sendCharacter(char c)
{
    while (!(UCSR0A & (1 << UDRE0)))
        continue;

    UDR0 = c;
}

// ---------------------------------------------------------------------------------------------- //

inline
void HostInterface::onCharacterReceived(char c)
{
    if (m_commandReceived)
        return;

    if (c == '\n')
    {
        m_buffer[m_bufferPosition] = '\0';
        m_commandReceived = true;
    }
    else
    {
        m_buffer[m_bufferPosition] = c;

        if (++m_bufferPosition >= BufferSize)
            m_bufferPosition = 0;
    }
}

// ---------------------------------------------------------------------------------------------- //
