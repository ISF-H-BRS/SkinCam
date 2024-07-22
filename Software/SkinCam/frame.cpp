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

#include "frame.h"
#include "global.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

ObjectPool<Frame::SharedData> Frame::s_objectPool;

// ---------------------------------------------------------------------------------------------- //

struct Frame::SharedData
{
    QImage rawDarkImage = { ImageWidth, ImageHeight, QImage::Format_Grayscale8 };
    QImage raw0Image    = { ImageWidth, ImageHeight, QImage::Format_Grayscale8 };
    QImage raw1Image    = { ImageWidth, ImageHeight, QImage::Format_Grayscale8 };
    QImage raw2Image    = { ImageWidth, ImageHeight, QImage::Format_Grayscale8 };

    QImage channel0Image = { ImageWidth, ImageHeight, QImage::Format_Grayscale8 };
    QImage channel1Image = { ImageWidth, ImageHeight, QImage::Format_Grayscale8 };
    QImage channel2Image = { ImageWidth, ImageHeight, QImage::Format_Grayscale8 };

    QImage compositeImage = { ImageWidth, ImageHeight, QImage::Format_RGBA8888 };

    QImage skinMaskImage = { ImageWidth, ImageHeight, QImage::Format_Grayscale8 };
    double skinRatio = 0.0;

    std::atomic<size_t> refCount = 0;
};

// ---------------------------------------------------------------------------------------------- //

Frame::Frame()
    : d(s_objectPool.acquire())
{
    ref();
}

// ---------------------------------------------------------------------------------------------- //

Frame::Frame(const ComputeBuffer1b& rawDark, const ComputeBuffer1b& raw0,
             const ComputeBuffer1b& raw1, const ComputeBuffer1b& raw2,
             const ComputeBuffer1b& channel0, const ComputeBuffer1b& channel1,
             const ComputeBuffer1b& channel2, const ComputeBuffer4b& composite,
             const ComputeBuffer1b& skinMask, double skinRatio)
    : d(s_objectPool.acquire())
{
    ref();

    rawDark.copyTo(d->rawDarkImage.bits());
    raw0.copyTo(d->raw0Image.bits());
    raw1.copyTo(d->raw1Image.bits());
    raw2.copyTo(d->raw2Image.bits());

    channel0.copyTo(d->channel0Image.bits());
    channel1.copyTo(d->channel1Image.bits());
    channel2.copyTo(d->channel2Image.bits());
    composite.copyTo(d->compositeImage.bits());

    skinMask.copyTo(d->skinMaskImage.bits());
    d->skinRatio = skinRatio;
}

// ---------------------------------------------------------------------------------------------- //

Frame::Frame(const Frame& other)
    : d(other.d)
{
    ref();
}

// ---------------------------------------------------------------------------------------------- //

Frame::~Frame()
{
    unref();
}

// ---------------------------------------------------------------------------------------------- //

auto Frame::operator=(const Frame& other) -> Frame&
{
    if (&other != this)
    {
        unref();
        d = other.d;
        ref();
    }

    return *this;
}

// ---------------------------------------------------------------------------------------------- //

auto Frame::rawDarkImage() const  -> const QImage&
{
    return d->rawDarkImage;
}

// ---------------------------------------------------------------------------------------------- //

auto Frame::raw0Image() const  -> const QImage&
{
    return d->raw0Image;
}

// ---------------------------------------------------------------------------------------------- //

auto Frame::raw1Image() const  -> const QImage&
{
    return d->raw1Image;
}

// ---------------------------------------------------------------------------------------------- //

auto Frame::raw2Image() const  -> const QImage&
{
    return d->raw2Image;
}

// ---------------------------------------------------------------------------------------------- //

auto Frame::channel0Image() const  -> const QImage&
{
    return d->channel0Image;
}

// ---------------------------------------------------------------------------------------------- //

auto Frame::channel1Image() const  -> const QImage&
{
    return d->channel1Image;
}

// ---------------------------------------------------------------------------------------------- //

auto Frame::channel2Image() const  -> const QImage&
{
    return d->channel2Image;
}

// ---------------------------------------------------------------------------------------------- //

auto Frame::compositeImage() const  -> const QImage&
{
    return d->compositeImage;
}

// ---------------------------------------------------------------------------------------------- //

auto Frame::skinMaskImage() const -> const QImage&
{
    return d->skinMaskImage;
}

// ---------------------------------------------------------------------------------------------- //

auto Frame::skinRatio() const -> double
{
    return d->skinRatio;
}

// ---------------------------------------------------------------------------------------------- //

inline
void Frame::ref()
{
    ASSERT_NOT_NULL(d);
    ++d->refCount;
}

// ---------------------------------------------------------------------------------------------- //

inline
void Frame::unref()
{
    ASSERT_NOT_NULL(d);

    if (--d->refCount == 0)
    {
        s_objectPool.release(d);
        d = nullptr;
    }
}

// ---------------------------------------------------------------------------------------------- //
