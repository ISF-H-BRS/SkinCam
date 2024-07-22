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

#include "abstractbuffer.h"
#include "computebuffer.h"
#include "vectors.h"

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

template <typename T>
class MemoryBuffer : public AbstractBuffer<T>
{
public:
    using ConstForEachFunc = std::function<void(const T&,uint,uint)>;

public:
    MemoryBuffer(uint width, uint height);
    MemoryBuffer(const T* data, uint width, uint height);
    MemoryBuffer(const std::vector<T>& data, uint width, uint height);
    MemoryBuffer(const MemoryBuffer& other);
    MemoryBuffer(MemoryBuffer&& other);

    auto operator=(const MemoryBuffer& other) -> MemoryBuffer&;
    auto operator=(MemoryBuffer&& other) -> MemoryBuffer&;

    auto swap(MemoryBuffer* other) -> MemoryBuffer&;

    auto at(uint x, uint y) -> T&;
    auto at(uint x, uint y) const -> const T&;

    auto map() -> T* override;
    void unmap() override;

    auto data() -> T*;
    auto data() const -> const T*;

    auto vector() const -> const std::vector<T>&;

    void copyTo(T* data) const;
    void copyFrom(const T* data);

    void copyTo(MemoryBuffer* buffer) const;
    void copyFrom(const MemoryBuffer& buffer);

    void copyTo(ComputeBuffer<T>* buffer) const;
    void copyFrom(const ComputeBuffer<T>& buffer);

    void forEach(const ConstForEachFunc& func) const;

    auto toComputeBuffer(cl_mem_flags flags) const -> ComputeBuffer<T>;
    static auto fromComputeBuffer(const ComputeBuffer<T>& buffer) -> MemoryBuffer;

private:
    std::vector<T> m_data;
};

// ---------------------------------------------------------------------------------------------- //

using MemoryBuffer1b = MemoryBuffer<Vec1b>;
using MemoryBuffer1w = MemoryBuffer<Vec1w>;
using MemoryBuffer1i = MemoryBuffer<Vec1i>;
using MemoryBuffer1u = MemoryBuffer<Vec1u>;
using MemoryBuffer1f = MemoryBuffer<Vec1f>;
using MemoryBuffer1d = MemoryBuffer<Vec1d>;

using MemoryBuffer2b = MemoryBuffer<Vec2b>;
using MemoryBuffer2w = MemoryBuffer<Vec2w>;
using MemoryBuffer2i = MemoryBuffer<Vec2i>;
using MemoryBuffer2u = MemoryBuffer<Vec2u>;
using MemoryBuffer2f = MemoryBuffer<Vec2f>;
using MemoryBuffer2d = MemoryBuffer<Vec2d>;

using MemoryBuffer3b = MemoryBuffer<Vec3b>;
using MemoryBuffer3w = MemoryBuffer<Vec3w>;
using MemoryBuffer3i = MemoryBuffer<Vec3i>;
using MemoryBuffer3u = MemoryBuffer<Vec3u>;
using MemoryBuffer3f = MemoryBuffer<Vec3f>;
using MemoryBuffer3d = MemoryBuffer<Vec3d>;

using MemoryBuffer4b = MemoryBuffer<Vec4b>;
using MemoryBuffer4w = MemoryBuffer<Vec4w>;
using MemoryBuffer4i = MemoryBuffer<Vec4i>;
using MemoryBuffer4u = MemoryBuffer<Vec4u>;
using MemoryBuffer4f = MemoryBuffer<Vec4f>;
using MemoryBuffer4d = MemoryBuffer<Vec4d>;

// ---------------------------------------------------------------------------------------------- //

template <typename T>
MemoryBuffer<T>::MemoryBuffer(uint width, uint height)
    : AbstractBuffer<T>(width, height),
      m_data(width * height) {}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
MemoryBuffer<T>::MemoryBuffer(const T* data, uint width, uint height)
    : MemoryBuffer(width, height)
{
    std::copy_n(data, m_data.size(), m_data.begin());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
MemoryBuffer<T>::MemoryBuffer(const std::vector<T>& data, uint width, uint height)
    : AbstractBuffer<T>(width, height),
      m_data(data) {}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
MemoryBuffer<T>::MemoryBuffer(const MemoryBuffer& other)
    : AbstractBuffer<T>(other),
      m_data(other.m_data) {}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
MemoryBuffer<T>::MemoryBuffer(MemoryBuffer&& other)
    : AbstractBuffer<T>(other),
      m_data(std::move(other.m_data)) {}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto MemoryBuffer<T>::operator=(const MemoryBuffer& other) -> MemoryBuffer&
{
    AbstractBuffer<T>::operator=(other);
    m_data = other.m_data;

    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto MemoryBuffer<T>::operator=(MemoryBuffer&& other) -> MemoryBuffer&
{
    AbstractBuffer<T>::operator=(other);
    m_data = std::move(other.m_data);

    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto MemoryBuffer<T>::swap(MemoryBuffer* other) -> MemoryBuffer&
{
    m_data.swap(other->m_data);
    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto MemoryBuffer<T>::at(uint x, uint y) -> T&
{
    return m_data.at(y * this->width() + x);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto MemoryBuffer<T>::at(uint x, uint y) const -> const T&
{
    return m_data.at(y * this->width() + x);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto MemoryBuffer<T>::map() -> T*
{
    return m_data.data();
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void MemoryBuffer<T>::unmap()
{
    // Nothing to do
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto MemoryBuffer<T>::data() -> T*
{
    return m_data.data();
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto MemoryBuffer<T>::data() const -> const T*
{
    return m_data.data();
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto MemoryBuffer<T>::vector() const -> const std::vector<T>&
{
    return m_data;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void MemoryBuffer<T>::copyTo(T* data) const
{
    auto src = reinterpret_cast<const uchar*>(m_data.data());
    auto dst = reinterpret_cast<uchar*>(data);

    std::memcpy(dst, src, this->sizeInBytes());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void MemoryBuffer<T>::copyFrom(const T* data)
{
    auto src = reinterpret_cast<const uchar*>(data);
    auto dst = reinterpret_cast<uchar*>(m_data.data());

    std::memcpy(dst, src, this->sizeInBytes());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void MemoryBuffer<T>::copyTo(MemoryBuffer* buffer) const
{
    ASSERT_NOT_NULL(buffer);
    ASSERT(buffer->sizeInBytes() == this->sizeInBytes());

    auto src = reinterpret_cast<const uchar*>(m_data.data());
    auto dst = reinterpret_cast<uchar*>(buffer->m_data.data());

    std::memcpy(dst, src, this->sizeInBytes());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void MemoryBuffer<T>::copyFrom(const MemoryBuffer& buffer)
{
    ASSERT(buffer.sizeInBytes() == this->sizeInBytes());

    auto src = reinterpret_cast<const uchar*>(buffer.m_data.data());
    auto dst = reinterpret_cast<uchar*>(m_data.data());

    std::memcpy(dst, src, this->sizeInBytes());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void MemoryBuffer<T>::copyTo(ComputeBuffer<T>* buffer) const
{
    ASSERT_NOT_NULL(buffer);
    ASSERT(buffer->sizeInBytes() == this->sizeInBytes());

    OpenCLWrapper::getInstance().enqueueWriteBuffer(buffer->buffer(),
                                                    this->sizeInBytes(), m_data.data());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void MemoryBuffer<T>::copyFrom(const ComputeBuffer<T>& buffer)
{
    ASSERT(buffer.sizeInBytes() == this->sizeInBytes());

    OpenCLWrapper::getInstance().enqueueReadBuffer(buffer.buffer(),
                                                   this->sizeInBytes(), m_data.data());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void MemoryBuffer<T>::forEach(const ConstForEachFunc& func) const
{
    static unsigned int threads = std::max(std::thread::hardware_concurrency(), 1U);

    const uint width = this->width();
    const uint height = this->height();

    std::atomic<uint> nextLine(0);

    auto process = [&]()
    {
        for (uint y = nextLine++; y < height; y = nextLine++)
        {
            for (uint x = 0; x < width; ++x)
                func(m_data[y * width + x], x, y);
        }
    };

    std::vector<std::future<void>> futures(threads);

    for (unsigned int i = 0; i < threads; ++i)
        futures.at(i) = std::async(std::launch::async, process);

    for (auto& future : futures)
        future.get();
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto MemoryBuffer<T>::toComputeBuffer(cl_mem_flags flags) const -> ComputeBuffer<T>
{
    return ComputeBuffer<T>(m_data, this->width(), this->height(), flags);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto MemoryBuffer<T>::fromComputeBuffer(const ComputeBuffer<T>& buffer) -> MemoryBuffer
{
    MemoryBuffer image(buffer.width(), buffer.height());
    image.copyFrom(buffer);
    return image;
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
