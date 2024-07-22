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

#include <cmath>
#include <vector>

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto gaussian(T value, T mean, T variance) -> T
{
    constexpr T TwoPi = static_cast<T>(6.2831853071795864769);

    const T base = static_cast<T>(1.0) / std::sqrt(TwoPi * variance);
    const T diff = value - mean;

    return base * std::exp(diff*diff / (static_cast<T>(-2.0) * variance));
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto gaussian(uint radius) -> std::vector<T>
{
    std::vector<T> weights;

    for (uint i = 0; i <= radius; ++i)
    {
        T weight = gaussian(static_cast<T>(i), static_cast<T>(0.0), static_cast<T>(radius*radius));
        weights.push_back(weight);
    }

    return weights;
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
