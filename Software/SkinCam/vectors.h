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

#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E = std::enable_if_t<N >= 1 && N <= 4>>
class Vec
{
public:
    using Type = typename ClType<T,N>::Type;

public:
    constexpr Vec() {}

    constexpr Vec(T v);
    constexpr Vec(T v0, T v1);
    constexpr Vec(T v0, T v1, T v2);
    constexpr Vec(T v0, T v1, T v2, T v3);

    constexpr Vec(const T* data);
    constexpr Vec(const Vec& other);
    constexpr Vec(Vec&& other);

    ~Vec();

    constexpr auto operator=(const Vec& other) -> Vec&;
    constexpr auto operator=(Vec&& other) -> Vec&;

    constexpr auto operator+(const Vec& other) const -> Vec;
    constexpr auto operator+=(const Vec& other) -> Vec&;

    constexpr auto operator-(const Vec& other) const -> Vec;
    constexpr auto operator-=(const Vec& other) -> Vec&;

    constexpr auto operator*(const Vec& other) const -> Vec;
    constexpr auto operator*=(const Vec& other) -> Vec&;

    constexpr auto operator/(const Vec& other) const -> Vec;
    constexpr auto operator/=(const Vec& other) -> Vec&;

    constexpr auto operator==(const Vec& other) const -> bool;
    constexpr auto operator!=(const Vec& other) const -> bool;

    constexpr auto at(size_t n) -> T&;
    constexpr auto at(size_t n) const -> T;

    constexpr auto x() -> T& { return at(0); }
    constexpr auto x() const -> T { return at(0); }

    constexpr auto y() -> T& { return at(1); }
    constexpr auto y() const -> T { return at(1); }

    constexpr auto z() -> T& { return at(2); }
    constexpr auto z() const -> T { return at(2); }

    constexpr auto w() -> T& { return at(3); }
    constexpr auto w() const -> T { return at(3); }

    constexpr auto r() -> T& { return at(0); }
    constexpr auto r() const -> T { return at(0); }

    constexpr auto g() -> T& { return at(1); }
    constexpr auto g() const -> T { return at(1); }

    constexpr auto b() -> T& { return at(2); }
    constexpr auto b() const -> T { return at(2); }

    constexpr auto a() -> T& { return at(3); }
    constexpr auto a() const -> T { return at(3); }

    constexpr auto value() -> Type& { return m_value; }
    constexpr auto value() const -> const Type& { return m_value; }

    constexpr operator Type() { return m_value; }
    constexpr operator Type() const { return m_value; }

    static constexpr auto sqrt(const Vec& v) -> Vec;
    static constexpr auto ln(const Vec& v) -> Vec;

private:
    Type m_value = {};
};

// ---------------------------------------------------------------------------------------------- //

using Vec1b = Vec<uchar, 1>;
using Vec2b = Vec<uchar, 2>;
using Vec3b = Vec<uchar, 3>;
using Vec4b = Vec<uchar, 4>;

using Vec1w = Vec<ushort, 1>;
using Vec2w = Vec<ushort, 2>;
using Vec3w = Vec<ushort, 3>;
using Vec4w = Vec<ushort, 4>;

using Vec1i = Vec<int, 1>;
using Vec2i = Vec<int, 2>;
using Vec3i = Vec<int, 3>;
using Vec4i = Vec<int, 4>;

using Vec1u = Vec<uint, 1>;
using Vec2u = Vec<uint, 2>;
using Vec3u = Vec<uint, 3>;
using Vec4u = Vec<uint, 4>;

using Vec1f = Vec<float, 1>;
using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;

using Vec1d = Vec<double, 1>;
using Vec2d = Vec<double, 2>;
using Vec3d = Vec<double, 3>;
using Vec4d = Vec<double, 4>;

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const Vec<T,N>& v)
{
    os << "Vec<" << typeid(T).name() << "," << N << ">(";

    for (size_t i = 0; i < N; ++i)
    {
        if (i != 0)
            os << ",";

        os << v.at(i);
    }

    os << ")";

    return os;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr Vec<T,N,E>::Vec(T v)
    : m_value{v}
{
    if constexpr (N > 1)
    {
        for (size_t i = 1; i < N; ++i)
            m_value.s[i] = v;
    }
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr Vec<T,N,E>::Vec(T v0, T v1)
    : m_value{v0, v1}
{
    if constexpr (N > 2)
    {
        for (size_t i = 2; i < N; ++i)
            m_value.s[i] = v1;
    }
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr Vec<T,N,E>::Vec(T v0, T v1, T v2)
    : m_value{v0, v1, v2}
{
    if constexpr (N > 3)
    {
        for (size_t i = 3; i < N; ++i)
            m_value.s[i] = v2;
    }
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr Vec<T,N,E>::Vec(T v0, T v1, T v2, T v3)
    : m_value{v0, v1, v2, v3} {}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr Vec<T,N,E>::Vec(const T* data)
{
    std::copy_n(data, N, m_value.s);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr Vec<T,N,E>::Vec(const Vec& other)
    : m_value(other.m_value) {}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr Vec<T,N,E>::Vec(Vec&& other)
    : m_value(other.m_value) {}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
Vec<T,N,E>::~Vec() = default;

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator=(const Vec& other) -> Vec&
{
    m_value = other.m_value;
    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator=(Vec&& other) -> Vec&
{
    m_value = other.m_value;
    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator+(const Vec& other) const -> Vec
{
    Vec result = *this;
    return result += other;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator+=(const Vec& other) -> Vec&
{
    std::transform(m_value.s, m_value.s+N, other.m_value.s, m_value.s, std::plus<T>());
    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N>
constexpr auto operator+(const T& scalar, const Vec<T,N>& vector) -> Vec<T,N>
{
    return Vec<T,N>(scalar) + vector;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto operator+(const Vec<T,N>& vector, const T& scalar) -> Vec<T,N>
{
    return scalar + vector;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator-(const Vec& other) const -> Vec
{
    Vec result = *this;
    return result -= other;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator-=(const Vec& other) -> Vec&
{
    std::transform(m_value.s, m_value.s+N, other.m_value.s, m_value.s, std::minus<T>());
    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N>
constexpr auto operator-(const T& scalar, const Vec<T,N>& vector) -> Vec<T,N>
{
    return Vec<T,N>(scalar) - vector;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto operator-(const Vec<T,N>& vector, const T& scalar) -> Vec<T,N>
{
    return scalar - vector;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator*(const Vec& other) const -> Vec
{
    Vec result = *this;
    return result *= other;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator*=(const Vec& other) -> Vec&
{
    std::transform(m_value.s, m_value.s+N, other.m_value.s, m_value.s, std::multiplies<T>());
    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N>
constexpr auto operator*(const T& scalar, const Vec<T,N>& vector) -> Vec<T,N>
{
    return Vec<T,N>(scalar) * vector;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto operator*(const Vec<T,N>& vector, const T& scalar) -> Vec<T,N>
{
    return scalar * vector;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator/(const Vec& other) const -> Vec
{
    Vec result = *this;
    return result /= other;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator/=(const Vec& other) -> Vec&
{
    std::transform(m_value.s, m_value.s+N, other.m_value.s, m_value.s, std::divides<T>());
    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N>
constexpr auto operator/(const T& scalar, const Vec<T,N>& vector) -> Vec<T,N>
{
    return Vec<T,N>(scalar) / vector;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto operator/(const Vec<T,N>& vector, const T& scalar) -> Vec<T,N>
{
    return scalar / vector;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator==(const Vec& other) const -> bool
{
    for (size_t i = 0; i < N; ++i)
    {
        if (m_value.s[i] != other.m_value.s[i])
            return false;
    }

    return true;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::operator!=(const Vec& other) const -> bool
{
    return !operator==(other);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::at(size_t n) -> T&
{
    ASSERT(n < N);
    return m_value.s[n];
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::at(size_t n) const -> T
{
    ASSERT(n < N);
    return m_value.s[n];
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::sqrt(const Vec& v) -> Vec
{
    constexpr auto sqrt = [](const T& arg){ return std::sqrt(arg); };

    Vec result;
    std::transform(v.m_value.s, v.m_value.s+N, result.m_value.s, sqrt);

    return result;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T, size_t N, typename E>
constexpr auto Vec<T,N,E>::ln(const Vec& v) -> Vec
{
    constexpr auto ln = [](const T& arg){ return std::log(arg); };

    Vec result;
    std::transform(v.m_value.s, v.m_value.s+N, result.m_value.s, ln);

    return result;
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
