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

#include "filter.h"

SKINCAM_BEGIN_NAMESPACE();

class ColorAdjustFilter : public Filter
{
public:
    static constexpr int MinimumBrightness = -100;
    static constexpr int MaximumBrightness =  100;

    static constexpr int MinimumContrast = -100;
    static constexpr int MaximumContrast =  100;

public:
    ColorAdjustFilter();

    void setParameters(int brightness, int contrast);

    auto getBrightness() const -> int;
    auto getContrast() const -> int;

    auto createWorker() -> std::unique_ptr<FilterWorker> override;

private:
    int m_brightness = 0;
    int m_contrast = 0;
};

SKINCAM_END_NAMESPACE();
