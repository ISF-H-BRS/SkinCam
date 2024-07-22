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

class CropGenKernel : public AbstractKernel
{
public:
    using OutputBuffer = ComputeBuffer2f;

public:
    CropGenKernel();

    void operator()(OutputBuffer* output, float scale,
                    float left, float top, float hBorder, float vBorder);
};

// ---------------------------------------------------------------------------------------------- //

inline
CropGenKernel::CropGenKernel()
    : AbstractKernel("cropgen") {}

// ---------------------------------------------------------------------------------------------- //

inline
void CropGenKernel::operator()(OutputBuffer* output, float scale,
                               float left, float top, float hBorder, float vBorder)
{
    setArg(0, output);
    setArg(1, scale);
    setArg(2, left);
    setArg(3, top);
    setArg(4, hBorder);
    setArg(5, vBorder);

    enqueue(output->width(), output->height());
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
