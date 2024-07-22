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

#include "classifierbackend.h"
#include "global.h"
#include "kernels.h"

SKINCAM_BEGIN_NAMESPACE();

class GaussianThresholdBackend : public ClassifierBackend
{
public:
    GaussianThresholdBackend(SkinClassifier* classifier);

    void update(SkinClassifier* classifier) override;
    void process(const InputBuffer& input, OutputBuffer* output) override;

private:
    GaussianKernel<Vec3f> gaussian;
    ThresholdKernel<Vec1f, Vec1f> threshold;

private:
    OutputBuffer m_buffer = { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };

    Vec3f m_mean;
    Vec3f m_variance;

    float m_threshold = 0.0F;
};

SKINCAM_END_NAMESPACE();
