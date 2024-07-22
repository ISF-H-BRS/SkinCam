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

#include "camera.h"
#include "cameraexception.h"
#include "controller.h"
#include "dummycamera.h"
#include "global.h"
#include "imageprovider.h"

SKINCAM_BEGIN_NAMESPACE();

class CameraSystem : public ImageProvider
{
public:
    static constexpr int MinimumRefreshRate =
            1'000'000 / Controller::MaximumSequenceDuration.count();

    static constexpr int MaximumRefreshRate =
            1'000'000 / Controller::MinimumSequenceDuration.count();

    static constexpr int DefaultRefreshRate =
            1'000'000 / Controller::DefaultSequenceDuration.count();

    static constexpr auto MinimumIlluminationTime = Controller::MinimumIlluminationTime;
    static constexpr auto MaximumIlluminationTime = Controller::MaximumIlluminationTime;
    static constexpr auto DefaultIlluminationTime = Controller::DefaultIlluminationTime;

#ifndef SKINCAM_BUILD_LEGACY
    static constexpr auto DefaultPowerLevel = Controller::DefaultPowerLevel;
#endif

    struct Setup
    {
        Camera::Setup cameraSetup;

        int refreshRate = DefaultRefreshRate;
        std::chrono::microseconds illuminationTime = DefaultIlluminationTime;

#ifdef SKINCAM_BUILD_LEGACY
        std::string controllerPort;
#else
        int powerLevel0 = DefaultPowerLevel;
        int powerLevel1 = DefaultPowerLevel;
        int powerLevel2 = DefaultPowerLevel;
#endif
    };

public:
    CameraSystem(const Setup& setup);

    void start() override;
    void stop() override;

    auto getMinRefreshRate() const -> int override;
    auto getMaxRefreshRate() const -> int override;

    void setRefreshRate(int rate) override;
    auto getRefreshRate() const -> int override;

    void getNextFrame(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                      MemoryBuffer1w* channel1, MemoryBuffer1w* channel2) override;

    void setIlluminationTime(std::chrono::microseconds us);
    auto getIlluminationTime() const -> std::chrono::microseconds;

#ifndef SKINCAM_BUILD_LEGACY
    void setPowerLevels(int level0, int level1, int level2);
    auto getPowerLevels() const -> std::tuple<int,int,int>;

    auto getPowerStatus() const -> Controller::PowerStatus;
#endif

    void setExposureTime(std::chrono::microseconds time);
    void setGain(Camera::Gain gain);
    void setTemperatureSetpoint(Camera::TemperatureSetpoint point);

    auto getCameraStatus() const -> Camera::Status;

    static auto getAvailableCameras() -> std::vector<std::string>;
    static auto isCameraAvailable(const std::string& serial) -> bool;

private:
    void setSequenceConfiguration(int refreshRate, std::chrono::microseconds illuminationTime);

private:
    AVT::VmbAPI::VimbaSystem* m_system;

#ifdef SKINCAM_USE_DUMMY_CAMERA
    DummyCameraPtr m_camera;
#else
    CameraPtr m_camera;
#endif

    ControllerPtr m_controller;

    int m_refreshRate = DefaultRefreshRate;
    std::chrono::microseconds m_illuminationTime = DefaultIlluminationTime;

    mutable std::mutex m_mutex;
};

SKINCAM_END_NAMESPACE();
