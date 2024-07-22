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
#include "global.h"
#include "kernels.h"

SKINCAM_BEGIN_NAMESPACE();

class ConvertResults
{
public:
    using InputBuffer = ComputeBuffer3f;
    using ChannelBuffer = ComputeBuffer1b;
    using CompositeBuffer = ComputeBuffer4b;

public:
    void operator()(const InputBuffer& input,
                    ChannelBuffer* channel0, ChannelBuffer* channel1,
                    ChannelBuffer* channel2, CompositeBuffer* composite);

private:
    ConvertKernel<Vec3f, Vec3b> convert;
    SplitKernel<Vec3b, Vec1b> split;
    WidenKernel<Vec3b, Vec4b> widen;

private:
    ComputeBuffer3b m_composite { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };
};

SKINCAM_END_NAMESPACE();
