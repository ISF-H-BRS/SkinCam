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

#include "filter.h"
#include "kernels.h"
#include "imageprovider.h"
#include "skinclassifier.h"

#include "imageprocessor/convertimages.h"
#include "imageprocessor/runfilters.h"

SKINCAM_BEGIN_NAMESPACE();

class ImageProcessor
{
public:
    using ChannelBuffer = MemoryBuffer<Vec1w>;
    using CameraBuffer = MemoryBuffer<Vec3w>;

    using WorkingBuffer = ComputeBuffer<Vec3f>;

    using SkinComputeBuffer = ComputeBuffer<Vec1f>;

    using VisualComputeBuffer = ComputeBuffer<Vec1f>;
    using VisualImageBuffer = MemoryBuffer<Vec1b>;

    using ImageBuffer1f = MemoryBuffer<Vec1f>;

    class FrameListener
    {
    protected:
        ~FrameListener() = default;

    private:
        friend class ImageProcessor;
        virtual void onNewFrameAvailable(const Frame& frame) = 0;
        virtual void onError(const char* msg) = 0;
    };

    static constexpr size_t MinimumThreadCount = 1;
    static constexpr size_t MaximumThreadCount = 4;

public:
    explicit ImageProcessor(ImageProvider* provider, FrameListener* listener, size_t threads = 1);
    ~ImageProcessor();

    void addRawFilter(Filter* filter);
    void addVisualFilter(Filter* filter);

    void setSkinClassifier(SkinClassifier* classifier);

    void start();
    void stop();

    void reprocess();

    void getProcessedImage(MemoryBuffer3f* image);
    void getRawImages(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                      MemoryBuffer1w* channel1, MemoryBuffer1w* channel2);

    auto isRunning() const -> bool;

private:
    class Worker;

    struct Thread
    {
        std::unique_ptr<Worker> worker;
        std::thread handle;
    };

private:
    void work(Worker* worker);

private:
    ImageProvider* m_provider;
    FrameListener* m_listener;

    std::vector<Thread> m_threads;
    Worker* m_lastWorker = nullptr;

    std::mutex m_mutex;

    bool m_running = false;
    bool m_error = false;
};

SKINCAM_END_NAMESPACE();
