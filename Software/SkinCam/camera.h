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

#include "cameraexception.h"
#include "global.h"
#include "memorybuffer.h"

#include <VimbaCPP/Include/VimbaCPP.h>

#include <condition_variable>
#include <thread>

using namespace std::chrono_literals;

SKINCAM_BEGIN_NAMESPACE();

class Camera;
using CameraPtr = std::unique_ptr<Camera>;

class Camera
{
public:
    static constexpr std::chrono::microseconds MinimumExposureTime =  1000us;
#ifdef SKINCAM_BUILD_LEGACY
    // Legacy system lacks feedback signal from camera and would loose triggers
    static constexpr std::chrono::microseconds MaximumExposureTime =  9000us;
#else
    static constexpr std::chrono::microseconds MaximumExposureTime = 10000us;
#endif
    static constexpr std::chrono::microseconds DefaultExposureTime =  5000us;

    enum class TemperatureSetpoint
    {
        Automatic           = 0,
        FiveDegrees         = 1,
        TwentyDegrees       = 2,
        ThirtyFiveDegrees   = 3,
        FiftyDegrees        = 4
    };

    static constexpr auto DefaultTemperatureSetpoint = TemperatureSetpoint::Automatic;

    static constexpr size_t TemperatureSetpointCount = 5;

    template <typename T = size_t>
    static constexpr auto indexOf(TemperatureSetpoint point) { return static_cast<T>(point); }

    template <typename T>
    static constexpr auto toTemperatureSetpoint(T index)
    {
        ASSERT(index < static_cast<T>(TemperatureSetpointCount));
        return static_cast<TemperatureSetpoint>(index);
    }

    enum class Gain
    {
        Low,
#ifndef SKINCAM_BUILD_LEGACY
        Medium,
#endif
        High
    };

    static constexpr auto GainCount = static_cast<size_t>(Gain::High) + 1;

    template <typename T = size_t>
    static constexpr auto indexOf(Gain gain) { return static_cast<T>(gain); }

    template <typename T>
    static constexpr auto toGain(T index)
    {
        ASSERT(index < static_cast<T>(GainCount));
        return static_cast<Gain>(index);
    }

    struct Setup
    {
        std::string serial;
        std::chrono::microseconds exposureTime = DefaultExposureTime;
        TemperatureSetpoint temperatureSetpoint = DefaultTemperatureSetpoint;
        Gain gain = Gain::Low;
    };

    struct Status
    {
        TemperatureSetpoint temperatureSetpoint = TemperatureSetpoint::Automatic;

        double temperature = 0.0;
        double humidity = 0.0;

        int coolingPower = 0;

        size_t packetsReceived = 0;
        size_t packetsRequested = 0;
    };

    static constexpr size_t ImagesPerFrame = ChannelCount + 1; // plus dark image

public:
    Camera(const Setup& setup, AVT::VmbAPI::VimbaSystem* system);
    ~Camera();

    void start();
    void stop();

    void getNextFrame(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                      MemoryBuffer1w* channel1, MemoryBuffer1w* channel2);

    void setExposureTime(std::chrono::microseconds time);
    void setGain(Gain gain);
    void setTemperatureSetpoint(TemperatureSetpoint point);

    auto getStatus() const -> Status;

private:
    class FeatureObserver;
    void setupFeatureObserver();
    void handleFeatureChanged(const AVT::VmbAPI::FeaturePtr& feature);

    class FrameObserver;
    void handleFrameReceived(const AVT::VmbAPI::FramePtr& frame);

    auto getReceiveStatus(const AVT::VmbAPI::FramePtr& frame) const -> VmbFrameStatusType;
    auto getImagePointer(const AVT::VmbAPI::FramePtr& frame) const -> VmbUchar_t*;

    void runFeatureCommand(const std::string& name);

    template <typename T>
    void setFeatureValue(const std::string& name, const T& value);

    template <typename T>
    auto getFeatureValue(const std::string& name) const -> T;

    void configureDevice(const Setup& setup);

    void checkCameraModel();
    void checkPacketSize();

    static void checkReceiveBufferSize();

private:
    std::string m_serial;

    AVT::VmbAPI::VimbaSystem* m_system;
    mutable AVT::VmbAPI::CameraPtr m_camera;

    Status m_status;

    mutable std::mutex m_mutex;
    mutable std::exception_ptr m_exception = nullptr;

    std::condition_variable m_condition;

    std::array<MemoryBuffer1w, ImagesPerFrame> m_currentFrames = {{
        { ImageHeight, ImageWidth },
        { ImageHeight, ImageWidth },
        { ImageHeight, ImageWidth },
        { ImageHeight, ImageWidth }
    }};

    size_t m_currentFrameIndex = 0;
    bool m_frameAvailable = false;

    int m_incompleteFrameScore = 0;

    bool m_running = false;
};

SKINCAM_END_NAMESPACE();
