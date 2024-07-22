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
#include "openclwrapper.h"
#include "vectors.h"

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

template <typename T>
class ComputeBuffer : public AbstractBuffer<T>
{
public:
    ComputeBuffer(uint width, uint height, cl_mem_flags flags);
    ComputeBuffer(const T* data, uint width, uint height, cl_mem_flags flags);
    ComputeBuffer(const std::vector<T>& data, uint width, uint height, cl_mem_flags flags);
    ComputeBuffer(ComputeBuffer&& other);

    auto operator=(ComputeBuffer&& other) -> ComputeBuffer&;

    ComputeBuffer(const ComputeBuffer& other) = delete;
    auto operator=(const ComputeBuffer& other) -> ComputeBuffer& = delete;

    auto swap(ComputeBuffer* other) -> ComputeBuffer&;

    void copyTo(ComputeBuffer* other) const;

    void copyTo(void* data) const;
    void copyFrom(const void* data);

    auto map() -> T* override;
    void unmap() override;

    auto buffer() const -> const cl::Buffer&;

private:
    cl::Buffer m_buffer;
    cl_mem_flags m_flags;
    void* m_mappedData = nullptr;
};

// ---------------------------------------------------------------------------------------------- //

using ComputeBuffer1b = ComputeBuffer<Vec1b>;
using ComputeBuffer1w = ComputeBuffer<Vec1w>;
using ComputeBuffer1i = ComputeBuffer<Vec1i>;
using ComputeBuffer1u = ComputeBuffer<Vec1u>;
using ComputeBuffer1f = ComputeBuffer<Vec1f>;
using ComputeBuffer1d = ComputeBuffer<Vec1d>;

using ComputeBuffer2b = ComputeBuffer<Vec2b>;
using ComputeBuffer2w = ComputeBuffer<Vec2w>;
using ComputeBuffer2i = ComputeBuffer<Vec2i>;
using ComputeBuffer2u = ComputeBuffer<Vec2u>;
using ComputeBuffer2f = ComputeBuffer<Vec2f>;
using ComputeBuffer2d = ComputeBuffer<Vec2d>;

using ComputeBuffer3b = ComputeBuffer<Vec3b>;
using ComputeBuffer3w = ComputeBuffer<Vec3w>;
using ComputeBuffer3i = ComputeBuffer<Vec3i>;
using ComputeBuffer3u = ComputeBuffer<Vec3u>;
using ComputeBuffer3f = ComputeBuffer<Vec3f>;
using ComputeBuffer3d = ComputeBuffer<Vec3d>;

using ComputeBuffer4b = ComputeBuffer<Vec4b>;
using ComputeBuffer4w = ComputeBuffer<Vec4w>;
using ComputeBuffer4i = ComputeBuffer<Vec4i>;
using ComputeBuffer4u = ComputeBuffer<Vec4u>;
using ComputeBuffer4f = ComputeBuffer<Vec4f>;
using ComputeBuffer4d = ComputeBuffer<Vec4d>;

// ---------------------------------------------------------------------------------------------- //

template <typename T>
ComputeBuffer<T>::ComputeBuffer(uint width, uint height, cl_mem_flags flags)
    : AbstractBuffer<T>(width, height),
      m_buffer(OpenCLWrapper::getInstance().createBuffer(flags, this->sizeInBytes(width, height))),
      m_flags(flags)
{
    ASSERT(width > 0 && height > 0);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
ComputeBuffer<T>::ComputeBuffer(const T* data, uint width, uint height, cl_mem_flags flags)
    : ComputeBuffer(width, height, flags)
{
    OpenCLWrapper::getInstance().enqueueWriteBuffer(m_buffer, this->sizeInBytes(), data);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
ComputeBuffer<T>::ComputeBuffer(const std::vector<T>& data,
                                uint width, uint height, cl_mem_flags flags)
    : ComputeBuffer(data.data(), width, height, flags)
{
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
ComputeBuffer<T>::ComputeBuffer(ComputeBuffer&& other)
    : AbstractBuffer<T>(other),
      m_buffer(std::move(other.m_buffer)),
      m_flags(other.m_flags)
{
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto ComputeBuffer<T>::operator=(ComputeBuffer&& other) -> ComputeBuffer&
{
    AbstractBuffer<T>::operator=(other);
    m_buffer = std::move(other.m_buffer);
    m_flags = other.m_flags;

    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto ComputeBuffer<T>::swap(ComputeBuffer* other) -> ComputeBuffer&
{
    ASSERT_NULL(m_mappedData);
    ASSERT_NULL(other->m_mappedData);

    std::swap(m_buffer, other->m_buffer);
    std::swap(m_flags, other->m_flags);

    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void ComputeBuffer<T>::copyTo(ComputeBuffer* other) const
{
    ASSERT_NULL(m_mappedData);
    OpenCLWrapper::getInstance().enqueueCopyBuffer(m_buffer, other->m_buffer, this->sizeInBytes());
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void ComputeBuffer<T>::copyTo(void* data) const
{
    ASSERT_NULL(m_mappedData);
    OpenCLWrapper::getInstance().enqueueReadBuffer(m_buffer, this->sizeInBytes(), data);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void ComputeBuffer<T>::copyFrom(const void* data)
{
    ASSERT_NULL(m_mappedData);
    OpenCLWrapper::getInstance().enqueueWriteBuffer(m_buffer, this->sizeInBytes(), data);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto ComputeBuffer<T>::map() -> T*
{
    ASSERT(m_flags & CL_MEM_ALLOC_HOST_PTR);
    ASSERT_NULL(m_mappedData);

    m_mappedData = OpenCLWrapper::getInstance().enqueueMapBuffer(m_buffer, this->sizeInBytes());
    return reinterpret_cast<T*>(m_mappedData);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void ComputeBuffer<T>::unmap()
{
    ASSERT_NOT_NULL(m_mappedData);

    OpenCLWrapper::getInstance().enqueueUnmapBuffer(m_buffer, m_mappedData);
    m_mappedData = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto ComputeBuffer<T>::buffer() const -> const cl::Buffer&
{
    return m_buffer;
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
