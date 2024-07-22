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

#include "coloradjustfilter.h"
#include "filterworker.h"
#include "kernels.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class ColorAdjustFilterWorker : public FilterWorker
{
public:
    ColorAdjustFilterWorker(ColorAdjustFilter* filter);

    void updateImpl() override;
    void runImpl(const Buffer& input, Buffer* output) override;

private:
    void updateFactors();

private:
    ConvertKernel<Vec3f, Vec3f> convert;

private:
    ColorAdjustFilter* m_filter;

    int m_brightness;
    int m_contrast;

    float m_alpha = 1.0F;
    float m_beta = 0.0F;
};

// ---------------------------------------------------------------------------------------------- //

ColorAdjustFilter::ColorAdjustFilter()
    : Filter("ColorAdjustFilter")
{
}

// ---------------------------------------------------------------------------------------------- //

void ColorAdjustFilter::setParameters(int brightness, int contrast)
{
    CLAMP_TO_RANGE(brightness, MinimumBrightness, MaximumBrightness);
    CLAMP_TO_RANGE(contrast, MinimumContrast, MaximumContrast);

    m_brightness = brightness;
    m_contrast = contrast;

    update();
}

// ---------------------------------------------------------------------------------------------- //

auto ColorAdjustFilter::getBrightness() const -> int
{
    return m_brightness;
}

// ---------------------------------------------------------------------------------------------- //

auto ColorAdjustFilter::getContrast() const -> int
{
    return m_contrast;
}

// ---------------------------------------------------------------------------------------------- //

auto ColorAdjustFilter::createWorker() -> std::unique_ptr<FilterWorker>
{
    return std::make_unique<ColorAdjustFilterWorker>(this);
}

// ---------------------------------------------------------------------------------------------- //

ColorAdjustFilterWorker::ColorAdjustFilterWorker(ColorAdjustFilter* filter)
    : FilterWorker(filter),
      m_filter(filter),
      m_brightness(m_filter->getBrightness()),
      m_contrast(m_filter->getContrast())
{
    updateFactors();
}

// ---------------------------------------------------------------------------------------------- //

void ColorAdjustFilterWorker::updateImpl()
{
    const int brightness = m_filter->getBrightness();
    const int contrast = m_filter->getContrast();

    if (brightness != m_brightness || contrast != m_contrast)
    {
        m_brightness = brightness;
        m_contrast = contrast;

        updateFactors();
    }
}

// ---------------------------------------------------------------------------------------------- //

void ColorAdjustFilterWorker::runImpl(const Buffer& input, Buffer* output)
{
    convert(input, output, m_alpha, m_beta);
}

// ---------------------------------------------------------------------------------------------- //

void ColorAdjustFilterWorker::updateFactors()
{
    const float factor = (m_contrast >= 0) ? 9.0F : 1.0F;

    m_alpha = 1.0F + (factor / 100.0F * static_cast<float>(m_contrast));
    m_beta = static_cast<float>(m_brightness) * 0.01F;
}

// ---------------------------------------------------------------------------------------------- //
