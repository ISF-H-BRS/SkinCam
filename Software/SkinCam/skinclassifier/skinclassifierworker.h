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

#include "boundingboxbackend.h"
#include "computebuffer.h"
#include "countskinpixels.h"
#include "gaussianbackend.h"
#include "gaussianthresholdbackend.h"
#include "global.h"
#include "kernels.h"

#include <memory>
#include <mutex>

SKINCAM_BEGIN_NAMESPACE();

class SkinClassifierWorker
{
public:
    using InputBuffer = ClassifierBackend::InputBuffer;
    using OutputBuffer = ClassifierBackend::OutputBuffer;

public:
    SkinClassifierWorker(SkinClassifier* classifier);
    ~SkinClassifierWorker();

    void update();
    void run(const InputBuffer& input, OutputBuffer* output, double* skinRatio);

private:
    NormDiffKernel<Vec3f> normDiff;
    RoiGenKernel generateMask;

    CountSkinPixels countSkinPixels;
    SummatePixels summatePixels;

private:
    void updateRegionOfInterest();

private:
    SkinClassifier* m_classifier;

    std::unique_ptr<BoundingBoxBackend> m_boundingBoxBackend;
    std::unique_ptr<GaussianBackend> m_gaussianBackend;
    std::unique_ptr<GaussianThresholdBackend> m_gaussianThresholdBackend;

    ClassifierBackend* m_currentBackend = nullptr;

    std::vector<Vec2i> m_regionOfInterest;
    ComputeBuffer2i m_roiPolygon { 1, 1, CL_MEM_READ_ONLY };
    ComputeBuffer1b m_roiMask { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };
    uint m_roiArea = ImageArea;

    ComputeBuffer3f m_normalizedDifferences { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };
    float m_threshold = 0.0F;

    std::mutex m_mutex;
};

SKINCAM_END_NAMESPACE();
