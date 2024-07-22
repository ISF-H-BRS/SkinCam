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

#include "dummycamera.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

DummyCamera::DummyCamera() {}

// ---------------------------------------------------------------------------------------------- //

void DummyCamera::start()
{
    m_packetsReceived = 0;
}

// ---------------------------------------------------------------------------------------------- //

void DummyCamera::stop() {}

// ---------------------------------------------------------------------------------------------- //

void DummyCamera::getNextFrame(MemoryBuffer1w*, MemoryBuffer1w*,
                               MemoryBuffer1w*, MemoryBuffer1w*)
{
    m_packetsReceived += 1024;
}

// ---------------------------------------------------------------------------------------------- //

void DummyCamera::setExposureTime(std::chrono::microseconds) {}

// ---------------------------------------------------------------------------------------------- //

void DummyCamera::setGain(Camera::Gain) {}

// ---------------------------------------------------------------------------------------------- //

void DummyCamera::setTemperatureSetpoint(Camera::TemperatureSetpoint) {}

// ---------------------------------------------------------------------------------------------- //

auto DummyCamera::getStatus() const -> Camera::Status
{
    return {
        Camera::TemperatureSetpoint::Automatic,
        20.0,
        30.0,
        1000,
        m_packetsReceived,
        0
    };
}

// ---------------------------------------------------------------------------------------------- //
