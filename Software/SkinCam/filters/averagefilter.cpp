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

#include "averagefilter.h"
#include "kernels.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class AverageFilterWorker : public FilterWorker
{
public:
    AverageFilterWorker(AverageFilter* filter);

    void updateImpl() override;
    void runImpl(const Buffer& input, Buffer* output) override;

private:
    void updateSamples(size_t sampleCount);

    void runAverage2(Buffer* output);
    void runAverage3(Buffer* output);
    void runAverage4(Buffer* output);

private:
    AverageKernel<Vec3f, 2> average2;
    AverageKernel<Vec3f, 3> average3;
    AverageKernel<Vec3f, 4> average4;

private:
    AverageFilter* m_filter;

    std::vector<ComputeBuffer3f> m_samples;
    size_t m_currentSample = 0;

    bool m_refillSamples = false;

    std::function<void(Buffer*)> m_kernel = nullptr;
};

// ---------------------------------------------------------------------------------------------- //

AverageFilter::AverageFilter()
    : Filter("AverageFilter")
{
}

// ---------------------------------------------------------------------------------------------- //

void AverageFilter::setSampleCount(int count)
{
    CLAMP_TO_RANGE(count, MinimumSampleCount, MaximumSampleCount);

    m_sampleCount = count;
    update();
}

// ---------------------------------------------------------------------------------------------- //

auto AverageFilter::getSampleCount() const -> int
{
    return m_sampleCount;
}

// ---------------------------------------------------------------------------------------------- //

auto AverageFilter::createWorker() -> std::unique_ptr<FilterWorker>
{
    return std::make_unique<AverageFilterWorker>(this);
}

// ---------------------------------------------------------------------------------------------- //

AverageFilterWorker::AverageFilterWorker(AverageFilter* filter)
    : FilterWorker(filter),
      m_filter(filter)
{
    updateSamples(static_cast<size_t>(m_filter->getSampleCount()));
}

// ---------------------------------------------------------------------------------------------- //

void AverageFilterWorker::updateImpl()
{
    const auto sampleCount = static_cast<size_t>(m_filter->getSampleCount());

    if (sampleCount != m_samples.size())
        updateSamples(sampleCount);
}

// ---------------------------------------------------------------------------------------------- //

void AverageFilterWorker::runImpl(const Buffer& input, Buffer* output)
{
    ASSERT_NOT_NULL(m_kernel);

    if (m_refillSamples) [[unlikely]]
    {
        for (auto& sample : m_samples)
            input.copyTo(&sample);

        m_refillSamples = false;
    }
    else
        input.copyTo(&m_samples.at(m_currentSample));

    if (++m_currentSample >= m_samples.size())
        m_currentSample = 0;

    m_kernel(output);
}

// ---------------------------------------------------------------------------------------------- //

void AverageFilterWorker::updateSamples(size_t sampleCount)
{
    ASSERT_IN_RANGE(sampleCount, 2, 4);

    while (m_samples.size() > sampleCount)
        m_samples.pop_back();

    while (m_samples.size() < sampleCount)
        m_samples.emplace_back(ImageWidth, ImageHeight, CL_MEM_READ_ONLY);

    if (sampleCount == 2)
        m_kernel = [this](Buffer* output){ runAverage2(output); };
    else if (sampleCount == 3)
        m_kernel = [this](Buffer* output){ runAverage3(output); };
    else if (sampleCount == 4)
        m_kernel = [this](Buffer* output){ runAverage4(output); };

    m_currentSample = 0;
    m_refillSamples = true;
}

// ---------------------------------------------------------------------------------------------- //

void AverageFilterWorker::runAverage2(Buffer* output)
{
    ASSERT(m_samples.size() == 2);
    average2(m_samples.begin(), output);
}

// ---------------------------------------------------------------------------------------------- //

void AverageFilterWorker::runAverage3(Buffer* output)
{
    ASSERT(m_samples.size() == 3);
    average3(m_samples.begin(), output);
}

// ---------------------------------------------------------------------------------------------- //

void AverageFilterWorker::runAverage4(Buffer* output)
{
    ASSERT(m_samples.size() == 4);
    average4(m_samples.begin(), output);
}

// ---------------------------------------------------------------------------------------------- //
