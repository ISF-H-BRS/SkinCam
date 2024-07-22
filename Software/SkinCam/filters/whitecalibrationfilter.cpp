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

#include "whitecalibrationfilter.h"
#include "filterworker.h"
#include "kernels.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class WhiteCalibrationFilterWorker : public FilterWorker
{
public:
    WhiteCalibrationFilterWorker(WhiteCalibrationFilter* filter);

    void updateImpl() override;
    void runImpl(const Buffer& input, Buffer* output) override;

private:
    void updateScale();

private:
    ScaleKernel<Vec3f> scale;

private:
    WhiteCalibrationFilter* m_filter;
    Vec3f m_scale = { 1.0F, 1.0F, 1.0F };
};

// ---------------------------------------------------------------------------------------------- //

WhiteCalibrationFilter::WhiteCalibrationFilter()
    : Filter("WhiteCalibrationFilter")
{
}

// ---------------------------------------------------------------------------------------------- //

void WhiteCalibrationFilter::setFactors(double factor0, double factor1, double factor2)
{
    CLAMP_TO_RANGE(factor0, MinimumFactor, MaximumFactor);
    CLAMP_TO_RANGE(factor1, MinimumFactor, MaximumFactor);
    CLAMP_TO_RANGE(factor2, MinimumFactor, MaximumFactor);

    m_factors = { factor0, factor1, factor2 };
    update();
}

// ---------------------------------------------------------------------------------------------- //

auto WhiteCalibrationFilter::getFactor(Channel channel) const -> double
{
    return m_factors.at(Global::indexOf(channel));
}

// ---------------------------------------------------------------------------------------------- //

auto WhiteCalibrationFilter::createWorker() -> std::unique_ptr<FilterWorker>
{
    return std::make_unique<WhiteCalibrationFilterWorker>(this);
}

// ---------------------------------------------------------------------------------------------- //

WhiteCalibrationFilterWorker::WhiteCalibrationFilterWorker(WhiteCalibrationFilter* filter)
    : FilterWorker(filter),
      m_filter(filter)
{
    updateScale();
}

// ---------------------------------------------------------------------------------------------- //

void WhiteCalibrationFilterWorker::updateImpl()
{
    updateScale();
}

// ---------------------------------------------------------------------------------------------- //

void WhiteCalibrationFilterWorker::runImpl(const Buffer& input, Buffer* output)
{
    scale(input, output, m_scale);
}

// ---------------------------------------------------------------------------------------------- //

void WhiteCalibrationFilterWorker::updateScale()
{
    auto factor0 = 1.0F / static_cast<float>(m_filter->getFactor(Channel::Wavelength0));
    auto factor1 = 1.0F / static_cast<float>(m_filter->getFactor(Channel::Wavelength1));
    auto factor2 = 1.0F / static_cast<float>(m_filter->getFactor(Channel::Wavelength2));

    m_scale = { factor0, factor1, factor2 };
}

// ---------------------------------------------------------------------------------------------- //
