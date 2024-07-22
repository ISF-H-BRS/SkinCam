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

#include "frame.h"
#include "memorypool.h"
#include "rectaverager.h"

#include <ifd.h>

#include <condition_variable>
#include <thread>
#include <vector>

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

class FaceDetector
{
public:
    class ResultListener
    {
    protected:
        ~ResultListener() = default;

    private:
        friend class FaceDetector;
        virtual void onFaceDetectionFinished(const std::vector<Rect>& realRects,
                                             const std::vector<Rect>& fakeRects) = 0;
    };

public:
    FaceDetector(ResultListener* listener);
    ~FaceDetector();

    void setSkinValueThreshold(double threshold);
    void setMinimumSkinRatio(double ratio);

    void setNextFrame(const Frame& frame);

private:
    using FramePool = MemoryPool<Frame>;
    using FramePtr = FramePool::UniquePtr;

private:
    auto waitForNextFrame() -> FramePtr;
    void updateAverages(const ifd::RectList& results, std::vector<RectAverager>* averages) const;
    auto isRealFace(const QImage& skinMask, const Rect& rect) const -> bool;

    void work();

private:
    ResultListener* m_listener;

    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_condition;

    int m_skinValueThreshold = 100;
    double m_minimumSkinRatio = 0.5;

    bool m_running = false;

    FramePool m_framePool;
    FramePtr m_nextFrame;

    std::unique_ptr<ifd::FaceDetector> m_detector;
};

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
