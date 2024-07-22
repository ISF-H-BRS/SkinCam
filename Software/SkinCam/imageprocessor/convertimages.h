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
#include "memorybuffer.h"

SKINCAM_BEGIN_NAMESPACE();

class ConvertImages
{
public:
    using InputBuffer = MemoryBuffer1w;
    using RawBuffer = ComputeBuffer1b;
    using CompositeBuffer = ComputeBuffer3f;

public:
    void operator()(const InputBuffer& dark, const InputBuffer& channel0,
                    const InputBuffer& channel1, const InputBuffer& channel2,
                    RawBuffer* rawDark, RawBuffer* rawChannel0,
                    RawBuffer* rawChannel1, RawBuffer* rawChannel2,
                    CompositeBuffer* composite);

private:
    MergeKernel<Vec1w, Vec3w> merge;
    SubtractKernel<Vec3w, Vec1w> subtract;
    ConvertKernel<Vec1w, Vec1b> convertRaw;
    ConvertKernel<Vec3w, Vec3f> convertComposite;

private:
    ComputeBuffer1w m_dark { ImageWidth, ImageHeight, CL_MEM_READ_ONLY };
    ComputeBuffer1w m_channel0 { ImageWidth, ImageHeight, CL_MEM_READ_ONLY };
    ComputeBuffer1w m_channel1 { ImageWidth, ImageHeight, CL_MEM_READ_ONLY };
    ComputeBuffer1w m_channel2 { ImageWidth, ImageHeight, CL_MEM_READ_ONLY };

    ComputeBuffer3w m_merged { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };
    ComputeBuffer3w m_subtracted { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };
};

SKINCAM_END_NAMESPACE();
