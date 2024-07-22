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

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

template <typename T>
class NormDiffKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<Vec3f>;

public:
    NormDiffKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void NormDiffKernel<T>::operator()(const InputBuffer& input, OutputBuffer* output)
{
    ASSERT(output->pixelCount() == input.pixelCount());

    setArg(0, input);
    setArg(1, output);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

#define NORMDIFF_KERNEL_SPECIALIZE(TYPE, CL_TYPE)       \
template <>                                             \
inline NormDiffKernel<TYPE>::NormDiffKernel()           \
    : AbstractKernel("normdiff", { "IN=" #CL_TYPE }) {} \

// ---------------------------------------------------------------------------------------------- //

NORMDIFF_KERNEL_SPECIALIZE(Vec3b, uchar3)
NORMDIFF_KERNEL_SPECIALIZE(Vec3w, ushort3)
NORMDIFF_KERNEL_SPECIALIZE(Vec3u, uint3)
NORMDIFF_KERNEL_SPECIALIZE(Vec3f, float3)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
