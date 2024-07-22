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
#include "global.h"
#include "kernels.h"
#include "sharpeningfilter.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class SharpeningFilterWorker : public FilterWorker
{
public:
    SharpeningFilterWorker(SharpeningFilter* filter);

    void updateImpl() override;
    void runImpl(const Buffer& input, Buffer* output) override;

private:
    void updateWeights();

private:
    BlurKernel<Vec3f> blur;
    SharpenKernel<Vec3f> sharpen;

private:
    SharpeningFilter* m_filter;

    ComputeBuffer1f m_weights;
    ComputeBuffer3f m_blurred;

    uint m_radius;
    float m_amount;
    float m_threshold;
};

// ---------------------------------------------------------------------------------------------- //

SharpeningFilter::SharpeningFilter()
    : Filter("SharpeningFilter")
{
}

// ---------------------------------------------------------------------------------------------- //

void SharpeningFilter::setParameters(uint radius, float amount, float threshold)
{
    CLAMP_TO_RANGE(radius, MinimumRadius, MaximumRadius);
    CLAMP_TO_RANGE(amount, MinimumAmount, MaximumAmount);
    CLAMP_TO_RANGE(threshold, MinimumThreshold, MaximumThreshold);

    m_radius = radius;
    m_amount = amount;
    m_threshold = threshold;

    update();
}

// ---------------------------------------------------------------------------------------------- //

auto SharpeningFilter::getRadius() const -> uint
{
    return m_radius;
}

// ---------------------------------------------------------------------------------------------- //

auto SharpeningFilter::getAmount() const -> float
{
    return m_amount;
}

// ---------------------------------------------------------------------------------------------- //

auto SharpeningFilter::getThreshold() const -> float
{
    return m_threshold;
}

// ---------------------------------------------------------------------------------------------- //

auto SharpeningFilter::createWorker() -> std::unique_ptr<FilterWorker>
{
    return std::make_unique<SharpeningFilterWorker>(this);
}

// ---------------------------------------------------------------------------------------------- //

SharpeningFilterWorker::SharpeningFilterWorker(SharpeningFilter* filter)
    : FilterWorker(filter),
      m_filter(filter),
      m_weights(1, 1, CL_MEM_READ_ONLY),
      m_blurred(ImageWidth, ImageHeight, CL_MEM_READ_WRITE),
      m_radius(m_filter->getRadius()),
      m_amount(m_filter->getAmount()),
      m_threshold(m_filter->getThreshold())
{
    updateWeights();
}

// ---------------------------------------------------------------------------------------------- //

void SharpeningFilterWorker::updateImpl()
{
    m_amount = m_filter->getAmount();
    m_threshold = m_filter->getThreshold();

    const uint radius = m_filter->getRadius();

    if (radius != m_radius)
    {
        m_radius = radius;
        updateWeights();
    }
}

// ---------------------------------------------------------------------------------------------- //

void SharpeningFilterWorker::runImpl(const Buffer& input, Buffer* output)
{
    blur(input, &m_blurred, m_weights);
    sharpen(input, output, m_blurred, m_threshold, m_amount);
}

// ---------------------------------------------------------------------------------------------- //

void SharpeningFilterWorker::updateWeights()
{
    std::vector<float> weights = gaussian<float>(m_radius);

    std::vector<Vec1f> data(weights.size());
    std::copy(weights.begin(), weights.end(), data.begin());

    m_weights = ComputeBuffer1f(data, static_cast<uint>(data.size()), 1, CL_MEM_READ_ONLY);
}

// ---------------------------------------------------------------------------------------------- //
