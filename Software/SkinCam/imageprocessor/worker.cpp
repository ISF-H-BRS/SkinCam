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

#include "worker.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;
using namespace std::chrono_literals;

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::Worker::addRawFilter(Filter* filter)
{
    m_rawFilters.push_back(filter->createWorker());
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::Worker::addVisualFilter(Filter* filter)
{
    m_visualFilters.push_back(filter->createWorker());
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::Worker::setSkinClassifier(SkinClassifier* classifier)
{
    m_classifier = classifier->createWorker();
}

// ---------------------------------------------------------------------------------------------- //

auto ImageProcessor::Worker::process(ImageProvider* provider) -> Frame
{
    provider->getNextFrame(&m_rawDark, &m_rawChannel0, &m_rawChannel1, &m_rawChannel2);
    return reprocess();
}

// ---------------------------------------------------------------------------------------------- //

auto ImageProcessor::Worker::reprocess() -> Frame
{
    convertImages(m_rawDark, m_rawChannel0, m_rawChannel1, m_rawChannel2,
                  &m_darkBuffer, &m_rawBuffer0, &m_rawBuffer1, &m_rawBuffer2, &m_rawBuffer);
    runFilters(m_rawBuffer, &m_processedBuffer, m_rawFilters);
    runFilters(m_processedBuffer, &m_visualBuffer, m_visualFilters);
    runClassifier(m_processedBuffer, &m_skinMask, &m_skinRatio, m_classifier);
    convertResults(m_visualBuffer,
                   &m_channelBuffer0, &m_channelBuffer1, &m_channelBuffer2, &m_compositeBuffer);
    return {
        m_darkBuffer,
        m_rawBuffer0,
        m_rawBuffer1,
        m_rawBuffer2,
        m_channelBuffer0,
        m_channelBuffer1,
        m_channelBuffer2,
        m_compositeBuffer,
        m_skinMask,
        m_skinRatio
    };
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::Worker::getProcessedImage(MemoryBuffer3f* image)
{
    image->copyFrom(m_processedBuffer);
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::Worker::getRawImages(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                                          MemoryBuffer1w* channel1, MemoryBuffer1w* channel2) const
{
    m_rawDark.copyTo(dark);
    m_rawChannel0.copyTo(channel0);
    m_rawChannel1.copyTo(channel1);
    m_rawChannel2.copyTo(channel2);
}

// ---------------------------------------------------------------------------------------------- //
