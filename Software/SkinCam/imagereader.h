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

#include "exception.h"
#include "imageprovider.h"

#include <condition_variable>
#include <mutex>
#include <thread>

SKINCAM_BEGIN_NAMESPACE();

class ImageReader : public ImageProvider
{
public:
    static constexpr int MinimumRefreshRate = 10;
    static constexpr int MaximumRefreshRate = 1000;
    static constexpr int DefaultRefreshRate = 25;

public:
    ImageReader(const std::string& filename);
    ~ImageReader() override;

    void start() override;
    void stop() override;

    auto getMinRefreshRate() const -> int override;
    auto getMaxRefreshRate() const -> int override;

    void setRefreshRate(int rate) override;
    auto getRefreshRate() const -> int override;

    void getNextFrame(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                      MemoryBuffer1w* channel1, MemoryBuffer1w* channel2) override;

private:
    void updateFrameTask();

private:
    std::unique_ptr<std::thread> m_thread;

    std::mutex m_mutex;
    std::condition_variable m_condition;

    using Clock = std::chrono::steady_clock;
    std::chrono::time_point<Clock> m_nextUpdateTime;

    std::vector<MemoryBuffer1w> m_frame;
    bool m_frameAvailable = false;

    int m_refreshRate = DefaultRefreshRate;

    bool m_running = false;
};

SKINCAM_END_NAMESPACE();
