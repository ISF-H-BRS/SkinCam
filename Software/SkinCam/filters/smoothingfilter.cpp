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

#include "filterworker.h"
#include "kernels.h"
#include "smoothingfilter.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class SmoothingFilterWorker : public FilterWorker
{
public:
    SmoothingFilterWorker(SmoothingFilter* filter);

    void updateImpl() override;
    void runImpl(const Buffer& input, Buffer* output) override;

private:
    void updateWeights();

private:
    BlurKernel<Vec3f> blur;

private:
    SmoothingFilter* m_filter;
    ComputeBuffer1f m_weights;
    uint m_radius;
};

// ---------------------------------------------------------------------------------------------- //

SmoothingFilter::SmoothingFilter()
    : Filter("SmoothingFilter")
{
}

// ---------------------------------------------------------------------------------------------- //

void SmoothingFilter::setRadius(uint radius)
{
    CLAMP_TO_RANGE(radius, MinimumRadius, MaximumRadius);

    m_radius = radius;
    update();
}

// ---------------------------------------------------------------------------------------------- //

auto SmoothingFilter::getRadius() const -> uint
{
    return m_radius;
}

// ---------------------------------------------------------------------------------------------- //

auto SmoothingFilter::createWorker() -> std::unique_ptr<FilterWorker>
{
    return std::make_unique<SmoothingFilterWorker>(this);
}

// ---------------------------------------------------------------------------------------------- //

SmoothingFilterWorker::SmoothingFilterWorker(SmoothingFilter* filter)
    : FilterWorker(filter),
      m_filter(filter),
      m_weights(1, 1, CL_MEM_READ_ONLY),
      m_radius(m_filter->getRadius())
{
    updateWeights();
}

// ---------------------------------------------------------------------------------------------- //

void SmoothingFilterWorker::updateImpl()
{
    const uint radius = m_filter->getRadius();

    if (radius != m_radius)
    {
        m_radius = radius;
        updateWeights();
    }
}

// ---------------------------------------------------------------------------------------------- //

void SmoothingFilterWorker::runImpl(const Buffer& input, Buffer* output)
{
    blur(input, output, m_weights);
}

// ---------------------------------------------------------------------------------------------- //

void SmoothingFilterWorker::updateWeights()
{
    std::vector<float> weights = gaussian<float>(m_radius);

    std::vector<Vec1f> data(weights.size());
    std::copy(weights.begin(), weights.end(), data.begin());

    m_weights = ComputeBuffer1f(data, static_cast<uint>(data.size()), 1, CL_MEM_READ_ONLY);
}

// ---------------------------------------------------------------------------------------------- //
