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

#include "computebuffer.h"
#include "objectpool.h"

#include <QImage>

SKINCAM_BEGIN_NAMESPACE();

class Frame
{
public:
    Frame();
    Frame(const ComputeBuffer1b& rawDark, const ComputeBuffer1b& raw0,
          const ComputeBuffer1b& raw1, const ComputeBuffer1b& raw2,
          const ComputeBuffer1b& channel0, const ComputeBuffer1b& channel1,
          const ComputeBuffer1b& channel2, const ComputeBuffer4b& composite,
          const ComputeBuffer1b& skinMask, double skinRatio = 0.0);
    Frame(const Frame& other);
    ~Frame();

    auto operator=(const Frame& other) -> Frame&;

    Frame(Frame&& other) = delete;
    auto operator=(Frame&& other) -> Frame& = delete;

    auto rawDarkImage() const  -> const QImage&;
    auto raw0Image() const  -> const QImage&;
    auto raw1Image() const  -> const QImage&;
    auto raw2Image() const  -> const QImage&;

    auto channel0Image() const  -> const QImage&;
    auto channel1Image() const  -> const QImage&;
    auto channel2Image() const  -> const QImage&;
    auto compositeImage() const  -> const QImage&;

    auto skinMaskImage() const -> const QImage&;
    auto skinRatio() const -> double;

private:
    void ref();
    void unref();

private:
    struct SharedData;
    SharedData* d = nullptr;

    static ObjectPool<SharedData> s_objectPool;
};

SKINCAM_END_NAMESPACE();
