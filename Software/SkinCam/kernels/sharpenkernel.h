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
#include "gaussian.h"

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

template <typename T>
class SharpenKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<T>;

public:
    SharpenKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output,
                    const InputBuffer& blurred, const T& threshold, float amount);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void SharpenKernel<T>::operator()(const InputBuffer& input, OutputBuffer* output,
                                  const InputBuffer& blurred, const T& threshold, float amount)
{
    ASSERT(input.pixelCount() == output->pixelCount());

    setArg(0, input);
    setArg(1, output);
    setArg(2, blurred);
    setArg(3, threshold);
    setArg(4, amount);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (T)
//      foreach (N)

// ---------------------------------------------------------------------------------------------- //

#define SHARPEN_KERNEL_SPECIALIZE(T, CL_T, CL_N)                \
template <> inline SharpenKernel<T>::SharpenKernel()            \
    : AbstractKernel("sharpen", { "T=" #CL_T, "N=" #CL_N }) {}  \

#define SHARPEN_KERNEL_FOREACH_N(T, CL_T, N, CL_N)      \
    SHARPEN_KERNEL_SPECIALIZE(Vec##N##T, CL_T, CL_N)    \

#define SHARPEN_KERNEL_FOREACH_T(T, CL_T)   \
    SHARPEN_KERNEL_FOREACH_N(T, CL_T, 1,  ) \
    SHARPEN_KERNEL_FOREACH_N(T, CL_T, 2, 2) \
    SHARPEN_KERNEL_FOREACH_N(T, CL_T, 3, 3) \
    SHARPEN_KERNEL_FOREACH_N(T, CL_T, 4, 4) \

// ---------------------------------------------------------------------------------------------- //

SHARPEN_KERNEL_FOREACH_T(b, uchar)
SHARPEN_KERNEL_FOREACH_T(w, ushort)
SHARPEN_KERNEL_FOREACH_T(u, uint)
SHARPEN_KERNEL_FOREACH_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
