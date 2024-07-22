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

#include "fpscounter.h"
#include "imagearchive.h"
#include "selectionprocessor.h"
#include "skincamsystem.h"

#include <QCoreApplication>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class SkinCamSystem::FrameListener : public ImageProcessor::FrameListener
{
public:
    FrameListener(SkinCamSystem* owner);

    void reset();

    auto fpsProcessed() const -> int;
    auto fpsDisplayed() const -> int;

    void setReady();

private:
    void onNewFrameAvailable(const Frame& frame) override;
    void onError(const char* msg) override;

private:
    SkinCamSystem* m_owner;

    FpsCounter m_processedCounter;
    FpsCounter m_displayedCounter;

    int m_fpsProcessed = 0;
    int m_fpsDisplayed = 0;

    bool m_ready = true;
};

// ---------------------------------------------------------------------------------------------- //

SkinCamSystem::SkinCamSystem()
    : m_skinClassifier(SkinClassifier::Method::Gaussian),
      m_frameListener(std::make_unique<FrameListener>(this))
{
    m_statusTimer.setInterval(1000);
    m_statusTimer.setSingleShot(false);
    connect(&m_statusTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
}

// ---------------------------------------------------------------------------------------------- //

SkinCamSystem::~SkinCamSystem() = default;

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setProcessorThreadCount(size_t threads)
{
    CLAMP_TO_RANGE(threads, ImageProcessor::MinimumThreadCount, ImageProcessor::MaximumThreadCount);
    m_processorThreads = threads;
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::openCamera(const CameraSystem::Setup& setup)
{
    RETURN_IF_NOT_NULL(m_cameraSystem);
    RETURN_IF_NOT_NULL(m_imageReader);

    auto future = std::async(std::launch::async,
                             [&]{ m_cameraSystem = std::make_unique<CameraSystem>(setup); });

    while (future.wait_for(20ms) != std::future_status::ready)
        QCoreApplication::processEvents();

    future.get();

    updateStatus();
    m_statusTimer.start();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::openImage(const QString& filename, int refreshRate)
{
    RETURN_IF_NOT_NULL(m_cameraSystem);
    RETURN_IF_NOT_NULL(m_imageReader);

    m_imageReader = std::make_unique<ImageReader>(filename.toStdString());
    m_imageReader->setRefreshRate(refreshRate);
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setupImageProcessor()
{
    RETURN_IF_NOT_NULL(m_processor);

    ImageProvider* provider = nullptr;

    if (m_cameraSystem)
        provider = m_cameraSystem.get();
    else if (m_imageReader)
        provider = m_imageReader.get();

    ASSERT_NOT_NULL(provider);

    const auto setup = [&]
    {
        m_processor = std::make_unique<ImageProcessor>(provider, m_frameListener.get(),
                                                       m_processorThreads);
        m_processor->addRawFilter(&m_barrelFilter);
        m_processor->addRawFilter(&m_transformationFilter);
        m_processor->addRawFilter(&m_cropFilter);
        m_processor->addRawFilter(&m_averageFilter);
        m_processor->addRawFilter(&m_smoothingFilter);
        m_processor->addRawFilter(&m_sharpeningFilter);
        m_processor->addRawFilter(&m_whiteCalibrationFilter);

        m_processor->addVisualFilter(&m_colorAdjustFilter);
        m_processor->setSkinClassifier(&m_skinClassifier);
    };

    auto future = std::async(std::launch::async, [&]{ setup(); });

    while (future.wait_for(20ms) != std::future_status::ready)
        QCoreApplication::processEvents();

    future.get();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::saveImage(const QString& filename)
{
    RETURN_IF_NULL(m_processor);

    MemoryBuffer1w dark(ImageWidth, ImageHeight);
    MemoryBuffer1w channel0(ImageWidth, ImageHeight);
    MemoryBuffer1w channel1(ImageWidth, ImageHeight);
    MemoryBuffer1w channel2(ImageWidth, ImageHeight);
    m_processor->getRawImages(&dark, &channel0, &channel1, &channel2);

    ImageArchive archive(dark, channel0, channel1, channel2);
    archive.save(filename.toStdString());
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::close()
{
    m_statusTimer.stop();

    m_processor = nullptr;
    m_cameraSystem = nullptr;
    m_imageReader = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setReprocessBlocked(bool block)
{
    m_reprocessBlocked = block;
}

// ---------------------------------------------------------------------------------------------- //

auto SkinCamSystem::processWhiteSelection(const std::vector<Polygon>& selection) -> Vec3f
{
    ASSERT_NOT_NULL(m_processor);

    const bool running = m_processor->isRunning();

    if (running)
        m_processor->stop();

    const bool calibrationEnabled = m_whiteCalibrationFilter.isEnabled();

    if (calibrationEnabled)
        m_whiteCalibrationFilter.setEnabled(false);

    MemoryBuffer3f image(ImageWidth, ImageHeight);
    m_processor->getProcessedImage(&image);

    if (calibrationEnabled)
        m_whiteCalibrationFilter.setEnabled(true);

    if (running)
        m_processor->start();

    return SelectionProcessor::getAverage(image, selection);
}

// ---------------------------------------------------------------------------------------------- //

auto SkinCamSystem::processSkinSelection(const std::vector<Polygon>& selection)
                                                                        -> std::tuple<Vec3f,Vec3f>
{
    ASSERT_NOT_NULL(m_processor);

    MemoryBuffer3f image(ImageWidth, ImageHeight);
    m_processor->getProcessedImage(&image);

    return SelectionProcessor::getNormDiffMeanAndVariance(image, selection);
}

// ---------------------------------------------------------------------------------------------- //

auto SkinCamSystem::processPixelSelection(const std::vector<Polygon>& selection,
                                          const PixelExport::Selection& fields) -> PixelExport::Data
{
    ASSERT_NOT_NULL(m_processor);

    MemoryBuffer3f image(ImageWidth, ImageHeight);
    m_processor->getProcessedImage(&image);

    return SelectionProcessor::get(image, selection, fields);
}

// ---------------------------------------------------------------------------------------------- //

auto SkinCamSystem::fpsProcessed() const -> int
{
    return m_frameListener->fpsProcessed();
}

// ---------------------------------------------------------------------------------------------- //

auto SkinCamSystem::fpsDisplayed() const -> int
{
    return m_frameListener->fpsDisplayed();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::start()
{
    RETURN_IF_NULL(m_processor);

    tryOrEmitError([&]{
        m_frameListener->reset();
        m_processor->start();
    });
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::stop()
{
    RETURN_IF_NULL(m_processor);

    tryOrEmitError([&]{
        m_processor->stop();
    });
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::reprocessIfPaused()
{
    tryOrEmitError([&]{
        if (m_processor != nullptr && !m_processor->isRunning() && !m_reprocessBlocked)
            m_processor->reprocess();
    });
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setColorAdjustmentEnabled(bool enable)
{
    m_colorAdjustFilter.setEnabled(enable);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateColorAdjustment(int brightness, int contrast)
{
    m_colorAdjustFilter.setParameters(brightness, contrast);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setSmoothingEnabled(bool enable)
{
    m_smoothingFilter.setEnabled(enable);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateSmoothing(uint radius)
{
    m_smoothingFilter.setRadius(radius);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setSharpeningEnabled(bool enable)
{
    m_sharpeningFilter.setEnabled(enable);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateSharpening(uint radius, float amount, float threshold)
{
    m_sharpeningFilter.setParameters(radius, amount, threshold);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setAveragingEnabled(bool enable)
{
    m_averageFilter.setEnabled(enable);
    // Don't reprocess if paused
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateAveraging(int sampleCount)
{
    m_averageFilter.setSampleCount(sampleCount);
    // Ditto
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setBarrelEnabled(bool enable)
{
    m_barrelFilter.setEnabled(enable);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateBarrel(int factor)
{
    m_barrelFilter.setFactor(factor);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setTransformationEnabled(bool enable)
{
    m_transformationFilter.setEnabled(enable);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateTransformation(const Matrix3f& matrix)
{
    m_transformationFilter.setMatrix(matrix);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setCropEnabled(bool enable)
{
    m_cropFilter.setEnabled(enable);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateCrop(int left, int right, int top, int bottom)
{
    m_cropFilter.setBorders(left, right, top, bottom);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setWhiteCalibrationEnabled(bool enable)
{
    m_whiteCalibrationFilter.setEnabled(enable);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateWhiteCalibration(double factor0, double factor1, double factor2)
{
    m_whiteCalibrationFilter.setFactors(factor0, factor1, factor2);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::setClassifier(SkinClassifier::Method method)
{
    m_skinClassifier.setMethod(method);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateClassifierScaling(int scaling)
{
    m_skinClassifier.setScaling(scaling);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateClassifierThreshold(int threshold)
{
    m_skinClassifier.setThreshold(threshold * 0.01F);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateClassifierParameters(const Vec3f& mean, const Vec3f& variance)
{
    m_skinClassifier.setParameters(mean, variance);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateRegionOfInterest(const std::vector<Vec2i>& vertices)
{
    m_skinClassifier.setRegionOfInterest(vertices);
    reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateTargetFrameRate(int value)
{
    tryOrEmitError([&]{
        if (m_cameraSystem)
            m_cameraSystem->setRefreshRate(value);

        if (m_imageReader)
            m_imageReader->setRefreshRate(value);
    });
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateIlluminationTime(std::chrono::microseconds us)
{
    tryOrEmitError([&]{
        if (m_cameraSystem)
            m_cameraSystem->setIlluminationTime(us);
    });
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateExposureTime(std::chrono::microseconds us)
{
    tryOrEmitError([&]{
        if (m_cameraSystem)
            m_cameraSystem->setExposureTime(us);
    });
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateGain(Camera::Gain gain)
{
    tryOrEmitError([&]{
        if (m_cameraSystem)
            m_cameraSystem->setGain(gain);
    });
}

// ---------------------------------------------------------------------------------------------- //

#ifdef SKINCAM_BUILD_LEGACY
void SkinCamSystem::updatePowerLevels(int, int, int) {}
#else
void SkinCamSystem::updatePowerLevels(int level0, int level1, int level2)
{
    tryOrEmitError([&]{
        if (m_cameraSystem)
            m_cameraSystem->setPowerLevels(level0, level1, level2);
    });
}
#endif

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateTemperatureSetpoint(Camera::TemperatureSetpoint setpoint)
{
    tryOrEmitError([&]{
        if (m_cameraSystem)
            m_cameraSystem->setTemperatureSetpoint(setpoint);
    });
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::handleNewFrame(const Frame& frame)
{
    m_frameListener->setReady();

    if (!m_processor) // Ignore frames queued before closing
        return;

    emit newFrameAvailable(frame);
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::handleError(const QString& msg)
{
    close();
    emit error(msg);
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::updateStatus()
{
    ASSERT_NOT_NULL(m_cameraSystem);

    tryOrEmitError([&]{
        Camera::Status status = m_cameraSystem->getCameraStatus();

#ifndef SKINCAM_BUILD_LEGACY
        Controller::PowerStatus power = m_cameraSystem->getPowerStatus();
#endif

        emit newCameraStatusAvailable(status);

#ifndef SKINCAM_BUILD_LEGACY
        emit newPowerStatusAvailable(power);
#endif
    });
}

// ---------------------------------------------------------------------------------------------- //

inline
void SkinCamSystem::tryOrEmitError(const std::function<void()>& func)
{
    try {
        func();
    }
    catch (const std::exception& e) {
        emit error(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

SkinCamSystem::FrameListener::FrameListener(SkinCamSystem* owner)
    : m_owner(owner) {}

// ---------------------------------------------------------------------------------------------- //

inline
void SkinCamSystem::FrameListener::reset()
{
    m_processedCounter.reset();
    m_displayedCounter.reset();

    m_fpsProcessed = 0;
    m_fpsDisplayed = 0;
}

// ---------------------------------------------------------------------------------------------- //

inline
auto SkinCamSystem::FrameListener::fpsProcessed() const -> int
{
    return m_fpsProcessed;
}

// ---------------------------------------------------------------------------------------------- //

inline
auto SkinCamSystem::FrameListener::fpsDisplayed() const -> int
{
    return m_fpsDisplayed;
}

// ---------------------------------------------------------------------------------------------- //

inline
void SkinCamSystem::FrameListener::setReady()
{
    m_ready = true;
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::FrameListener::onNewFrameAvailable(const Frame& frame)
{
    m_fpsProcessed = m_processedCounter.update();

    if (m_ready)
    {
        m_ready = false;
        m_fpsDisplayed = m_displayedCounter.update();

        QMetaObject::invokeMethod(m_owner, "handleNewFrame",
                                  Qt::QueuedConnection, Q_ARG(Frame, frame));
    }
}

// ---------------------------------------------------------------------------------------------- //

void SkinCamSystem::FrameListener::onError(const char* msg)
{
    QMetaObject::invokeMethod(m_owner, "handleError",
                              Qt::QueuedConnection, Q_ARG(QString, msg));
}

// ---------------------------------------------------------------------------------------------- //
