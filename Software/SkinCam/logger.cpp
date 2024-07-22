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

#include "logger.h"

#include <algorithm>
#include <iostream>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

std::vector<Logger::Listener*> Logger::s_listeners;
std::string Logger::s_log;
std::mutex Logger::s_mutex;

// ---------------------------------------------------------------------------------------------- //

void Logger::addListener(Listener* listener)
{
    s_listeners.push_back(listener);
}

// ---------------------------------------------------------------------------------------------- //

void Logger::removeListener(Listener* listener)
{
    auto it = std::find(s_listeners.begin(), s_listeners.end(), listener);

    if (it != s_listeners.end())
        s_listeners.erase(it);
}

// ---------------------------------------------------------------------------------------------- //

void Logger::info(const char* msg)
{
    append(msg, std::cout);
}

// ---------------------------------------------------------------------------------------------- //

void Logger::info(const std::string& msg)
{
    info(msg.c_str());
}

// ---------------------------------------------------------------------------------------------- //

void Logger::error(const char* msg)
{
    append(msg, std::cerr);
}

// ---------------------------------------------------------------------------------------------- //

void Logger::error(const std::string& msg)
{
    info(msg.c_str());
}

// ---------------------------------------------------------------------------------------------- //

auto Logger::log() -> const std::string&
{
    return s_log;
}

// ---------------------------------------------------------------------------------------------- //

inline
void Logger::append(const char* msg, std::ostream& os)
{
    std::lock_guard lock(s_mutex);

    os << msg << std::endl;

    s_log += msg;
    s_log += "\n";

    for (auto listener : s_listeners)
        listener->onLogAppended(msg);
}

// ---------------------------------------------------------------------------------------------- //
