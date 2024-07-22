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

#include "assertions.h"
#include "types.h"

#include <future>
#include <vector>

SKINCAM_BEGIN_NAMESPACE();

template <typename T>
class AbstractBuffer
{
public:
    using ForEachFunc = std::function<void(T&,uint,uint)>;

public:
    virtual auto map() -> T* = 0;
    virtual void unmap() = 0;

    auto width() const -> uint;
    auto height() const -> uint;

    auto pixelCount() const -> size_t;
    auto sizeInBytes() const -> size_t;

    void forEach(const ForEachFunc& func);

    static auto sizeInBytes(uint width, uint height) -> size_t;

protected:
    AbstractBuffer(uint width, uint height);
    AbstractBuffer(const AbstractBuffer& other);
    AbstractBuffer(AbstractBuffer&& other);

    auto operator=(const AbstractBuffer& other) -> AbstractBuffer&;
    auto operator=(AbstractBuffer&& other) -> AbstractBuffer&;

private:
    template <typename Func, typename U, typename Buffer>
    static void forEach(Buffer& buffer, const Func& func);

private:
    uint m_width;
    uint m_height;
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
AbstractBuffer<T>::AbstractBuffer(uint width, uint height)
    : m_width(width),
      m_height(height)
{
    ASSERT(width > 0 && height > 0);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
AbstractBuffer<T>::AbstractBuffer(const AbstractBuffer& other)
    : m_width(other.m_width),
      m_height(other.m_height)
{
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
AbstractBuffer<T>::AbstractBuffer(AbstractBuffer&& other)
    : m_width(other.m_width),
      m_height(other.m_height)
{
    other.m_width = 0;
    other.m_height = 0;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto AbstractBuffer<T>::operator=(const AbstractBuffer& other) -> AbstractBuffer&
{
    m_width = other.m_width;
    m_height = other.m_height;
    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto AbstractBuffer<T>::operator=(AbstractBuffer&& other) -> AbstractBuffer&
{
    m_width = other.m_width;
    m_height = other.m_height;

    other.m_width = 0;
    other.m_height = 0;

    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto AbstractBuffer<T>::width() const -> uint
{
    return m_width;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto AbstractBuffer<T>::height() const -> uint
{
    return m_height;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto AbstractBuffer<T>::pixelCount() const -> size_t
{
    return m_width * m_height;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto AbstractBuffer<T>::sizeInBytes() const -> size_t
{
    return pixelCount() * sizeof(T);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto AbstractBuffer<T>::sizeInBytes(uint width, uint height) -> size_t
{
    return width * height * sizeof(T);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void AbstractBuffer<T>::forEach(const ForEachFunc& func)
{
    static unsigned int threads = std::max(std::thread::hardware_concurrency(), 1U);

    const uint width = this->width();
    const uint height = this->height();

    std::atomic<uint> nextLine(0);

    T* data = map();

    auto process = [&]()
    {
        for (uint y = nextLine++; y < height; y = nextLine++)
        {
            for (uint x = 0; x < width; ++x)
                func(data[y * width + x], x, y);
        }
    };

    std::vector<std::future<void>> futures(threads);

    for (unsigned int i = 0; i < threads; ++i)
        futures.at(i) = std::async(std::launch::async, process);

    for (auto& future : futures)
        future.wait();

    unmap(); // Unmap before get(), in case of exception

    for (auto& future : futures)
        future.get();
}

SKINCAM_END_NAMESPACE();
