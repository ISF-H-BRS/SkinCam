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

#include "exception.h"
#include "memorybuffer.h"

SKINCAM_BEGIN_NAMESPACE();

class ImageProvider
{
public:
    virtual ~ImageProvider() = default;

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual auto getMinRefreshRate() const -> int = 0;
    virtual auto getMaxRefreshRate() const -> int = 0;

    virtual void setRefreshRate(int rate) = 0;
    virtual auto getRefreshRate() const -> int = 0;

    virtual void getNextFrame(MemoryBuffer1w* dark, MemoryBuffer1w* channel0,
                              MemoryBuffer1w* channel1, MemoryBuffer1w* channel2) = 0;

protected:
    ImageProvider() = default;
};

SKINCAM_END_NAMESPACE();
