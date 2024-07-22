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
#include "namespace.h"
#include "settings.h"

#include <QWidget>

namespace Ui {
    class AcquisitionWidget;
}

SKINCAM_BEGIN_NAMESPACE();

constexpr auto millisecondsToTime(double ms) -> std::chrono::microseconds {
    return std::chrono::microseconds(static_cast<long>(ms * 1000.0));
}

constexpr auto timeToMilliseconds(std::chrono::microseconds time) -> double {
    return static_cast<double>(time.count()) / 1000.0;
}

class AcquisitionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AcquisitionWidget(QWidget* parent = nullptr);
    ~AcquisitionWidget() override;

    void applySettings(const Settings::AcquisitionGroup& settings);
    void storeSettings(Settings::AcquisitionGroup* settings);

    void setMinMaxFrameRate(int minimum, int maximum);

    auto getFrameRate() const -> int;
    auto getIlluminationTime() const -> std::chrono::microseconds;
    auto getExposureTime() const -> std::chrono::microseconds;
    auto getCameraGain() const -> Camera::Gain;

signals:
    void frameRateChanged(int rate);
    void illuminationTimeChanged(std::chrono::microseconds us);
    void exposureTimeChanged(std::chrono::microseconds us);
    void cameraGainChanged(Camera::Gain gain);

private slots:
    void handleFrameRateChange(int value);
    void handleIlluminationTimeChange(double value);
    void handleExposureTimeChange(double value);
    void handleLockButtonClicked();
    void handleCameraGainChange(int index);

    void resetValues();

private:
    void setTimesLocked(bool lock);

    static constexpr auto MinimumFrameRate = CameraSystem::MinimumRefreshRate;
    static constexpr auto MaximumFrameRate = CameraSystem::MaximumRefreshRate;

    static constexpr double MinimumIlluminationTime =
                                        timeToMilliseconds(CameraSystem::MinimumIlluminationTime);
    static constexpr double MaximumIlluminationTime =
                                        timeToMilliseconds(CameraSystem::MaximumIlluminationTime);

    static constexpr double MinimumExposureTime = timeToMilliseconds(Camera::MinimumExposureTime);
    static constexpr double MaximumExposureTime = timeToMilliseconds(Camera::MaximumExposureTime);

private:
    std::unique_ptr<Ui::AcquisitionWidget> m_ui;
    bool m_timesLocked = false;
};

SKINCAM_END_NAMESPACE();
