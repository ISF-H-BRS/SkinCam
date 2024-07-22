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

#include <memory>
#include <mutex>
#include <vector>

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

template <typename T>
class ObjectPool
{
public:
    auto acquire() -> T*;
    void release(T* object);

private:
    std::vector<std::unique_ptr<T>> m_objects;
    std::vector<T*> m_usedObjects;
    std::vector<T*> m_freeObjects;
    std::mutex m_mutex;
};

// ---------------------------------------------------------------------------------------------- //

template <typename T>
auto ObjectPool<T>::acquire() -> T*
{
    std::lock_guard lock(m_mutex);

    if (m_freeObjects.empty())
    {
        m_objects.emplace_back(new T);
        m_freeObjects.push_back(m_objects.back().get());
    }

    T* data = m_freeObjects.back();
    m_freeObjects.pop_back();
    m_usedObjects.push_back(data);

    return data;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
void ObjectPool<T>::release(T* object)
{
    std::lock_guard lock(m_mutex);

    auto it = std::find(m_usedObjects.begin(), m_usedObjects.end(), object);
    ASSERT(it != m_usedObjects.end());

    m_usedObjects.erase(it);
    m_freeObjects.push_back(object);
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
