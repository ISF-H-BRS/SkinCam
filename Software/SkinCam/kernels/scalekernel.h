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
class ScaleKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<T>;

public:
    ScaleKernel();

    void operator()(const InputBuffer& input, OutputBuffer* output, const T& scale);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void ScaleKernel<T>::operator()(const InputBuffer& input, OutputBuffer* output, const T& scale)
{
    ASSERT(output->pixelCount() == input.pixelCount());

    setArg(0, input);
    setArg(1, output);
    setArg(2, scale);

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (N, CL_N)

// ---------------------------------------------------------------------------------------------- //

#define SCALE_KERNEL_SPECIALIZE(TYPE, CL_TYPE)          \
template <> inline ScaleKernel<TYPE>::ScaleKernel()     \
    : AbstractKernel("scale", { "TYPE=" #CL_TYPE }) {}  \

#define SCALE_KERNEL_FOREACH_N(N, CL_N)             \
    SCALE_KERNEL_SPECIALIZE(Vec##N##f, float##CL_N) \

// ---------------------------------------------------------------------------------------------- //

SCALE_KERNEL_FOREACH_N(1,  )
SCALE_KERNEL_FOREACH_N(2, 2)
SCALE_KERNEL_FOREACH_N(3, 3)
SCALE_KERNEL_FOREACH_N(4, 4)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
