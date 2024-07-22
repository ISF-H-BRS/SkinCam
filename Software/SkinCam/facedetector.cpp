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

#include "assertions.h"
#include "facedetector.h"
#include "global.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr int DetectorScale = 4;
    constexpr int DetectorWidth = ImageWidth / DetectorScale;
    constexpr int DetectorHeight = ImageHeight / DetectorScale;

    constexpr size_t AveragerBufferSize = 7;

    constexpr int PixelCountStride = DetectorScale * 2;
    constexpr int PixelCountStrideSquared = PixelCountStride * PixelCountStride;
}

// ---------------------------------------------------------------------------------------------- //

FaceDetector::FaceDetector(ResultListener* listener)
    : m_listener(listener),
      m_detector(std::make_unique<ifd::FaceDetector>(DetectorWidth, DetectorHeight))
{
    ASSERT_NOT_NULL(listener);

    m_running = true;
    m_thread = std::thread(&FaceDetector::work, this);
}

// ---------------------------------------------------------------------------------------------- //

FaceDetector::~FaceDetector()
{
    ASSERT(m_thread.joinable());

    m_running = false;
    m_condition.notify_one();
    m_thread.join();
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetector::setSkinValueThreshold(double threshold)
{
    m_skinValueThreshold = static_cast<int>(threshold * 255.0);
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetector::setMinimumSkinRatio(double ratio)
{
    m_minimumSkinRatio = ratio;
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetector::setNextFrame(const Frame& frame)
{
    std::lock_guard lock(m_mutex);

    m_nextFrame = m_framePool.makeUnique(frame);
    m_condition.notify_one();
}

// ---------------------------------------------------------------------------------------------- //

auto FaceDetector::waitForNextFrame() -> FramePtr
{
    const auto condition = [this]{ return m_nextFrame || !m_running; };

    std::unique_lock lock(m_mutex);
    m_condition.wait(lock, condition);

    FramePtr frame;

    if (m_nextFrame)
        frame = std::move(m_nextFrame);

    return frame;
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetector::updateAverages(const ifd::RectList& results,
                                  std::vector<RectAverager>* averages) const
{
    static const auto toRect = [](const ifd::Rect& rect) -> Rect {
        return {
            static_cast<int>(rect.x),
            static_cast<int>(rect.y),
            static_cast<int>(rect.width),
            static_cast<int>(rect.height)
        };
    };

    if (averages->size() != results.size())
    {
        averages->clear();

        for (size_t i = 0; i < results.size(); ++i)
            averages->emplace_back(AveragerBufferSize);
    }

    ASSERT(averages->size() == results.size());

    for (size_t i = 0; i < results.size(); ++i)
    {
        const ifd::Rect& result = results[i];
        averages->at(i).addRect(DetectorScale * toRect(result));
    }
}

// ---------------------------------------------------------------------------------------------- //

auto FaceDetector::isRealFace(const QImage& skinMask, const Rect& rect) const -> bool
{
    const auto data = std::span(skinMask.constBits(), skinMask.sizeInBytes());
    const int bytesPerLine = skinMask.bytesPerLine(); // May contain padding bytes

    const int yFirst = std::max(rect.y(), 0);
    const int yLast = std::min(rect.y() + rect.height(), ImageHeight - 1);

    const int xFirst = std::max(rect.x(), 0);
    const int xLast = std::min(rect.x() + rect.width(), ImageWidth - 1);

    int pixelCount = 0;

    for (int y = yFirst; y < yLast; y += PixelCountStride)
    {
        const auto offset = y * bytesPerLine;

        for (int x = xFirst; x < xLast; x += PixelCountStride)
        {
            ASSERT(offset + x < static_cast<int>(data.size()));

            if (data[offset + x] >= m_skinValueThreshold)
                ++pixelCount;
        }
    }

    const int totalCount = rect.width() * rect.height() / PixelCountStrideSquared;
    const auto ratio = static_cast<double>(pixelCount) / totalCount;

    return ratio >= m_minimumSkinRatio;
};

// ---------------------------------------------------------------------------------------------- //

void FaceDetector::work()
{
    ifd::RectList results;
    std::vector<RectAverager> averages;

    std::vector<Rect> realRects;
    std::vector<Rect> fakeRects;

    while (m_running)
    {
        const auto frame = waitForNextFrame();

        if (!frame)
            continue;

        const QImage& image = frame->channel0Image();

        const QImage scaled = image.scaled(DetectorWidth, DetectorHeight,
                                           Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        const auto data = reinterpret_cast<const ifd::GrayscalePixel*>(scaled.constBits());
        m_detector->process(std::span(data, scaled.width() * scaled.height()), &results);

        updateAverages(results, &averages);

        realRects.clear();
        fakeRects.clear();

        if (!results.empty())
        {
            const QImage& mask = frame->skinMaskImage();

            for (const RectAverager& average : averages)
            {
                const Rect rect = average.getAverage();

                if (isRealFace(mask, rect))
                    realRects.push_back(rect);
                else
                    fakeRects.push_back(rect);
            }
        }

        m_listener->onFaceDetectionFinished(realRects, fakeRects);
    }
}

// ---------------------------------------------------------------------------------------------- //
