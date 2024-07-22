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

enum class FlipMode
{
    Horizontal,
    Vertical,
    Both
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
class FlipKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<T>;

public:
    FlipKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output, FlipMode mode);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void FlipKernel<T>::operator()(const InputBuffer& input, OutputBuffer* output, FlipMode mode)
{
    ASSERT(input.pixelCount() == output->pixelCount());

    setArg(0, input);
    setArg(1, output);
    setArg(2, static_cast<uint>(mode));

    enqueue(output->width(), output->height());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (T, CL_T)
//      foreach (N, CL_N)

// ---------------------------------------------------------------------------------------------- //

#define FLIP_KERNEL_SPECIALIZE(T, CL_T, N, CL_N)            \
template <> inline FlipKernel<Vec##N##T>::FlipKernel()      \
    : AbstractKernel("flip", { "TYPE=" #CL_T #CL_N }) {}    \

#define FLIP_KERNEL_FOREACH_N(T, CL_T, N, CL_N) \
    FLIP_KERNEL_SPECIALIZE(T, CL_T, N, CL_N)    \

#define FLIP_KERNEL_FOREACH_T(T, CL_T)     \
    FLIP_KERNEL_FOREACH_N(T, CL_T, 1,  )   \
    FLIP_KERNEL_FOREACH_N(T, CL_T, 2, 2)   \
    FLIP_KERNEL_FOREACH_N(T, CL_T, 3, 3)   \
    FLIP_KERNEL_FOREACH_N(T, CL_T, 4, 4)   \

// ---------------------------------------------------------------------------------------------- //

FLIP_KERNEL_FOREACH_T(b, uchar)
FLIP_KERNEL_FOREACH_T(w, ushort)
FLIP_KERNEL_FOREACH_T(u, uint)
FLIP_KERNEL_FOREACH_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
