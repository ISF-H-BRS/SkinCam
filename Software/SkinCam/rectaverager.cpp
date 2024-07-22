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
#include "rectaverager.h"

#include <numeric>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

static inline
auto getDifference(const Rect& rect1, const Rect& rect2) -> int
{
    const int l1 = rect1.x();
    const int l2 = rect2.x();

    const int r1 = l1 + rect1.width();
    const int r2 = l2 + rect2.width();

    const int b1 = rect1.y();
    const int b2 = rect2.y();

    const int t1 = b1 + rect1.height();
    const int t2 = b2 + rect2.height();

    int diff = std::abs(l1 - l2);
    diff = std::max(diff, std::abs(r1 - r2));
    diff = std::max(diff, std::abs(b1 - b2));
    diff = std::max(diff, std::abs(t1 - t2));

    return diff;
}

// ---------------------------------------------------------------------------------------------- //

RectAverager::RectAverager(size_t bufferSize)
{
    setBufferSize(bufferSize);
}

// ---------------------------------------------------------------------------------------------- //

void RectAverager::setBufferSize(size_t size)
{
    ASSERT(size >= MinimumBufferSize && size % 2 == 1);

    m_bufferSize = size;
    m_coeffs.resize(m_bufferSize);

    // Savitzky-Golay smoothing
    const auto m = static_cast<double>(m_bufferSize);
    const auto shift = static_cast<double>(m_bufferSize - 1) / 2;

    for (size_t index = 0; index < size; ++index)
    {
        const auto i = static_cast<double>(index) - shift;
        m_coeffs[index] = ((3.0*m*m - 7.0 - 20.0*i*i) / 4.0) / (m*(m*m - 4.0) / 3.0);
    }
}

// ---------------------------------------------------------------------------------------------- //

void RectAverager::addRect(const Rect& rect)
{
    if (!m_rects.empty() && getDifference(rect, m_rects.back()) > MaximumDifference)
        m_rects.clear();

    while (m_rects.size() <= m_bufferSize)
        m_rects.push_back(rect);

    while (m_rects.size() > m_bufferSize)
        m_rects.pop_front();
}

// ---------------------------------------------------------------------------------------------- //

void RectAverager::addRect(int x, int y, int width, int height)
{
    addRect({ x, y, width, height });
}

// ---------------------------------------------------------------------------------------------- //

void RectAverager::reset()
{
    m_rects.clear();
}

// ---------------------------------------------------------------------------------------------- //

auto RectAverager::getAverage() const -> Rect
{
    ASSERT(m_rects.size() == m_bufferSize);
    ASSERT(m_rects.size() == m_coeffs.size());

    double x = 0.0;
    double y = 0.0;
    double w = 0.0;
    double h = 0.0;

    for (size_t i = 0; i < m_bufferSize; ++i)
    {
        x += m_coeffs[i] * m_rects[i].x();
        y += m_coeffs[i] * m_rects[i].y();
        w += m_coeffs[i] * m_rects[i].width();
        h += m_coeffs[i] * m_rects[i].height();
    }

    return {
        static_cast<int>(std::round(x)),
        static_cast<int>(std::round(y)),
        static_cast<int>(std::round(w)),
        static_cast<int>(std::round(h))
    };
}

// ---------------------------------------------------------------------------------------------- //
