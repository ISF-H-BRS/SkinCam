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
class ThresholdKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<U>;

public:
    ThresholdKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output,
                    const T& lower, const T& upper, const U& positive, const U& negative);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T, typename U>
void ThresholdKernel<T,U>::operator()(const InputBuffer& input, OutputBuffer* output,
                                      const T& lower, const T& upper,
                                      const U& positive, const U& negative)
{
    ASSERT(input.pixelCount() == output->pixelCount());

    setArg(0, input);
    setArg(1, output);
    setArg(2, lower);
    setArg(3, upper);
    setArg(4, positive);
    setArg(5, negative);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (IN_T, CL_IN_T)
//      foreach (IN_N, CL_IN_N)
//          foreach (OUT_T, CL_OUT_T)
//              foreach (OUT_N, CL_OUT_N)

// ---------------------------------------------------------------------------------------------- //

#define THRESHOLD_KERNEL_SPECIALIZE(IN, CL_IN, OUT, CL_OUT, IN_N)                       \
template <> inline ThresholdKernel<IN,OUT>::ThresholdKernel()                           \
    : AbstractKernel("threshold", { "IN=" #CL_IN, "OUT=" #CL_OUT, "IN_N=" #IN_N }) {}   \

#define THRESHOLD_KERNEL_FOREACH_OUT_N(IN_T, CL_IN_T, IN_N, CL_IN_N,            \
                                       OUT_T, CL_OUT_T, OUT_N, CL_OUT_N)        \
    THRESHOLD_KERNEL_SPECIALIZE(Vec##IN_N##IN_T,   CL_IN_T##CL_IN_N,            \
                                Vec##OUT_N##OUT_T, CL_OUT_T##CL_OUT_N, IN_N);   \

#define THRESHOLD_KERNEL_FOREACH_OUT_T(IN_T, CL_IN_T, IN_N, CL_IN_N, OUT_T, CL_OUT_T)   \
    THRESHOLD_KERNEL_FOREACH_OUT_N(IN_T, CL_IN_T, IN_N, CL_IN_N, OUT_T, CL_OUT_T, 1,  ) \
    THRESHOLD_KERNEL_FOREACH_OUT_N(IN_T, CL_IN_T, IN_N, CL_IN_N, OUT_T, CL_OUT_T, 2, 2) \
    THRESHOLD_KERNEL_FOREACH_OUT_N(IN_T, CL_IN_T, IN_N, CL_IN_N, OUT_T, CL_OUT_T, 3, 3) \
    THRESHOLD_KERNEL_FOREACH_OUT_N(IN_T, CL_IN_T, IN_N, CL_IN_N, OUT_T, CL_OUT_T, 4, 4) \

#define THRESHOLD_KERNEL_FOREACH_IN_N(IN_T, CL_IN_T, IN_N, CL_IN_N)         \
    THRESHOLD_KERNEL_FOREACH_OUT_T(IN_T, CL_IN_T, IN_N, CL_IN_N, b, uchar)  \
    THRESHOLD_KERNEL_FOREACH_OUT_T(IN_T, CL_IN_T, IN_N, CL_IN_N, w, ushort) \
    THRESHOLD_KERNEL_FOREACH_OUT_T(IN_T, CL_IN_T, IN_N, CL_IN_N, u, uint)   \
    THRESHOLD_KERNEL_FOREACH_OUT_T(IN_T, CL_IN_T, IN_N, CL_IN_N, f, float)  \

#define THRESHOLD_KERNEL_FOREACH_IN_T(IN_T, CL_IN_T)    \
    THRESHOLD_KERNEL_FOREACH_IN_N(IN_T, CL_IN_T, 1,  )  \
    THRESHOLD_KERNEL_FOREACH_IN_N(IN_T, CL_IN_T, 2, 2)  \
    THRESHOLD_KERNEL_FOREACH_IN_N(IN_T, CL_IN_T, 3, 3)  \
    THRESHOLD_KERNEL_FOREACH_IN_N(IN_T, CL_IN_T, 4, 4)  \

// ---------------------------------------------------------------------------------------------- //

THRESHOLD_KERNEL_FOREACH_IN_T(b, uchar)
THRESHOLD_KERNEL_FOREACH_IN_T(w, ushort)
THRESHOLD_KERNEL_FOREACH_IN_T(u, uint)
THRESHOLD_KERNEL_FOREACH_IN_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
