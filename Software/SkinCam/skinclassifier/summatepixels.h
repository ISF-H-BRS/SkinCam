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

class SummatePixels
{
public:
    using InputBuffer = ComputeBuffer1b;

public:
    auto operator()(const InputBuffer& buffer) -> uint;

private:
    PartialSumKernel<Vec1b, Vec1u> sum1;
    PartialSumKernel<Vec1u, Vec1u> sum2;

private:
    ComputeBuffer1u m_sum1 { ImageHeight, 1, CL_MEM_READ_WRITE };
    ComputeBuffer1u m_sum2 { ImageHeight/4, 1, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR };
};

SKINCAM_END_NAMESPACE();
