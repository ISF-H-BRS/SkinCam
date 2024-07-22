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

#include "camerasystem.h"
#include "filters.h"
#include "fpscounter.h"
#include "imagereader.h"
#include "imageprocessor.h"
#include "pixelexport.h"
#include "polygon.h"
#include "skinclassifier.h"

#include <QObject>
#include <QTimer>

SKINCAM_BEGIN_NAMESPACE();

class SkinCamSystem : public QObject
{
    Q_OBJECT

public:
    SkinCamSystem();
    ~SkinCamSystem();

    void setProcessorThreadCount(size_t threads);

    void openCamera(const CameraSystem::Setup& setup);
    void openImage(const QString& filename, int refreshRate = 10);

    void setupImageProcessor();

    void saveImage(const QString& filename);
    void close();

    void setReprocessBlocked(bool block);

    auto processWhiteSelection(const std::vector<Polygon>& selection) -> Vec3f;
    auto processSkinSelection(const std::vector<Polygon>& selection) -> std::tuple<Vec3f,Vec3f>;
    auto processPixelSelection(const std::vector<Polygon>& selection,
                               const PixelExport::Selection& fields) -> PixelExport::Data;

    auto fpsProcessed() const -> int;
    auto fpsDisplayed() const -> int;

public slots:
    void start();
    void stop();

    void reprocessIfPaused();

    void setColorAdjustmentEnabled(bool enable);
    void updateColorAdjustment(int brightness, int contrast);

    void setSmoothingEnabled(bool enable);
    void updateSmoothing(uint radius);

    void setSharpeningEnabled(bool enable);
    void updateSharpening(uint radius, float amount, float threshold);

    void setAveragingEnabled(bool enable);
    void updateAveraging(int sampleCount);

    void setBarrelEnabled(bool enable);
    void updateBarrel(int factor);

    void setTransformationEnabled(bool enable);
    void updateTransformation(const Matrix3f& matrix);

    void setCropEnabled(bool enable);
    void updateCrop(int left, int right, int top, int bottom);

    void setWhiteCalibrationEnabled(bool enable);
    void updateWhiteCalibration(double factor0, double factor1, double factor2);

    void setClassifier(SkinClassifier::Method method);

    void updateClassifierScaling(int scaling);
    void updateClassifierThreshold(int threshold);
    void updateClassifierParameters(const Vec3f& mean, const Vec3f& variance);

    void updateRegionOfInterest(const std::vector<Vec2i>& vertices);

    void updateTargetFrameRate(int value);
    void updateIlluminationTime(std::chrono::microseconds us);
    void updateExposureTime(std::chrono::microseconds us);
    void updateGain(Camera::Gain gain);

    void updatePowerLevels(int level0, int level1, int level2);

    void updateTemperatureSetpoint(Camera::TemperatureSetpoint setpoint);

signals:
    void newFrameAvailable(const Frame& frame);
    void newCameraStatusAvailable(const Camera::Status& status);

#ifndef SKINCAM_BUILD_LEGACY
    void newPowerStatusAvailable(const Controller::PowerStatus& status);
#endif

    void error(const QString& msg);

private slots:
    void handleNewFrame(const Frame& frame);
    void handleError(const QString& msg);

    void updateStatus();

private:
    void tryOrEmitError(const std::function<void()>& func);

private:
    WhiteCalibrationFilter m_whiteCalibrationFilter;
    ColorAdjustFilter m_colorAdjustFilter;
    SmoothingFilter m_smoothingFilter;
    SharpeningFilter m_sharpeningFilter;
    AverageFilter m_averageFilter;
    BarrelFilter m_barrelFilter;
    TransformationFilter m_transformationFilter;
    CropFilter m_cropFilter;

    SkinClassifier m_skinClassifier;

    class FrameListener;
    std::unique_ptr<FrameListener> m_frameListener;

    std::unique_ptr<CameraSystem> m_cameraSystem;
    std::unique_ptr<ImageReader> m_imageReader;
    std::unique_ptr<ImageProcessor> m_processor;

    size_t m_processorThreads = 1;

    QTimer m_statusTimer;

    bool m_reprocessBlocked = false;
};

SKINCAM_END_NAMESPACE();
