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

template <typename T, size_t N, typename E = std::enable_if_t<N >= 2 && N <= 4>>
class AverageKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<T>;

public:
    AverageKernel();

    template <class InputIt>
    void operator()(InputIt first, OutputBuffer* output);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
template <class InputIt>
void AverageKernel<T,N,E>::operator()(InputIt first, OutputBuffer* output)
{
    for (size_t i = 0; i < N; ++i)
    {
        const InputBuffer& input = *first++;
        ASSERT(input.pixelCount() == output->pixelCount());

        setArg(i, input);
    }

    setArg(N, output);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (T, CL_T)
//      foreach (N, CL_N)
//          foreach (COUNT)

// ---------------------------------------------------------------------------------------------- //

#define AVERAGE_KERNEL_SPECIALIZE(T, CL_T, N, CL_N, COUNT)                      \
template <> inline AverageKernel<Vec##N##T,COUNT>::AverageKernel()              \
    : AbstractKernel("average", { "T=" #CL_T, "N=" #CL_N, "COUNT=" #COUNT }) {} \

#define AVERAGE_KERNEL_FOREACH_COUNT(T, CL_T, N, CL_N, COUNT)   \
    AVERAGE_KERNEL_SPECIALIZE(T, CL_T, N, CL_N, COUNT)          \

#define AVERAGE_KERNEL_FOREACH_N(T, CL_T, N, CL_N)      \
    AVERAGE_KERNEL_FOREACH_COUNT(T, CL_T, N, CL_N, 2)   \
    AVERAGE_KERNEL_FOREACH_COUNT(T, CL_T, N, CL_N, 3)   \
    AVERAGE_KERNEL_FOREACH_COUNT(T, CL_T, N, CL_N, 4)   \

#define AVERAGE_KERNEL_FOREACH_T(T, CL_T)   \
    AVERAGE_KERNEL_FOREACH_N(T, CL_T, 1, )  \
    AVERAGE_KERNEL_FOREACH_N(T, CL_T, 2, 2) \
    AVERAGE_KERNEL_FOREACH_N(T, CL_T, 3, 3) \
    AVERAGE_KERNEL_FOREACH_N(T, CL_T, 4, 4) \

// ---------------------------------------------------------------------------------------------- //

AVERAGE_KERNEL_FOREACH_T(b, uchar)
AVERAGE_KERNEL_FOREACH_T(w, ushort)
AVERAGE_KERNEL_FOREACH_T(u, uint)
AVERAGE_KERNEL_FOREACH_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
