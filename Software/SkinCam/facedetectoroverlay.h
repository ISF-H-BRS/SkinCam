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

#include "imageoverlay.h"
#include "rect.h"

#include <QRect>

#include <vector>

SKINCAM_BEGIN_NAMESPACE();

class FaceDetectorOverlay : public ImageOverlay
{
    Q_OBJECT

public:
    FaceDetectorOverlay(int width, int height, ImageWidget* parent);

public slots:
    void setMirrored(bool mirror);

    void setSkinPixelsHighlighted(bool highlight);
    void setSkinMask(const QImage& mask);

    void setRects(const std::vector<Rect>& realRects, const std::vector<Rect>& fakeRects);

    void clear();

private:
    void updateAlphaMask();
    void paintEvent(QPaintEvent* event) override;

private:
    QSize m_imageSize;

    QImage m_skinMask;
    QImage m_alphaMask;

    QPixmap m_alphaPixmap;

    bool m_needAlphaMaskUpdate = true;

    std::vector<QRect> m_realRects;
    std::vector<QRect> m_fakeRects;

    bool m_mirrored = false;
    bool m_highlighted = true;
};

SKINCAM_END_NAMESPACE();
