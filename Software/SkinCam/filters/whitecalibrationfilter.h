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

class WhiteCalibrationFilter : public Filter
{
public:
    static constexpr double MinimumFactor = 0.0001;
    static constexpr double MaximumFactor = 1.0000;
    static constexpr double DefaultFactor = 1.0000;

public:
    WhiteCalibrationFilter();

    void setFactors(double factor0, double factor1, double factor2);
    auto getFactor(Channel channel) const -> double ;

    auto createWorker() -> std::unique_ptr<FilterWorker> override;

private:
    std::array<double, ChannelCount> m_factors = { DefaultFactor, DefaultFactor, DefaultFactor };
};

SKINCAM_END_NAMESPACE();
