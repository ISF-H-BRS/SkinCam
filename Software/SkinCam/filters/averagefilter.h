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

class AverageFilter : public Filter
{
public:
    static constexpr int MinimumSampleCount = 2;
    static constexpr int MaximumSampleCount = 4;
    static constexpr int DefaultSampleCount = 2;

public:
    AverageFilter();

    void setSampleCount(int count);
    auto getSampleCount() const -> int;

    auto createWorker() -> std::unique_ptr<FilterWorker> override;

private:
    int m_sampleCount = DefaultSampleCount;
};

SKINCAM_END_NAMESPACE();
