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

#include "imageprocessor.h"
#include "imageprocessor/worker.h"

#include <iostream>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

ImageProcessor::ImageProcessor(ImageProvider* provider, FrameListener* listener, size_t threads)
    : m_provider(provider),
      m_listener(listener),
      m_threads(threads)
{
    ASSERT_NOT_NULL(provider);
    ASSERT_NOT_NULL(listener);

    CLAMP_TO_RANGE(threads, MinimumThreadCount, MaximumThreadCount);

    for (auto& thread : m_threads)
        thread.worker = std::make_unique<Worker>();
}

// ---------------------------------------------------------------------------------------------- //

ImageProcessor::~ImageProcessor()
{
    try {
        stop();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::addRawFilter(Filter* filter)
{
    for (auto& thread : m_threads)
        thread.worker->addRawFilter(filter);
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::addVisualFilter(Filter* filter)
{
    for (auto& thread : m_threads)
        thread.worker->addVisualFilter(filter);
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::setSkinClassifier(SkinClassifier* classifier)
{
    for (auto& thread : m_threads)
        thread.worker->setSkinClassifier(classifier);
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::start()
{
    if (m_running)
        return;

    m_provider->start();

    m_running = true;
    m_error = false;

    for (auto& thread : m_threads)
        thread.handle = std::thread(&ImageProcessor::work, this, thread.worker.get());
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::stop()
{
    if (!m_running)
        return;

    m_running = false;

    for (auto& thread : m_threads)
        thread.handle.join();

    m_provider->stop();
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::reprocess()
{
    if (m_running || !m_lastWorker)
        return;

    Frame frame = m_lastWorker->reprocess();
    m_listener->onNewFrameAvailable(frame);
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::getProcessedImage(MemoryBuffer3f* image)
{
    ASSERT_NOT_NULL(image);
    RETURN_IF_NULL(m_lastWorker);

    const bool running = m_running;

    if (running)
        stop();

    m_lastWorker->reprocess();
    m_lastWorker->getProcessedImage(image);

    if (running)
        start();
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::getRawImages(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                                  MemoryBuffer1w* channel1, MemoryBuffer1w* channel2)
{
    ASSERT_NOT_NULL(dark);
    ASSERT_NOT_NULL(channel0);
    ASSERT_NOT_NULL(channel1);
    ASSERT_NOT_NULL(channel2);
    RETURN_IF_NULL(m_lastWorker);

    const bool running = m_running;

    if (running)
        stop();

    m_lastWorker->getRawImages(dark, channel0, channel1, channel2);

    if (running)
        start();
}

// ---------------------------------------------------------------------------------------------- //

auto ImageProcessor::isRunning() const -> bool
{
    return m_running;
}

// ---------------------------------------------------------------------------------------------- //

void ImageProcessor::work(Worker* worker)
{
    // This is the worker method running concurrently in multiple threads.
    // It must serialize all calls to the listener and avoid calling if the
    // processor has already been stopped by the user or due to an error.

    const auto working = [this]{ return m_running && !m_error; };

    while (working())
    {
        try {
            Frame frame = worker->process(m_provider);

            std::lock_guard lock(m_mutex);
            m_lastWorker = worker;

            if (working())
                m_listener->onNewFrameAvailable(frame);
        }
        catch (const std::exception& e)
        {
            std::lock_guard lock(m_mutex);

            if (working())
            {
                m_error = true;
                m_listener->onError(e.what());
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------- //
