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

#include "barrelfilter.h"
#include "filterworker.h"
#include "kernels.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class BarrelFilterWorker : public FilterWorker
{
public:
    BarrelFilterWorker(BarrelFilter* filter);

    void updateImpl() override;
    void runImpl(const Buffer& input, Buffer* output) override;

private:
    void updateMap();

private:
    BarrelGenKernel barrelGen;
    RemapKernel<Vec3f, RemapMode::Uniform> remap;

private:
    BarrelFilter* m_filter;

    ComputeBuffer2f m_map = { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };
    int m_factor = 0;
};

// ---------------------------------------------------------------------------------------------- //

BarrelFilter::BarrelFilter()
    : Filter("BarrelFilter")
{
}

// ---------------------------------------------------------------------------------------------- //

void BarrelFilter::setFactor(int factor)
{
    CLAMP_TO_RANGE(factor, MinimumFactor, MaximumFactor);

    m_factor = factor;
    update();
}

// ---------------------------------------------------------------------------------------------- //

auto BarrelFilter::getFactor() const -> int
{
    return m_factor;
}

// ---------------------------------------------------------------------------------------------- //

auto BarrelFilter::createWorker() -> std::unique_ptr<FilterWorker>
{
    return std::make_unique<BarrelFilterWorker>(this);
}

// ---------------------------------------------------------------------------------------------- //

BarrelFilterWorker::BarrelFilterWorker(BarrelFilter* filter)
    : FilterWorker(filter),
      m_filter(filter)
{
    m_factor = m_filter->getFactor();
    updateMap();
}

// ---------------------------------------------------------------------------------------------- //

void BarrelFilterWorker::updateImpl()
{
    const int factor = m_filter->getFactor();

    if (factor != m_factor)
    {
        m_factor = factor;
        updateMap();
    }
}

// ---------------------------------------------------------------------------------------------- //

void BarrelFilterWorker::runImpl(const Buffer& input, Buffer* output)
{
    remap(input, output, m_map);
}

// ---------------------------------------------------------------------------------------------- //

void BarrelFilterWorker::updateMap()
{
    float factor = 0.0F;

    if (m_factor >= 0)
        factor = m_factor * 0.005F;
    else
        factor = m_factor * 0.002F;

    barrelGen(&m_map, factor);
}

// ---------------------------------------------------------------------------------------------- //
