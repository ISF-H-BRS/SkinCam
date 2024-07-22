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
class BlurKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<T>;
    using WeightBuffer = ComputeBuffer<Vec1f>;

public:
    BlurKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output, const WeightBuffer& weights);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void BlurKernel<T>::operator()(const InputBuffer& input, OutputBuffer* output,
                               const WeightBuffer& weights)
{
    ASSERT(output->width() == input.width() && output->height() == input.height());

    setArg(0, input);
    setArg(1, output);
    setArg(2, weights);
    setArg(3, static_cast<int>(weights.width() - 1));

    enqueue(output->width(), output->height());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (T)
//      foreach (N)

// ---------------------------------------------------------------------------------------------- //

#define BLUR_KERNEL_SPECIALIZE(T, CL_T, N, CL_N)            \
template <> inline BlurKernel<Vec##N##T>::BlurKernel()      \
    : AbstractKernel("blur", { "T=" #CL_T, "N=" #CL_N }) {} \

#define BLUR_KERNEL_FOREACH_N(T, CL_T, N, CL_N)   \
    BLUR_KERNEL_SPECIALIZE(T, CL_T, N, CL_N) \

#define BLUR_KERNEL_FOREACH_T(T, CL_T)    \
    BLUR_KERNEL_FOREACH_N(T, CL_T, 1,  )  \
    BLUR_KERNEL_FOREACH_N(T, CL_T, 2, 2)  \
    BLUR_KERNEL_FOREACH_N(T, CL_T, 3, 3)  \
    BLUR_KERNEL_FOREACH_N(T, CL_T, 4, 4)  \

// ---------------------------------------------------------------------------------------------- //

BLUR_KERNEL_FOREACH_T(b, uchar)
BLUR_KERNEL_FOREACH_T(w, ushort)
BLUR_KERNEL_FOREACH_T(u, uint)
BLUR_KERNEL_FOREACH_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
