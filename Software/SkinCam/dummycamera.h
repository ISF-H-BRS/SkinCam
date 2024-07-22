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

SKINCAM_BEGIN_NAMESPACE();

class DummyCamera;
using DummyCameraPtr = std::unique_ptr<DummyCamera>;

class DummyCamera
{
public:
    DummyCamera();

    void start();
    void stop();

    void getNextFrame(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                      MemoryBuffer1w* channel1, MemoryBuffer1w* channel2);

    void setExposureTime(std::chrono::microseconds time);
    void setGain(Camera::Gain gain);
    void setTemperatureSetpoint(Camera::TemperatureSetpoint point);

    auto getStatus() const -> Camera::Status;

private:
    size_t m_packetsReceived = 0;
};

SKINCAM_END_NAMESPACE();