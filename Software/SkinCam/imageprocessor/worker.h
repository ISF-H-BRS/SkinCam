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

#include "convertimages.h"
#include "convertresults.h"
#include "kernels.h"
#include "imageprocessor.h"
#include "runfilters.h"
#include "runskinclassifier.h"

SKINCAM_BEGIN_NAMESPACE();

class ImageProcessor::Worker
{
public:
    void addRawFilter(Filter* filter);
    void addVisualFilter(Filter* filter);

    void setSkinClassifier(SkinClassifier* classifier);

    auto process(ImageProvider* provider) -> Frame;
    auto reprocess() -> Frame;

    void getProcessedImage(MemoryBuffer3f* image);
    void getRawImages(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                      MemoryBuffer1w* channel1, MemoryBuffer1w* channel2) const;

private:
    ConvertImages convertImages;
    RunFilters runFilters;
    RunSkinClassifier runClassifier;
    ConvertResults convertResults;

private:
    std::vector<std::unique_ptr<FilterWorker>> m_rawFilters;
    std::vector<std::unique_ptr<FilterWorker>> m_visualFilters;

    std::unique_ptr<SkinClassifierWorker> m_classifier = nullptr;

    MemoryBuffer1w m_rawDark { ImageWidth, ImageHeight };
    MemoryBuffer1w m_rawChannel0 { ImageWidth, ImageHeight };
    MemoryBuffer1w m_rawChannel1 { ImageWidth, ImageHeight };
    MemoryBuffer1w m_rawChannel2 { ImageWidth, ImageHeight };

    ComputeBuffer3f m_rawBuffer       { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };
    ComputeBuffer3f m_processedBuffer { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };
    ComputeBuffer3f m_visualBuffer    { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };

    ComputeBuffer1b m_darkBuffer { ImageWidth, ImageHeight, CL_MEM_WRITE_ONLY };
    ComputeBuffer1b m_rawBuffer0 { ImageWidth, ImageHeight, CL_MEM_WRITE_ONLY };
    ComputeBuffer1b m_rawBuffer1 { ImageWidth, ImageHeight, CL_MEM_WRITE_ONLY };
    ComputeBuffer1b m_rawBuffer2 { ImageWidth, ImageHeight, CL_MEM_WRITE_ONLY };

    ComputeBuffer1b m_channelBuffer0 { ImageWidth, ImageHeight, CL_MEM_WRITE_ONLY };
    ComputeBuffer1b m_channelBuffer1 { ImageWidth, ImageHeight, CL_MEM_WRITE_ONLY };
    ComputeBuffer1b m_channelBuffer2 { ImageWidth, ImageHeight, CL_MEM_WRITE_ONLY };

    ComputeBuffer4b m_compositeBuffer { ImageWidth, ImageHeight, CL_MEM_WRITE_ONLY };

    ComputeBuffer1b m_skinMask { ImageWidth, ImageHeight, CL_MEM_WRITE_ONLY };

    double m_skinRatio = 0.0;
};

SKINCAM_END_NAMESPACE();
