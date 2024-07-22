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

#include "namespace.h"

#include <algorithm>
#include <chrono>
#include <iostream>

SKINCAM_BEGIN_NAMESPACE();

namespace Assertions {

void fail(const char* condition, const char* function, const char* file, int line);

void notifySatisfied(const char* condition, const char* function);
void notifyUnsatisfied(const char* condition, const char* function);

void notifyIgnoreNull(const char* function, const char* ptrarg);
void notifyIgnoreNotNull(const char* function, const char* ptrarg);

inline
auto operator<<(std::ostream& os, std::chrono::microseconds us) -> std::ostream&
{
    os << us.count() << " µs";
    return os;
}

template <typename T>
void clampToRange(T& value, const T& min, const T& max, const char* function)
{
    T newValue = std::clamp(value, min, max);

    std::cerr << "In function " << function << ": Value " << value
              << " is outside range (" << min << "," << max << "). "
              << "Clamping to " << newValue << "." << std::endl;

    value = newValue;
}

template <typename T>
void notifyIgnoreOutOfRange(const T& value, const T& min, const T& max, const char* function)
{
    std::cerr << "In function " << function << ": Value " << value
              << " is outside range (" << min << "," << max << "). "
              << "Aborting function call." << std::endl;
}

} // End of namespace Assertions


#ifdef NDEBUG
#define ASSERT(condition)
#else
#define ASSERT(condition) \
    if (!(condition)) Assertions::fail(#condition, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#endif

#define ASSERT_NULL(p) ASSERT((p) == nullptr)
#define ASSERT_NOT_NULL(p) ASSERT((p) != nullptr)
#define ASSERT_IN_RANGE(value, min, max) ASSERT((value) >= (min) && (value) <= (max))


#ifdef NDEBUG
#define CLAMP_TO_RANGE(value, min, max) \
    if (value < min || value > max) Assertions::clampToRange(value, min, max, __PRETTY_FUNCTION__)
#else
#define CLAMP_TO_RANGE(value, min, max) ASSERT_IN_RANGE(value, min, max)
#endif


#ifdef NDEBUG
#define RETURN_IF(condition) \
    if (condition) return Assertions::notifySatisfied(#condition, __PRETTY_FUNCTION__)
#else
#define RETURN_IF(condition) ASSERT(!(condition))
#endif


#ifdef NDEBUG
#define RETURN_IF_NOT(condition) \
    if (!(condition)) return Assertions::notifyUnsatisfied(#condition, __PRETTY_FUNCTION__)
#else
#define RETURN_IF_NOT(condition) ASSERT(condition)
#endif


#ifdef NDEBUG
#define RETURN_IF_OUT_OF_RANGE(value, min, max) \
    if (value < min || value > max) \
        return Assertions::notifyIgnoreOutOfRange(value, min, max, __PRETTY_FUNCTION__))
#else
#define RETURN_IF_OUT_OF_RANGE(value, min, max) ASSERT_IN_RANGE(value, min, max)
#endif


#ifdef NDEBUG
#define RETURN_IF_NULL(ptr) \
    if ((ptr) == nullptr) return Assertions::notifyIgnoreNull(__PRETTY_FUNCTION__, #ptr)
#else
#define RETURN_IF_NULL(ptr) ASSERT_NOT_NULL(ptr)
#endif


#ifdef NDEBUG
#define RETURN_IF_NOT_NULL(ptr) \
    if ((ptr) != nullptr) return Assertions::notifyIgnoreNotNull(__PRETTY_FUNCTION__, #ptr)
#else
#define RETURN_IF_NOT_NULL(ptr) ASSERT_NULL(ptr)
#endif

SKINCAM_END_NAMESPACE();
