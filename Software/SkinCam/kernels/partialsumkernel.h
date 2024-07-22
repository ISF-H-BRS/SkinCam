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
class PartialSumKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<U>;

public:
    PartialSumKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T, typename U>
void PartialSumKernel<T,U>::operator()(const InputBuffer& input, OutputBuffer* output)
{
    ASSERT(input.pixelCount() % output->pixelCount() == 0);

    const uint stride = input.pixelCount() / output->pixelCount();

    setArg(0, input);
    setArg(1, output);
    setArg(2, stride);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (IN_T)
//      foreach (OUT_T)
//          foreach (N)

// ---------------------------------------------------------------------------------------------- //

#define PARTIALSUM_KERNEL_SPECIALIZE(IN_T, OUT_T, CL_IN_T, CL_OUT_T)        \
template <> inline PartialSumKernel<IN_T,OUT_T>::PartialSumKernel()         \
    : AbstractKernel("partialsum", { "IN=" #CL_IN_T, "OUT=" #CL_OUT_T }) {} \

#define PARTIALSUM_KERNEL_FOREACH_N(IN_T, CL_IN_T, OUT_T, CL_OUT_T, N, CL_N)                    \
    PARTIALSUM_KERNEL_SPECIALIZE(Vec##N##IN_T, Vec##N##OUT_T, CL_IN_T##CL_N, CL_OUT_T##CL_N)    \

#define PARTIALSUM_KERNEL_FOREACH_OUT_T(IN_T, CL_IN_T, OUT_T, CL_OUT_T) \
    PARTIALSUM_KERNEL_FOREACH_N(IN_T, CL_IN_T, OUT_T, CL_OUT_T, 1,  )   \
    PARTIALSUM_KERNEL_FOREACH_N(IN_T, CL_IN_T, OUT_T, CL_OUT_T, 2, 2)   \
    PARTIALSUM_KERNEL_FOREACH_N(IN_T, CL_IN_T, OUT_T, CL_OUT_T, 3, 3)   \
    PARTIALSUM_KERNEL_FOREACH_N(IN_T, CL_IN_T, OUT_T, CL_OUT_T, 4, 4)   \

#define PARTIALSUM_KERNEL_FOREACH_IN_T(IN_T, CL_IN_T)           \
    PARTIALSUM_KERNEL_FOREACH_OUT_T(IN_T, CL_IN_T, b, uchar)    \
    PARTIALSUM_KERNEL_FOREACH_OUT_T(IN_T, CL_IN_T, w, ushort)   \
    PARTIALSUM_KERNEL_FOREACH_OUT_T(IN_T, CL_IN_T, u, uint)     \
    PARTIALSUM_KERNEL_FOREACH_OUT_T(IN_T, CL_IN_T, f, float)    \

// ---------------------------------------------------------------------------------------------- //

PARTIALSUM_KERNEL_FOREACH_IN_T(b, uchar)
PARTIALSUM_KERNEL_FOREACH_IN_T(w, ushort)
PARTIALSUM_KERNEL_FOREACH_IN_T(u, uint)
PARTIALSUM_KERNEL_FOREACH_IN_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
