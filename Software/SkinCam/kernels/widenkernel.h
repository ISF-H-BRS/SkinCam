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
class WidenKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<U>;

public:
    WidenKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output, const U& fill);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T, typename U>
void WidenKernel<T,U>::operator()(const InputBuffer& input, OutputBuffer* output, const U& fill)
{
    ASSERT(input.pixelCount() == output->pixelCount());

    setArg(0, input);
    setArg(1, output);
    setArg(2, fill);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (T, CL_T)
//      foreach (IN_N, CL_IN_N, OUT_N)

// ---------------------------------------------------------------------------------------------- //

#define WIDEN_KERNEL_SPECIALIZE(T, CL_T, IN_N, CL_IN_N, OUT_N)                  \
template <> inline WidenKernel<Vec##IN_N##T, Vec##OUT_N##T>::WidenKernel()      \
    : AbstractKernel("widen",                                                   \
            { "IN=" #CL_T #CL_IN_N, "OUT=" #CL_T #OUT_N, "IN_N=" #IN_N }) {}    \

#define WIDEN_KERNEL_FOREACH_N(T, CL_T, IN_N, CL_IN_N, OUT_N)   \
    WIDEN_KERNEL_SPECIALIZE(T, CL_T, IN_N, CL_IN_N, OUT_N)      \

#define WIDEN_KERNEL_FOREACH_T(T, CL_T)         \
    WIDEN_KERNEL_FOREACH_N(T, CL_T, 1,  , 2)    \
    WIDEN_KERNEL_FOREACH_N(T, CL_T, 1,  , 3)    \
    WIDEN_KERNEL_FOREACH_N(T, CL_T, 1,  , 4)    \
    WIDEN_KERNEL_FOREACH_N(T, CL_T, 2, 2, 3)    \
    WIDEN_KERNEL_FOREACH_N(T, CL_T, 2, 2, 4)    \
    WIDEN_KERNEL_FOREACH_N(T, CL_T, 3, 3, 4)    \

// ---------------------------------------------------------------------------------------------- //

WIDEN_KERNEL_FOREACH_T(b, uchar)
WIDEN_KERNEL_FOREACH_T(w, ushort)
WIDEN_KERNEL_FOREACH_T(u, uint)
WIDEN_KERNEL_FOREACH_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
