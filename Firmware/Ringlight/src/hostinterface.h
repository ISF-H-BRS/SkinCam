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

#include <stddef.h>

class HostInterface
{
public:
    class Owner
    {
        friend class HostInterface;
        virtual void onCommandReceived(const char* command) = 0;
    };

public:
    HostInterface(Owner* owner);

    void update();

    void sendMessage(const char* message);

private:
    void sendCharacter(char c);
    void onCharacterReceived(char c);

private:
    Owner* m_owner;

    static constexpr size_t BufferSize = 16;

    char m_buffer[BufferSize];
    size_t m_bufferPosition = 0;

    char m_command[BufferSize];
    volatile bool m_commandReceived = false;

    friend void usartInterruptHandler();
    static HostInterface* s_instance;
};
