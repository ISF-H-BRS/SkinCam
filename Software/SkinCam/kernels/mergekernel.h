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
class MergeKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<U>;

public:
    MergeKernel();

    void operator()(const InputBuffer& input0, const InputBuffer& input1, OutputBuffer* output);

    void operator()(const InputBuffer& input0, const InputBuffer& input1,
                    const InputBuffer& input2, OutputBuffer* output);

    void operator()(const InputBuffer& input0, const InputBuffer& input1,
                    const InputBuffer& input2, const InputBuffer& input3,
                    OutputBuffer* output);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T, typename U>
void MergeKernel<T,U>::operator()(const InputBuffer& input0, const InputBuffer& input1,
                                  OutputBuffer* output)
{
    ASSERT(input0.pixelCount() == output->pixelCount());
    ASSERT(input1.pixelCount() == output->pixelCount());

    setArg(0, input0);
    setArg(1, input1);
    setArg(2, output);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, typename U>
void MergeKernel<T,U>::operator()(const InputBuffer& input0, const InputBuffer& input1,
                                  const InputBuffer& input2, OutputBuffer* output)
{
    ASSERT(input0.pixelCount() == output->pixelCount());
    ASSERT(input1.pixelCount() == output->pixelCount());
    ASSERT(input2.pixelCount() == output->pixelCount());

    setArg(0, input0);
    setArg(1, input1);
    setArg(2, input2);
    setArg(3, output);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, typename U>
void MergeKernel<T,U>::operator()(const InputBuffer& input0, const InputBuffer& input1,
                                  const InputBuffer& input2, const InputBuffer& input3,
                                  OutputBuffer* output)
{
    ASSERT(input0.pixelCount() == output->pixelCount());
    ASSERT(input1.pixelCount() == output->pixelCount());
    ASSERT(input2.pixelCount() == output->pixelCount());
    ASSERT(input3.pixelCount() == output->pixelCount());

    setArg(0, input0);
    setArg(1, input1);
    setArg(2, input2);
    setArg(3, input3);
    setArg(4, output);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (T, CL_T)
//      foreach (N, CL_N)

// ---------------------------------------------------------------------------------------------- //

#define MERGE_KERNEL_SPECIALIZE(T, CL_T, N)                         \
template <> inline MergeKernel<Vec1##T,Vec##N##T>::MergeKernel()    \
    : AbstractKernel("merge", { "TYPE=" #CL_T, "N=" #N }) {}        \

#define MERGE_KERNEL_FOREACH_N(T, CL_T, N)  \
    MERGE_KERNEL_SPECIALIZE(T, CL_T, N)     \

#define MERGE_KERNEL_FOREACH_T(T, CL_T)   \
    MERGE_KERNEL_FOREACH_N(T, CL_T, 2)    \
    MERGE_KERNEL_FOREACH_N(T, CL_T, 3)    \
    MERGE_KERNEL_FOREACH_N(T, CL_T, 4)    \

// ---------------------------------------------------------------------------------------------- //

MERGE_KERNEL_FOREACH_T(b, uchar)
MERGE_KERNEL_FOREACH_T(w, ushort)
MERGE_KERNEL_FOREACH_T(u, uint)
MERGE_KERNEL_FOREACH_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
