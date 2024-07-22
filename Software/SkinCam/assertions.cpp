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

#include "assertions.h"

// ---------------------------------------------------------------------------------------------- //

namespace SkinCam::Assertions {

// ---------------------------------------------------------------------------------------------- //

void fail(const char* condition, const char* function, const char* file, int line)
{
    std::cerr << file << ":" << line << ", function " << function << ": "
              << "Assertion " << condition << " failed." << std::endl;

    std::abort();
}

// ---------------------------------------------------------------------------------------------- //

void notifySatisfied(const char* condition, const char* function)
{
    std::cerr << "In function " << function << ": "
              << "Condition " << condition << " should not be satisfied. "
              << "Aborting function call." << std::endl;
}

// ---------------------------------------------------------------------------------------------- //

void notifyUnsatisfied(const char* condition, const char* function)
{
    std::cerr << "In function " << function << ": "
              << "Condition " << condition << " is not satisfied. "
              << "Aborting function call." << std::endl;
}

// ---------------------------------------------------------------------------------------------- //

void notifyIgnoreNull(const char* function, const char* ptrarg)
{
    std::cerr << "In function " << function << ": "
              << "Argument " << ptrarg << " is a null-pointer. "
              << "Aborting function call." << std::endl;
}

// ---------------------------------------------------------------------------------------------- //

void notifyIgnoreNotNull(const char* function, const char* ptrarg)
{
    std::cerr << "In function " << function << ": "
              << "Argument " << ptrarg << " is not a null-pointer. "
              << "Aborting function call." << std::endl;
}

// ---------------------------------------------------------------------------------------------- //

} // End of namespace SkinCam::Assertions

// ---------------------------------------------------------------------------------------------- //
