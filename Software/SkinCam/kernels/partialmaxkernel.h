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
class PartialMaxKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<T>;

public:
    PartialMaxKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void PartialMaxKernel<T>::operator()(const InputBuffer& input, OutputBuffer* output)
{
    ASSERT(input.pixelCount() % output->pixelCount() == 0);

    const uint stride = input.pixelCount() / output->pixelCount();

    setArg(0, input);
    setArg(1, output);
    setArg(2, stride);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (T)
//      foreach (N)

// ---------------------------------------------------------------------------------------------- //

#define PARTIALMAX_KERNEL_SPECIALIZE(T, CL_T, N)                \
template <> inline PartialMaxKernel<T>::PartialMaxKernel()      \
    : AbstractKernel("partialmax", { "T=" #CL_T, "N=" #N }) {}  \

#define PARTIALMAX_KERNEL_FOREACH_N(T, CL_T, N)         \
    PARTIALMAX_KERNEL_SPECIALIZE(Vec##N##T, CL_T, N)    \

#define PARTIALMAX_KERNEL_FOREACH_T(T, CL_T)    \
    PARTIALMAX_KERNEL_FOREACH_N(T, CL_T, 1)     \
    PARTIALMAX_KERNEL_FOREACH_N(T, CL_T, 2)     \
    PARTIALMAX_KERNEL_FOREACH_N(T, CL_T, 3)     \
    PARTIALMAX_KERNEL_FOREACH_N(T, CL_T, 4)     \

// ---------------------------------------------------------------------------------------------- //

PARTIALMAX_KERNEL_FOREACH_T(b, uchar)
PARTIALMAX_KERNEL_FOREACH_T(w, ushort)
PARTIALMAX_KERNEL_FOREACH_T(u, uint)
PARTIALMAX_KERNEL_FOREACH_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
