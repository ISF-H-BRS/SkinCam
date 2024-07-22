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

#include "computebuffer.h"
#include "kernels.h"
#include "summatepixels.h"

SKINCAM_BEGIN_NAMESPACE();

class CountSkinPixels
{
public:
    using InputBuffer = ComputeBuffer1f;
    using MaskBuffer = ComputeBuffer1b;

public:
    auto operator()(const InputBuffer& buffer, float threshold) -> uint;
    auto operator()(const InputBuffer& buffer, const MaskBuffer& mask, float threshold) -> uint;

private:
    ThresholdKernel<Vec1f, Vec1b> threshold;
    MaskKernel<Vec1b> mask;

    SummatePixels summate;

private:
    auto process(const InputBuffer& buffer, const MaskBuffer* mask, float threshold) -> uint;

private:
    ComputeBuffer1b m_skinMask { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };
};

SKINCAM_END_NAMESPACE();
