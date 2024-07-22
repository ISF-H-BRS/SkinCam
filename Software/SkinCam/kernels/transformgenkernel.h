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
#include "matrix3.h"

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

class TransformGenKernel : public AbstractKernel
{
public:
    using OutputBuffer = ComputeBuffer2f;

public:
    TransformGenKernel();

    void operator()(OutputBuffer* output, const Matrix3f& transform);
};

// ---------------------------------------------------------------------------------------------- //

inline
TransformGenKernel::TransformGenKernel()
    : AbstractKernel("transformgen") {}

// ---------------------------------------------------------------------------------------------- //

inline
void TransformGenKernel::operator()(OutputBuffer* output, const Matrix3f& transform)
{
    const auto halfWidth = static_cast<float>(output->width() / 2);
    const auto halfHeight = static_cast<float>(output->height() / 2);

    Matrix3f matrix = Matrix3f::translate(halfWidth, halfHeight) * transform
                    * Matrix3f::translate(-halfWidth, -halfHeight);
    matrix.invert();

    setArg(0, output);

    // Homogenous data unnecessary
    for (uint i = 0; i < 6; ++i)
        setArg(i+1, matrix.at(i));

    enqueue(output->width(), output->height());
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
