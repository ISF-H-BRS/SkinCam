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

#include "objectpool.h"

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

template <typename T>
class MemoryPool
{
public:
    class Destructor;
    using UniquePtr = std::unique_ptr<T, Destructor>;

    template <typename... Args>
    auto makeUnique(Args&&... args) -> UniquePtr;

private:
    void release(T* object);

private:
    using ObjectBuffer = std::array<std::byte, sizeof(T)>;
    ObjectPool<ObjectBuffer> m_objectPool;
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
class MemoryPool<T>::Destructor
{
public:
    Destructor(MemoryPool* pool = nullptr)
        : m_pool(pool) {}

    void operator()(T* p)
    {
        if (p)
        {
            ASSERT_NOT_NULL(m_pool);

            p->~T();
            m_pool->release(p);
        }
    }

private:
    MemoryPool* m_pool;
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
template <typename... Args>
auto MemoryPool<T>::makeUnique(Args&&... args) -> UniquePtr
{
    const auto ptr = m_objectPool.acquire();
    return UniquePtr(new (ptr) T(std::forward<Args>(args)...), Destructor(this));
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void MemoryPool<T>::release(T* object)
{
    const auto ptr = reinterpret_cast<ObjectBuffer*>(object);
    m_objectPool.release(ptr);
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
