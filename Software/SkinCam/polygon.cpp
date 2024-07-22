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
#include "polygon.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

static
auto intersect(int x, int y, const Vec2i& v1, const Vec2i& v2) -> bool
{
    int y1 = v1.y();
    int y2 = v2.y();

    if (y1 == y2) // Skip horizontal edges
        return false;

    const bool swap = y2 < y1;

    if (swap)
        std::swap(y1, y2);

    if (y < y1 || y >= y2) // Include only one vertex so only one intersection is counted
        return false;

    int x1 = v1.x();
    int x2 = v2.x();

    if (swap)
        std::swap(x1, x2);

    int intersection = x1 + static_cast<int>(static_cast<float>(x2 - x1)
                                    / static_cast<float>(y2 - y1) * static_cast<float>(y - y1));
    return intersection >= x;
}

// ---------------------------------------------------------------------------------------------- //

Polygon::Polygon(const std::vector<Vertex>& vertices)
    : m_vertices(vertices)
{
    ASSERT(vertices.size() >= 3);
}

// ---------------------------------------------------------------------------------------------- //

auto Polygon::contains(int x, int y) const -> bool
{
    const size_t vertexCount = m_vertices.size();

    int intersectionCount = 0;

    for (size_t i = 1; i <= vertexCount; ++i)
    {
        const Vec2i& v1 = m_vertices[i - 1];
        const Vec2i& v2 = m_vertices[i == vertexCount ? 0 : i];

        if (intersect(x, y, v1, v2))
            ++intersectionCount;
    }

    return (intersectionCount % 2 != 0);
}

// ---------------------------------------------------------------------------------------------- //
