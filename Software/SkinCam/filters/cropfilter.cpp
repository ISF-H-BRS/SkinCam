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

#include "cropfilter.h"
#include "filterworker.h"
#include "kernels.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class CropFilterWorker : public FilterWorker
{
public:
    CropFilterWorker(CropFilter* filter);

    void updateImpl() override;
    void runImpl(const Buffer& input, Buffer* output) override;

private:
    void updateBorders();

private:
    CropGenKernel cropGen;
    RemapKernel<Vec3f, RemapMode::Uniform> remap;

private:
    CropFilter* m_filter;

    std::tuple<int,int,int,int> m_borders;

    ComputeBuffer2f m_map;
};

// ---------------------------------------------------------------------------------------------- //

CropFilter::CropFilter()
    : Filter("CropFilter")
{
}

// ---------------------------------------------------------------------------------------------- //

void CropFilter::setBorders(int left, int right, int top, int bottom)
{
    CLAMP_TO_RANGE(left,   MinimumHorizontalBorder, MaximumHorizontalBorder);
    CLAMP_TO_RANGE(right,  MinimumHorizontalBorder, MaximumHorizontalBorder);
    CLAMP_TO_RANGE(top,    MinimumVerticalBorder,   MaximumVerticalBorder);
    CLAMP_TO_RANGE(bottom, MinimumVerticalBorder,   MaximumVerticalBorder);

    m_borders = std::make_tuple(left, right, top, bottom);

    update();
}

// ---------------------------------------------------------------------------------------------- //

auto CropFilter::getBorders() const -> std::tuple<int,int,int,int>
{
    return m_borders;
}

// ---------------------------------------------------------------------------------------------- //

auto CropFilter::createWorker() -> std::unique_ptr<FilterWorker>
{
    return std::make_unique<CropFilterWorker>(this);
}

// ---------------------------------------------------------------------------------------------- //

CropFilterWorker::CropFilterWorker(CropFilter* filter)
    : FilterWorker(filter),
      m_filter(filter),
      m_borders(m_filter->getBorders()),
      m_map(ImageWidth, ImageHeight, CL_MEM_READ_WRITE)
{
    updateBorders();
}

// ---------------------------------------------------------------------------------------------- //

void CropFilterWorker::updateImpl()
{
    const auto borders = m_filter->getBorders();

    if (borders != m_borders)
    {
        m_borders = borders;
        updateBorders();
    }

}

// ---------------------------------------------------------------------------------------------- //

void CropFilterWorker::runImpl(const Buffer& input, Buffer* output)
{
    remap(input, output, m_map);
}

// ---------------------------------------------------------------------------------------------- //

void CropFilterWorker::updateBorders()
{
    auto [left, right, top, bottom] = m_borders;

    ASSERT(left + right <= ImageWidth && top + bottom <= ImageHeight);

    const auto originalWidth = static_cast<float>(ImageWidth);
    const auto originalHeight = static_cast<float>(ImageHeight);

    const auto croppedWidth = static_cast<float>(ImageWidth - left - right);
    const auto croppedHeight = static_cast<float>(ImageHeight - top - bottom);

    const float originalRatio = originalWidth / originalHeight;
    const float croppedRatio = croppedWidth / croppedHeight;

    const float scale = (croppedRatio > originalRatio) ? originalWidth / croppedWidth
                                                       : originalHeight / croppedHeight;
    ASSERT(scale >= 1.0F);

    const float finalWidth = scale * croppedWidth;
    const float finalHeight = scale * croppedHeight;

    const float hBorder = (originalWidth - finalWidth) / 2.0F;
    const float vBorder = (originalHeight - finalHeight) / 2.0F;

    cropGen(&m_map, scale, static_cast<float>(left), static_cast<float>(top), hBorder, vBorder);
}

// ---------------------------------------------------------------------------------------------- //
