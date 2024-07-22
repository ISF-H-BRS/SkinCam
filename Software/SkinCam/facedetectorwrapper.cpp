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

#include "facedetectorwrapper.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class FaceDetectorWrapper::Listener : public FaceDetector::ResultListener
{
public:
    Listener(FaceDetectorWrapper* owner);

    void onFaceDetectionFinished(const std::vector<Rect>& realRects,
                                 const std::vector<Rect>& fakeRects) override;

private:
    FaceDetectorWrapper* m_owner;
};

// ---------------------------------------------------------------------------------------------- //

FaceDetectorWrapper::FaceDetectorWrapper(QObject* parent)
    : QObject(parent),
      m_listener(std::make_unique<Listener>(this)),
      m_faceDetector(std::make_unique<FaceDetector>(m_listener.get()))
{
}

// ---------------------------------------------------------------------------------------------- //

FaceDetectorWrapper::~FaceDetectorWrapper() = default;

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorWrapper::setSkinValueThreshold(int threshold)
{
    m_faceDetector->setSkinValueThreshold(threshold * 0.01);
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorWrapper::setMinimumSkinRatio(int ratio)
{
    m_faceDetector->setMinimumSkinRatio(ratio * 0.01);
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorWrapper::setNextFrame(const Frame& frame)
{
    m_faceDetector->setNextFrame(frame);
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorWrapper::handleFaceDetectionFinished(const std::vector<Rect>& realRects,
                                                      const std::vector<Rect>& fakeRects)
{
    emit resultsAvailable(realRects, fakeRects);
}

// ---------------------------------------------------------------------------------------------- //

FaceDetectorWrapper::Listener::Listener(FaceDetectorWrapper* owner)
    : m_owner(owner) {}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectorWrapper::Listener::onFaceDetectionFinished(const std::vector<Rect>& realRects,
                                                            const std::vector<Rect>& fakeRects)
{
    QMetaObject::invokeMethod(m_owner, "handleFaceDetectionFinished", Qt::QueuedConnection,
                              Q_ARG(std::vector<Rect>, realRects),
                              Q_ARG(std::vector<Rect>, fakeRects));
}

// ---------------------------------------------------------------------------------------------- //
