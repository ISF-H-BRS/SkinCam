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

template <typename T, typename U>
class SubtractKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<T>;
    using SubtrahendBuffer = ComputeBuffer<U>;

public:
    SubtractKernel();

    void operator()(const InputBuffer& input, const SubtrahendBuffer& subtrahend,
                    OutputBuffer* output);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T, typename U>
void SubtractKernel<T,U>::operator()(const InputBuffer& input, const SubtrahendBuffer& subtrahend,
                                     OutputBuffer* output)
{
    ASSERT(subtrahend.pixelCount() == input.pixelCount());
    ASSERT(output->pixelCount() == input.pixelCount());

    setArg(0, input);
    setArg(1, subtrahend);
    setArg(2, output);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (TYPE)
//      foreach (N)

// ---------------------------------------------------------------------------------------------- //

#define SUBTRACT_KERNEL_SPECIALIZE(INOUT_T, SUB_T, CL_TYPE, CL_N)   \
template <> inline SubtractKernel<INOUT_T, SUB_T>::SubtractKernel() \
    : AbstractKernel("subtract", { "TYPE=" #CL_TYPE,                \
                                   "   N=" #CL_N }) {}              \

#define SUBTRACT_KERNEL_FOREACH_N(T, CL_T, N, CL_N)             \
    SUBTRACT_KERNEL_SPECIALIZE(Vec##N##T, Vec1##T, CL_T, CL_N)  \

#define SUBTRACT_KERNEL_FOREACH_TYPE(T, CL_T)   \
    SUBTRACT_KERNEL_FOREACH_N(T, CL_T, 1,  )    \
    SUBTRACT_KERNEL_FOREACH_N(T, CL_T, 2, 2)    \
    SUBTRACT_KERNEL_FOREACH_N(T, CL_T, 3, 3)    \
    SUBTRACT_KERNEL_FOREACH_N(T, CL_T, 4, 4)    \

// ---------------------------------------------------------------------------------------------- //

SUBTRACT_KERNEL_FOREACH_TYPE(b, uchar)
SUBTRACT_KERNEL_FOREACH_TYPE(w, ushort)
SUBTRACT_KERNEL_FOREACH_TYPE(u, uint)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
