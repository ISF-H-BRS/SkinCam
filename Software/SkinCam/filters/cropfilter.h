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

class CropFilter : public Filter
{
public:
    static constexpr int MinimumHorizontalBorder = 0;
    static constexpr int MaximumHorizontalBorder = ImageWidth/2 - 2;

    static constexpr int MinimumVerticalBorder = 0;
    static constexpr int MaximumVerticalBorder = ImageHeight/2 - 2;

public:
    CropFilter();

    void setBorders(int left, int right, int top, int bottom);
    auto getBorders() const -> std::tuple<int,int,int,int>;

    auto createWorker() -> std::unique_ptr<FilterWorker> override;

private:
    std::tuple<int,int,int,int> m_borders = std::make_tuple(0, 0, 0, 0);
};

SKINCAM_END_NAMESPACE();
