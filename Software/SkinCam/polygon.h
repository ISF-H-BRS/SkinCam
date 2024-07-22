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

#include "vectors.h"

#include <vector>

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

class Polygon
{
public:
    using Vertex = Vec2i;
    using Point = Vec2i;

public:
    Polygon(const std::vector<Vertex>& vertices);

    auto vertices() const -> const std::vector<Vertex>&;

    auto contains(int x, int y) const -> bool;
    auto contains(const Point& point) const -> bool;

private:
    std::vector<Vertex> m_vertices;
};

// ---------------------------------------------------------------------------------------------- //

inline
auto Polygon::vertices() const -> const std::vector<Vertex>&
{
    return m_vertices;
}

// ---------------------------------------------------------------------------------------------- //

inline
auto Polygon::contains(const Point& point) const -> bool
{
    return contains(point.x(), point.y());
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
