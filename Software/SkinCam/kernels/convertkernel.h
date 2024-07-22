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
class ConvertKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<U>;

public:
    ConvertKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output,
                    float alpha = 1.0F, float beta = 0.0F);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T, typename U>
void ConvertKernel<T,U>::operator()(const InputBuffer& input, OutputBuffer* output,
                                    float alpha, float beta)
{
    ASSERT(output->pixelCount() == input.pixelCount());

    setArg(0, input);
    setArg(1, output);
    setArg(2, alpha);
    setArg(3, beta);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (N)
//      foreach (IN_T)
//          foreach (OUT_T)

// ---------------------------------------------------------------------------------------------- //

#define CONVERT_KERNEL_SPECIALIZE(IN_T, OUT_T, CL_IN_T, CL_OUT_T, CL_N, CL_SAT) \
template <> inline ConvertKernel<IN_T, OUT_T>::ConvertKernel()                  \
    : AbstractKernel("convert", { " IN=" #CL_IN_T,                              \
                                  "OUT=" #CL_OUT_T,                             \
                                  "  N=" #CL_N,                                 \
                                  "SAT=" #CL_SAT }) {}                          \

#define CONVERT_KERNEL_FOREACH_OUT_T(N, CL_N, IN_T, CL_IN_T, OUT_T, CL_OUT_T, CL_SAT)   \
    CONVERT_KERNEL_SPECIALIZE(Vec##N##IN_T, Vec##N##OUT_T,                              \
                              CL_IN_T##CL_N, CL_OUT_T##CL_N, CL_N, CL_SAT)              \

#define CONVERT_KERNEL_FOREACH_IN_T(N, CL_N, IN_T, CL_IN_T)                 \
    CONVERT_KERNEL_FOREACH_OUT_T(N, CL_N, IN_T, CL_IN_T, b, uchar,  _sat)   \
    CONVERT_KERNEL_FOREACH_OUT_T(N, CL_N, IN_T, CL_IN_T, w, ushort, _sat)   \
    CONVERT_KERNEL_FOREACH_OUT_T(N, CL_N, IN_T, CL_IN_T, u, uint,   _sat)   \
    CONVERT_KERNEL_FOREACH_OUT_T(N, CL_N, IN_T, CL_IN_T, f, float,      )   \

#define CONVERT_KERNEL_FOREACH_N(N, CL_N)           \
    CONVERT_KERNEL_FOREACH_IN_T(N, CL_N, b, uchar)  \
    CONVERT_KERNEL_FOREACH_IN_T(N, CL_N, w, ushort) \
    CONVERT_KERNEL_FOREACH_IN_T(N, CL_N, u, uint)   \
    CONVERT_KERNEL_FOREACH_IN_T(N, CL_N, f, float)  \

// ---------------------------------------------------------------------------------------------- //

CONVERT_KERNEL_FOREACH_N(1,  )
CONVERT_KERNEL_FOREACH_N(2, 2)
CONVERT_KERNEL_FOREACH_N(3, 3)
CONVERT_KERNEL_FOREACH_N(4, 4)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
