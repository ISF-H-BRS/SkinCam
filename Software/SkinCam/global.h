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
#include "vectors.h"

#include <cmath>

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

#ifdef SKINCAM_BUILD_LEGACY
constexpr int ImageWidth  = 636;
constexpr int ImageHeight = 508;
#else
constexpr int ImageWidth  = 640;
constexpr int ImageHeight = 512;
#endif

constexpr int ImageArea = ImageWidth * ImageHeight;

// ---------------------------------------------------------------------------------------------- //

enum class Channel
{
    Wavelength0,
    Wavelength1,
    Wavelength2
};

static constexpr size_t ChannelCount = 3;

// ---------------------------------------------------------------------------------------------- //

namespace Global {

    template <typename T = size_t>
    constexpr auto indexOf(Channel channel) { return static_cast<T>(channel); }

    template <typename T>
    constexpr auto toChannel(T index) { return static_cast<Channel>(index); }

    inline
    constexpr auto getString(Channel channel) -> const char*
    {
        if (channel == Channel::Wavelength0)
            return "1050 nm";

        if (channel == Channel::Wavelength1)
            return "1300 nm";

        return "1550 nm";
    }

    template <typename T>
    inline auto getNormalizedDifferences(const Vec<T,3>& v) -> Vec<T,3>
    {
        static constexpr T minDiff = T(0.0001);
        static constexpr T zero = T(0);

        const T x = v.x();
        const T y = v.y();
        const T z = v.z();

        T dx = x - y;

        if (std::fabs(dx) > minDiff)
            dx /= x + y;
        else
            dx = zero;

        T dy = x - z;

        if (std::fabs(dy) > minDiff)
            dy /= x + z;
        else
            dy = zero;

        T dz = y - z;

        if (std::fabs(dz) > minDiff)
            dz /= y + z;
        else
            dz = zero;

        return { dx, dy, dz };
    }
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
