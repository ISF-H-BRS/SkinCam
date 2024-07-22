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

class SharpeningFilter : public Filter
{
public:
    static constexpr uint MinimumRadius =  1;
    static constexpr uint MaximumRadius = 10;
    static constexpr uint DefaultRadius =  1;

    static constexpr float MinimumAmount =   0.0F;
    static constexpr float MaximumAmount = 100.0F;
    static constexpr float DefaultAmount =   6.0F;

    static constexpr float MinimumThreshold = 0.0F;
    static constexpr float MaximumThreshold = 1.0F;
    static constexpr float DefaultThreshold = 0.0F;

public:
    SharpeningFilter();

    void setParameters(uint radius, float amount, float threshold);

    auto getRadius() const -> uint;
    auto getAmount() const -> float;
    auto getThreshold() const -> float;

    auto createWorker() -> std::unique_ptr<FilterWorker> override;

private:
    uint m_radius = DefaultRadius;
    float m_amount = DefaultAmount;
    float m_threshold = DefaultThreshold;
};

SKINCAM_END_NAMESPACE();
