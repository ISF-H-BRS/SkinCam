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

#include "facedetector.h"

#include <QObject>

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

class FaceDetectorWrapper : public QObject
{
    Q_OBJECT

public:
    explicit FaceDetectorWrapper(QObject* parent = nullptr);
    ~FaceDetectorWrapper();

public slots:
    void setSkinValueThreshold(int threshold);
    void setMinimumSkinRatio(int ratio);

    void setNextFrame(const Frame& frame);

signals:
    void resultsAvailable(const std::vector<Rect>& realRects,
                          const std::vector<Rect>& fakeRects);

private slots:
    void handleFaceDetectionFinished(const std::vector<Rect>& realRects,
                                     const std::vector<Rect>& fakeRects);
private:
    class Listener;
    std::unique_ptr<Listener> m_listener;

    std::unique_ptr<FaceDetector> m_faceDetector;
};

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
