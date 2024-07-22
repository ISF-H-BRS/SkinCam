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

#include <array>
#include <cmath>

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

template <typename T>
class Matrix3
{
public:
    constexpr Matrix3();
    constexpr Matrix3(T e00, T e01, T e02,
                      T e10, T e11, T e12,
                      T e20, T e21, T e22);

    constexpr auto operator*(const Matrix3& m) const -> Matrix3;

    template <typename U>
    constexpr friend auto operator*(U s, const Matrix3<U>& m) -> Matrix3<U>;

    constexpr auto operator*(T s) const -> Matrix3;
    constexpr auto operator*=(T s) -> Matrix3&;

    constexpr auto operator/(T s) const -> Matrix3;
    constexpr auto operator/=(T s) -> Matrix3&;

    constexpr auto getAdjugate() const -> Matrix3;
    constexpr auto getDeterminant() const -> T;
    constexpr auto getInverse() const -> Matrix3;

    constexpr auto invert() -> Matrix3&;

    constexpr auto at(size_t n) const -> T;

    constexpr operator const T*() const;
    constexpr operator T*();

    static constexpr auto null() -> Matrix3;
    static constexpr auto identity() -> Matrix3;

    static constexpr auto scale(T x, T y) -> Matrix3;
    static constexpr auto rotate(T radians) -> Matrix3;
    static constexpr auto translate(T x, T y) -> Matrix3;

private:
    struct Elements
    {
        T _00;
        T _01;
        T _02;

        T _10;
        T _11;
        T _12;

        T _20;
        T _21;
        T _22;
    };

    union {
        std::array<T, 3*3> m_data;
        Elements m;
    };
};

// ---------------------------------------------------------------------------------------------- //

using Matrix3f = Matrix3<float>;
using Matrix3d = Matrix3<double>;

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr Matrix3<T>::Matrix3()
    : m_data({ T(1), T(0), T(0),
               T(0), T(1), T(0),
               T(0), T(0), T(1) }) {}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr Matrix3<T>::Matrix3(T e00, T e01, T e02,
                              T e10, T e11, T e12,
                              T e20, T e21, T e22)
    : m_data({ e00, e01, e02,
               e10, e11, e12,
               e20, e21, e22 }) {}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::operator*(const Matrix3& other) const -> Matrix3
{
    return {
        m._00*other.m._00 + m._01*other.m._10 + m._02*other.m._20,
        m._00*other.m._01 + m._01*other.m._11 + m._02*other.m._21,
        m._00*other.m._02 + m._01*other.m._12 + m._02*other.m._22,

        m._10*other.m._00 + m._11*other.m._10 + m._12*other.m._20,
        m._10*other.m._01 + m._11*other.m._11 + m._12*other.m._21,
        m._10*other.m._02 + m._11*other.m._12 + m._12*other.m._22,

        m._20*other.m._00 + m._21*other.m._10 + m._22*other.m._20,
        m._20*other.m._01 + m._21*other.m._11 + m._22*other.m._21,
        m._20*other.m._02 + m._21*other.m._12 + m._22*other.m._22
    };
}

// ---------------------------------------------------------------------------------------------- //

template <typename U>
constexpr auto operator*(U s, const Matrix3<U>& matrix) -> Matrix3<U>
{
    return {
        s * matrix.m._00, s * matrix.m._01, s * matrix.m._02,
        s * matrix.m._10, s * matrix.m._11, s * matrix.m._12,
        s * matrix.m._20, s * matrix.m._21, s * matrix.m._22
    };
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::operator*(T s) const -> Matrix3
{
    return {
        s * m._00, s * m._01, s * m._02,
        s * m._10, s * m._11, s * m._12,
        s * m._20, s * m._21, s * m._22
    };
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::operator*=(T s) -> Matrix3&
{
    m._00 *= s;
    m._01 *= s;
    m._02 *= s;

    m._10 *= s;
    m._11 *= s;
    m._12 *= s;

    m._20 *= s;
    m._21 *= s;
    m._22 *= s;

    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::operator/(T s) const -> Matrix3
{
    s = T(1) / s;
    return operator*(s);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::operator/=(T s) -> Matrix3&
{
    s = T(1) / s;
    return operator*=(s);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::getAdjugate() const -> Matrix3
{
    return {
        m._11*m._22 - m._21*m._12, m._02*m._21 - m._01*m._22, m._01*m._12 - m._02*m._11,
        m._12*m._20 - m._10*m._22, m._00*m._22 - m._02*m._20, m._02*m._10 - m._00*m._12,
        m._10*m._21 - m._11*m._20, m._01*m._20 - m._00*m._21, m._00*m._11 - m._01*m._10
    };
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::getDeterminant() const -> T
{
    return m._00*m._11*m._22 + m._01*m._12*m._20 + m._02*m._10*m._21
         - m._02*m._11*m._20 - m._01*m._10*m._22 - m._00*m._12*m._21;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::getInverse() const -> Matrix3
{
    return getAdjugate() / getDeterminant();
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::invert() -> Matrix3&
{
    *this = getInverse();
    return *this;
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::at(size_t n) const -> T
{
    return m_data.at(n);
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr Matrix3<T>::operator const T*() const
{
    return m_data.data();
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr Matrix3<T>::operator T*()
{
    return m_data.data();
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::null() -> Matrix3
{
    constexpr T nul = T(0);

    return {
        nul, nul, nul,
        nul, nul, nul,
        nul, nul, nul
    };
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::identity() -> Matrix3
{
    constexpr T nul = T(0);
    constexpr T one = T(1);

    return {
        one, nul, nul,
        nul, one, nul,
        nul, nul, one
    };
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::scale(T x, T y) -> Matrix3
{
    constexpr T nul = T(0);
    constexpr T one = T(1);

    return {
        x  , nul, nul,
        nul, y  , nul,
        nul, nul, one
    };
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::rotate(T radians) -> Matrix3
{
    constexpr T nul = T(0);
    constexpr T one = T(1);

    const T sin = std::sin(radians);
    const T cos = std::cos(radians);

    return {
        cos, -sin, nul,
        sin,  cos, nul,
        nul,  nul, one
    };
}

// ---------------------------------------------------------------------------------------------- //

template <typename T>
constexpr auto Matrix3<T>::translate(T x, T y) -> Matrix3
{
    constexpr T nul = T(0);
    constexpr T one = T(1);

    return {
        one, nul, x,
        nul, one, y,
        nul, nul, one
    };
}

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
