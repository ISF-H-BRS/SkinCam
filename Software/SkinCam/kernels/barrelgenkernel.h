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

#include "abstractkernel.h"

#include <cmath> // for sqrt()

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

class BarrelGenKernel : public AbstractKernel
{
public:
    using OutputBuffer = ComputeBuffer2f;

public:
    BarrelGenKernel();

    void operator()(OutputBuffer* output, float factor);
};

// ---------------------------------------------------------------------------------------------- //

inline
BarrelGenKernel::BarrelGenKernel()
    : AbstractKernel("barrelgen") {}

// ---------------------------------------------------------------------------------------------- //

inline
void BarrelGenKernel::operator()(OutputBuffer* output, float factor)
{
    const float halfWidth = static_cast<float>(output->width() / 2);
    const float halfHeight = static_cast<float>(output->height() / 2);

    const float radius = std::sqrt(halfWidth*halfWidth + halfHeight*halfHeight);

    const float hScale = (factor >= 0.0F) ? radius : halfWidth;
    const float vScale = (factor >= 0.0F) ? radius : halfHeight;

    setArg(0, output);
    setArg(1, factor);
    setArg(2, halfWidth);
    setArg(3, halfHeight);
    setArg(4, hScale);
    setArg(5, vScale);

    enqueue(output->width(), output->height());
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
