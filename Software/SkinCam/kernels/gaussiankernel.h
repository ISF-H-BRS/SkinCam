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
class GaussianKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<Vec1f>;

public:
    GaussianKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output,
                    const T& mean, const T& variance);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void GaussianKernel<T>::operator()(const InputBuffer& input, OutputBuffer* output,
                                   const T& mean, const T& variance)
{
    ASSERT(output->pixelCount() == input.pixelCount());

    setArg(0, input);
    setArg(1, output);
    setArg(2, mean);
    setArg(3, variance);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (T, CL_T)
//      foreach (N, CL_N)

// ---------------------------------------------------------------------------------------------- //

#define GAUSSIAN_KERNEL_SPECIALIZE(IN, CL_IN, CL_N)                 \
template <> inline GaussianKernel<IN>::GaussianKernel()             \
    : AbstractKernel("gaussian", { "IN=" #CL_IN, "N=" #CL_N }) {}   \

#define GAUSSIAN_KERNEL_FOREACH_N(T, CL_T, N, CL_N)         \
    GAUSSIAN_KERNEL_SPECIALIZE(Vec##N##T, CL_T##CL_N, CL_N) \

#define GAUSSIAN_KERNEL_FOREACH_T(T, CL_T)      \
    GAUSSIAN_KERNEL_FOREACH_N(T, CL_T, 1,  )    \
    GAUSSIAN_KERNEL_FOREACH_N(T, CL_T, 2, 2)    \
    GAUSSIAN_KERNEL_FOREACH_N(T, CL_T, 3, 3)    \
    GAUSSIAN_KERNEL_FOREACH_N(T, CL_T, 4, 4)    \

// ---------------------------------------------------------------------------------------------- //

GAUSSIAN_KERNEL_FOREACH_T(b, uchar)
GAUSSIAN_KERNEL_FOREACH_T(w, ushort)
GAUSSIAN_KERNEL_FOREACH_T(u, uint)
GAUSSIAN_KERNEL_FOREACH_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
