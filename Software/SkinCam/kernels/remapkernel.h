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

enum class RemapMode
{
    Uniform,
    Separate
};

// ---------------------------------------------------------------------------------------------- //

template <typename T, RemapMode M>
class RemapKernel : public AbstractKernel
{
public:
    using InputBuffer = ComputeBuffer<T>;
    using OutputBuffer = ComputeBuffer<T>;
    using MapBuffer = ComputeBuffer<Vec2f>;

public:
    RemapKernel();

    // Uniform
    void operator()(const InputBuffer& input, OutputBuffer* output, const MapBuffer& map);

    // Separate
    void operator()(const InputBuffer& input, OutputBuffer* output,
                    const MapBuffer& map0, const MapBuffer& map1, const MapBuffer& map2);
};

// ---------------------------------------------------------------------------------------------- //

template <typename T, RemapMode M>
void RemapKernel<T,M>::operator()(const InputBuffer& input, OutputBuffer* output,
                                  const MapBuffer& map)
{
    static_assert(M == RemapMode::Uniform);

    ASSERT(map.pixelCount() == output->pixelCount());

    setArg(0, input);
    setArg(1, output);
    setArg(2, map);
    setArg(3, input.width());
    setArg(4, input.height());

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, RemapMode M>
void RemapKernel<T,M>::operator()(const InputBuffer& input, OutputBuffer* output,
                                  const MapBuffer& map0, const MapBuffer& map1,
                                  const MapBuffer& map2)
{
    static_assert(M == RemapMode::Separate);

    ASSERT(map0.pixelCount() == output->pixelCount());
    ASSERT(map1.pixelCount() == output->pixelCount());
    ASSERT(map2.pixelCount() == output->pixelCount());

    setArg(0, input);
    setArg(1, output);
    setArg(2, map0);
    setArg(3, map1);
    setArg(4, map2);
    setArg(5, input.width());
    setArg(6, input.height());

    enqueue(output->pixelCount());
}

// ---------------------------------------------------------------------------------------------- //

//  foreach (T, CL_T)
//      foreach (N, CL_N)
//          foreach (MODE, CL_MODE)

// ---------------------------------------------------------------------------------------------- //

#define REMAP_KERNEL_SPECIALIZE(TYPE, CL_TYPE, CL_N, MODE, CL_MODE)             \
template <> inline RemapKernel<TYPE, RemapMode::MODE>::RemapKernel()            \
    : AbstractKernel("remap", { "TYPE=" #CL_TYPE, "N=" #CL_N, #CL_MODE }) {}    \

#define REMAP_KERNEL_FOREACH_MODE(T, CL_T, N, CL_N, MODE, CL_MODE)      \
    REMAP_KERNEL_SPECIALIZE(Vec##N##T, CL_T##CL_N, CL_N, MODE, CL_MODE) \

#define REMAP_KERNEL_FOREACH_N(T, CL_T, N, CL_N)                    \
    REMAP_KERNEL_FOREACH_MODE(T, CL_T, N, CL_N, Uniform, UNIFORM)   \
    REMAP_KERNEL_FOREACH_MODE(T, CL_T, N, CL_N, Separate, SEPARATE) \

#define REMAP_KERNEL_FOREACH_T(T, CL_T)     \
    REMAP_KERNEL_FOREACH_N(T, CL_T, 1,  )   \
    REMAP_KERNEL_FOREACH_N(T, CL_T, 2, 2)   \
    REMAP_KERNEL_FOREACH_N(T, CL_T, 3, 3)   \
    REMAP_KERNEL_FOREACH_N(T, CL_T, 4, 4)   \

// ---------------------------------------------------------------------------------------------- //

REMAP_KERNEL_FOREACH_T(b, uchar)
REMAP_KERNEL_FOREACH_T(w, ushort)
REMAP_KERNEL_FOREACH_T(u, uint)
REMAP_KERNEL_FOREACH_T(f, float)

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
