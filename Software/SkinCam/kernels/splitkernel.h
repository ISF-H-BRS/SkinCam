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
class SplitKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<U>;

public:
    SplitKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output0,
                                              OutputBuffer* output1);

    void operator()(const InputBuffer& input, OutputBuffer* output0,
                                              OutputBuffer* output1,
                                              OutputBuffer* output2);

    void operator()(const InputBuffer& input, OutputBuffer* output0,
                                              OutputBuffer* output1,
                                              OutputBuffer* output2,
                                              OutputBuffer* output3);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T, typename U>
void SplitKernel<T,U>::operator()(const InputBuffer& input, OutputBuffer* output0,
                                                            OutputBuffer* output1)
{
    ASSERT(output0->pixelCount() == input.pixelCount());
    ASSERT(output1->pixelCount() == input.pixelCount());

    setArg(0, input);
    setArg(1, output0);
    setArg(2, output1);

    enqueue(input.pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, typename U>
void SplitKernel<T,U>::operator()(const InputBuffer& input, OutputBuffer* output0,
                                                            OutputBuffer* output1,
                                                            OutputBuffer* output2)
{
    ASSERT(output0->pixelCount() == input.pixelCount());
    ASSERT(output1->pixelCount() == input.pixelCount());
    ASSERT(output2->pixelCount() == input.pixelCount());

    setArg(0, input);
    setArg(1, output0);
    setArg(2, output1);
    setArg(3, output2);

    enqueue(input.pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, typename U>
void SplitKernel<T,U>::operator()(const InputBuffer& input, OutputBuffer* output0,
                                                            OutputBuffer* output1,
                                                            OutputBuffer* output2,
                                                            OutputBuffer* output3)
{
    ASSERT(output0->pixelCount() == input.pixelCount());
    ASSERT(output1->pixelCount() == input.pixelCount());
    ASSERT(output2->pixelCount() == input.pixelCount());
    ASSERT(output3->pixelCount() == input.pixelCount());

    setArg(0, input);
    setArg(1, output0);
    setArg(2, output1);
    setArg(3, output2);
    setArg(4, output3);

    enqueue(input.pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (T, CL_T)
//      foreach (N, CL_N)

// ---------------------------------------------------------------------------------------------- //

#define SPLIT_KERNEL_SPECIALIZE(T, CL_T, N)                                 \
template <> inline SplitKernel<Vec##N##T,Vec1##T>::SplitKernel()            \
    : AbstractKernel("split", { "IN=" #CL_T #N, "OUT=" #CL_T, "N=" #N }) {} \

#define SPLIT_KERNEL_FOREACH_N(T, CL_T, N)  \
    SPLIT_KERNEL_SPECIALIZE(T, CL_T, N)     \

#define SPLIT_KERNEL_FOREACH_T(T, CL_T)   \
    SPLIT_KERNEL_FOREACH_N(T, CL_T, 2)    \
    SPLIT_KERNEL_FOREACH_N(T, CL_T, 3)    \
    SPLIT_KERNEL_FOREACH_N(T, CL_T, 4)    \

// ---------------------------------------------------------------------------------------------- //

SPLIT_KERNEL_FOREACH_T(b, uchar)
SPLIT_KERNEL_FOREACH_T(w, ushort)
SPLIT_KERNEL_FOREACH_T(u, uint)
SPLIT_KERNEL_FOREACH_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
