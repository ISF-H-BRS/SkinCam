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

#include <utility>

SKINCAM_BEGIN_NAMESPACE();

template <typename T>
auto swapBytes(T value) -> T
{
    constexpr int Size = sizeof(T);

    auto bytes = reinterpret_cast<char*>(&value);

    for (int i = 0; i < Size / 2; ++i)
        std::swap(bytes[i], bytes[Size - 1 - i]);

    return value;
}

template <typename T>
auto cpu2le(T value) -> T
{
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    return swapBytes(value);
#else
    return value;
#endif
}

template <typename T>
auto le2cpu(T value) -> T
{
    return cpu2le(value);
}

template <typename T>
auto cpu2be(T value) -> T
{
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    return swapBytes(value);
#else
    return value;
#endif
}

template <typename T>
auto be2cpu(T value) -> T
{
    return cpu2be(value);
}

SKINCAM_END_NAMESPACE();
