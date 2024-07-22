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

#include "namespace.h"

#include <mutex>
#include <string>
#include <vector>

SKINCAM_BEGIN_NAMESPACE();

class Logger
{
public:
    class Listener
    {
        friend class Logger;
        virtual void onLogAppended(const char* msg) = 0;
    };

public:
    static void addListener(Listener* listener);
    static void removeListener(Listener* listener);

    static void info(const char* msg);
    static void info(const std::string& msg);

    static void error(const char* msg);
    static void error(const std::string& msg);

    static auto log() -> const std::string&;

private:
    static void append(const char* msg, std::ostream& os);

private:
    static std::vector<Listener*> s_listeners;

    static std::string s_log;
    static std::mutex s_mutex;
};

SKINCAM_END_NAMESPACE();
