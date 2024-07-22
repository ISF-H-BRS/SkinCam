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

#include "imagearchive.h"
#include "imagereader.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

ImageReader::ImageReader(const std::string& filename)
{
    ImageArchive archive(filename);
    m_frame.push_back(archive.getDark());
    m_frame.push_back(archive.getChannel0());
    m_frame.push_back(archive.getChannel1());
    m_frame.push_back(archive.getChannel2());
}

// ---------------------------------------------------------------------------------------------- //

ImageReader::~ImageReader()
{
    ImageReader::stop();
}

// ---------------------------------------------------------------------------------------------- //

void ImageReader::start()
{
    if (!m_thread)
    {
        m_frameAvailable = false;
        m_running = true;
        m_nextUpdateTime = Clock::now();
        m_thread = std::make_unique<std::thread>(&ImageReader::updateFrameTask, this);
    }
}

// ---------------------------------------------------------------------------------------------- //

void ImageReader::stop()
{
    if (m_thread)
    {
        m_running = false;
        m_thread->join();

        m_thread = nullptr;
        m_frameAvailable = false;
    }
}

// ---------------------------------------------------------------------------------------------- //

auto ImageReader::getMinRefreshRate() const -> int
{
    return MinimumRefreshRate;
}

// ---------------------------------------------------------------------------------------------- //

auto ImageReader::getMaxRefreshRate() const -> int
{
    return MaximumRefreshRate;
}

// ---------------------------------------------------------------------------------------------- //

void ImageReader::setRefreshRate(int rate)
{
    CLAMP_TO_RANGE(rate, MinimumRefreshRate, MaximumRefreshRate);
    m_refreshRate = rate;
}

// ---------------------------------------------------------------------------------------------- //

auto ImageReader::getRefreshRate() const -> int
{
    return m_refreshRate;
}

// ---------------------------------------------------------------------------------------------- //

void ImageReader::getNextFrame(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                               MemoryBuffer1w* channel1, MemoryBuffer1w* channel2)
{
    ASSERT_NOT_NULL(dark);
    ASSERT_NOT_NULL(channel0);
    ASSERT_NOT_NULL(channel1);
    ASSERT_NOT_NULL(channel2);

    RETURN_IF_NOT(m_running);

    const auto timeout = std::chrono::microseconds(20 * 1'000'000 / m_refreshRate);
    const auto available = [this]{ return m_frameAvailable; };

    std::unique_lock lock(m_mutex);

    if (!m_condition.wait_for(lock, timeout, available))
        throw Exception("Wait for next frame timed out.");

    m_frame.at(0).copyTo(dark);
    m_frame.at(1).copyTo(channel0);
    m_frame.at(2).copyTo(channel1);
    m_frame.at(3).copyTo(channel2);
    m_frameAvailable = false;
}

// ---------------------------------------------------------------------------------------------- //

void ImageReader::updateFrameTask()
{
    while (m_running)
    {
        std::this_thread::sleep_until(m_nextUpdateTime);
        m_nextUpdateTime += std::chrono::microseconds(1'000'000 / m_refreshRate);

        m_mutex.lock();
        m_frameAvailable = true;
        m_mutex.unlock();

        m_condition.notify_one();
    }
}

// ---------------------------------------------------------------------------------------------- //
