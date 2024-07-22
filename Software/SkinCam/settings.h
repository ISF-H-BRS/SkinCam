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

#include "namespace.h"

#include "CoSeSt2/CoSeSt2.h"

#include <QPoint>
#include <QString>

SKINCAM_BEGIN_NAMESPACE();

COSEST_BEGIN(Settings, CoSeSt::StorageType::Local)
  COSEST_BEGIN_GROUP(Setup)
    COSEST_ENTRY(QString, SerialNumber,    QString())
    COSEST_ENTRY(QString, ControllerPort,  QString())
    COSEST_ENTRY(bool,    ServerEnabled,   false)
    COSEST_ENTRY(int,     ServerPort,      5432)
    COSEST_ENTRY(bool,    SaveOnExit,      true)
    COSEST_ENTRY(int,     ThreadCount,     1)
    COSEST_ENTRY(QString, Renderer,        "Software")
  COSEST_END_GROUP(Setup)

  COSEST_BEGIN_GROUP(ColorAdjustment)
    COSEST_ENTRY(bool, Enabled,    false)
    COSEST_ENTRY(int,  Brightness, 0)
    COSEST_ENTRY(int,  Contrast,   0)
  COSEST_END_GROUP(ColorAdjustment)

  COSEST_BEGIN_GROUP(Sharpening)
    COSEST_ENTRY(bool,   Enabled,   false)
    COSEST_ENTRY(int,    Radius,    1)
    COSEST_ENTRY(double, Amount,    6.0)
    COSEST_ENTRY(int,    Threshold, 0)
  COSEST_END_GROUP(Sharpening)

  COSEST_BEGIN_GROUP(Smoothing)
    COSEST_ENTRY(bool, Enabled, false)
    COSEST_ENTRY(int,  Radius,  1)
  COSEST_END_GROUP(Smoothing)

  COSEST_BEGIN_GROUP(Average)
    COSEST_ENTRY(bool, Enabled,     false)
    COSEST_ENTRY(int,  SampleCount, 2)
  COSEST_END_GROUP(Average)

  COSEST_BEGIN_GROUP(Barrel)
    COSEST_ENTRY(bool, Enabled, false)
    COSEST_ENTRY(int,  Factor,  0)
  COSEST_END_GROUP(Barrel)

COSEST_BEGIN_GROUP(Transformation)
    COSEST_ENTRY(bool,   Enabled, false)
    COSEST_ENTRY(double, HScale,  1.0)
    COSEST_ENTRY(double, VScale,  1.0)
    COSEST_ENTRY(int,    HShift,  0)
    COSEST_ENTRY(int,    VShift,  0)
    COSEST_ENTRY(double, RAngle,  0.0)
  COSEST_END_GROUP(Transformation)

  COSEST_BEGIN_GROUP(Crop)
    COSEST_ENTRY(bool, Enabled, false)
    COSEST_ENTRY(int,  Left,    0)
    COSEST_ENTRY(int,  Right,   0)
    COSEST_ENTRY(int,  Top,     0)
    COSEST_ENTRY(int,  Bottom,  0)
  COSEST_END_GROUP(Crop)

  COSEST_BEGIN_GROUP(ImageOutput)
    COSEST_ENTRY(bool, Mirrored, false)
  COSEST_END_GROUP(ImageOutput)

  COSEST_BEGIN_GROUP(Classifier)
    COSEST_ENTRY(int, Method,     0)
    COSEST_ENTRY(int, Scaling,    0)
    COSEST_ENTRY(int, Threshold, 10)
  COSEST_END_GROUP(Classifier)

  COSEST_BEGIN_GROUP(DetectionThresholds)
    COSEST_ENTRY(int, SkinRatio,   50)
  COSEST_END_GROUP(DetectionThresholds)

  COSEST_BEGIN_GROUP(RegionOfInterest)
    COSEST_ENTRY(bool,   Enabled, false)
    COSEST_ENTRY(QPoint, Vertex0, QPoint( 80,  64))
    COSEST_ENTRY(QPoint, Vertex1, QPoint(560,  64))
    COSEST_ENTRY(QPoint, Vertex2, QPoint(560, 448))
    COSEST_ENTRY(QPoint, Vertex3, QPoint( 80, 448))
  COSEST_END_GROUP(RegionOfInterest)

  COSEST_BEGIN_GROUP(FaceDetection)
    COSEST_ENTRY(bool, Enabled,               false)
    COSEST_ENTRY(int,  MinimumSkinRatio,      50)
    COSEST_ENTRY(bool, SkinPixelsHighlighted, true)
  COSEST_END_GROUP(FaceDetection)

  COSEST_BEGIN_GROUP(SkinClassification)
    COSEST_ENTRY(double, Mean0,     0.0)
    COSEST_ENTRY(double, Mean1,     0.0)
    COSEST_ENTRY(double, Mean2,     0.0)
    COSEST_ENTRY(double, Variance0, 0.0)
    COSEST_ENTRY(double, Variance1, 0.0)
    COSEST_ENTRY(double, Variance2, 0.0)
  COSEST_END_GROUP(SkinClassification)

  COSEST_BEGIN_GROUP(WhiteCalibration)
    COSEST_ENTRY(bool,   Enabled, true)
    COSEST_ENTRY(double, Factor0, 1.0)
    COSEST_ENTRY(double, Factor1, 1.0)
    COSEST_ENTRY(double, Factor2, 1.0)
  COSEST_END_GROUP(WhiteCalibration)

  COSEST_BEGIN_GROUP(Acquisition)
    COSEST_ENTRY(int,    FrameRate,        25)
    COSEST_ENTRY(double, IlluminationTime, 10.0)
    COSEST_ENTRY(double, ExposureTime,     10.0)
    COSEST_ENTRY(bool,   TimesLocked,      true)
    COSEST_ENTRY(int,    CameraGain,        0)
  COSEST_END_GROUP(Acquisition)

#ifndef SKINCAM_BUILD_LEGACY
  COSEST_BEGIN_GROUP(LedPower)
    COSEST_ENTRY(int, PowerLevel0, 60)
    COSEST_ENTRY(int, PowerLevel1, 60)
    COSEST_ENTRY(int, PowerLevel2, 60)
  COSEST_END_GROUP(LedPower)
#endif

  COSEST_BEGIN_GROUP(Advanced)
    COSEST_ENTRY(int, TemperatureSetpoint, 0)
  COSEST_END_GROUP(Advanced)
COSEST_END

SKINCAM_END_NAMESPACE();
