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

#include <cmath> // for round()

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

class Rect
{
public:
    constexpr Rect() {}
    constexpr Rect(int x, int y, int width, int height)
        : m_x(x), m_y(y), m_width(width), m_height(height) {}

    constexpr auto x() const -> int { return m_x; }
    constexpr auto y() const -> int { return m_y; }
    constexpr auto width() const -> int { return m_width; }
    constexpr auto height() const -> int { return m_height; }

private:
    int m_x = 0;
    int m_y = 0;
    int m_width = 0;
    int m_height = 0;
};

// ---------------------------------------------------------------------------------------------- //

inline
auto operator+(const Rect& lhs, const Rect& rhs) -> Rect
{
    return {
        lhs.x()      + rhs.x(),
        lhs.y()      + rhs.y(),
        lhs.width()  + rhs.width(),
        lhs.height() + rhs.height()
    };
}

// ---------------------------------------------------------------------------------------------- //

inline
auto operator*(int s, const Rect& r) -> Rect
{
    return {
        s * r.x(),
        s * r.y(),
        s * r.width(),
        s * r.height()
    };
}

// ---------------------------------------------------------------------------------------------- //

inline
auto operator*(double s, const Rect& r) -> Rect
{
    const double x = std::round(s * r.x());
    const double y = std::round(s * r.y());
    const double w = std::round(s * r.width());
    const double h = std::round(s * r.height());

    return {
        static_cast<int>(x),
        static_cast<int>(y),
        static_cast<int>(w),
        static_cast<int>(h)
    };
}

// ---------------------------------------------------------------------------------------------- //

inline
auto operator/(const Rect& r, int s) -> Rect
{
    return {
        r.x()      / s,
        r.y()      / s,
        r.width()  / s,
        r.height() / s
    };
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
