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
#include "facedetectoroverlay.h"

#include <QPainter>

#include <cmath> // for round()
#include <span>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr int SkinMaskAlpha = 192;
}

// ---------------------------------------------------------------------------------------------- //

FaceDetectorOverlay::FaceDetectorOverlay(int width, int height, ImageWidget* parent)
    : ImageOverlay(parent),
      m_imageSize(QSize(width, height)),
      m_skinMask(m_imageSize, QImage::Format_Grayscale8),
      m_alphaMask(m_imageSize, QImage::Format_RGBA8888)
{
    ASSERT_NOT_NULL(parent);

    m_skinMask.fill(QColor(0, 0, 0, 255));
    m_alphaMask.fill(QColor(0, 0, 0, 0));
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorOverlay::setMirrored(bool mirror)
{
    m_mirrored = mirror;
    update();
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorOverlay::setSkinPixelsHighlighted(bool highlight)
{
    m_highlighted = highlight;
    update();
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorOverlay::setSkinMask(const QImage& mask)
{
    ASSERT(mask.size() == m_alphaMask.size());
    ASSERT(mask.format() == QImage::Format_Grayscale8);

    m_skinMask = mask;
    m_needAlphaMaskUpdate = true;

    update();
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorOverlay::setRects(const std::vector<Rect>& realRects,
                                   const std::vector<Rect>& fakeRects)
{
    m_realRects.clear();
    m_fakeRects.clear();

    for (const auto& rect : realRects)
        m_realRects.emplace_back(rect.x(), rect.y(), rect.width(), rect.height());

    for (const auto& rect : fakeRects)
        m_fakeRects.emplace_back(rect.x(), rect.y(), rect.width(), rect.height());

    update();
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorOverlay::clear()
{
    m_realRects.clear();
    m_fakeRects.clear();

    update();
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorOverlay::updateAlphaMask()
{
    static constexpr int Stride = 4; // RGBA format
    static constexpr int Offset = 3; // Last byte

    std::span<const uchar> inData(m_skinMask.constBits(), m_skinMask.sizeInBytes());
    std::span<uchar> outData(m_alphaMask.bits(), m_alphaMask.sizeInBytes());

    const size_t pixelCount = m_skinMask.width() * m_skinMask.height();

    ASSERT(inData.size() == pixelCount);
    ASSERT(outData.size() == 4*pixelCount);

    for (size_t i = 0; i < pixelCount; ++i)
        outData[i*Stride + Offset] = SkinMaskAlpha - (inData[i] * SkinMaskAlpha / 255);

    m_alphaPixmap.convertFromImage(m_alphaMask);
    m_needAlphaMaskUpdate = false;
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorOverlay::paintEvent(QPaintEvent* event)
{
    ImageOverlay::paintEvent(event);

    const QRect& widgetRect = rect();

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (m_mirrored)
    {
        painter.translate(widgetRect.width(), 0.0);
        painter.scale(-1.0, 1.0);
    }

    QSize imageSize = m_imageSize;
    imageSize.scale(widgetRect.size(), Qt::KeepAspectRatio);

    QRect imageRect(QPoint(), imageSize);
    imageRect.moveCenter(widgetRect.center());

    if (m_highlighted)
    {
        if (m_needAlphaMaskUpdate)
            updateAlphaMask();

        painter.drawPixmap(imageRect, m_alphaPixmap);
    }

    const double scale = 1.0 * imageSize.width() / m_imageSize.width();

    const auto paintRect = [&](const QRect& rect)
    {
        const auto x = static_cast<int>(std::round(imageRect.x() + scale * rect.x()));
        const auto y = static_cast<int>(std::round(imageRect.y() + scale * rect.y()));
        const auto w = static_cast<int>(std::round(scale * rect.width()));
        const auto h = static_cast<int>(std::round(scale * rect.height()));

        painter.drawRect(x, y, w, h);
    };

    painter.setPen({ Qt::red, 3.0 });

    for (const auto& rect : m_fakeRects)
        paintRect(rect);

    painter.setPen({ Qt::green, 3.0 });

    for (const auto& rect : m_realRects)
        paintRect(rect);

    emit paintFinished(this);
}

// ---------------------------------------------------------------------------------------------- //
