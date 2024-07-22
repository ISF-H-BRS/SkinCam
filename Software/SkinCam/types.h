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

#include "clheader.h"
#include "namespace.h"

using uchar = unsigned char;
using uint = unsigned int;
using ushort = unsigned short;

SKINCAM_BEGIN_NAMESPACE();

template <typename T, size_t N>
struct ClType {};

template <> struct ClType<char, 1> { using Type = cl_char;  };
template <> struct ClType<char, 2> { using Type = cl_char2; };
template <> struct ClType<char, 3> { using Type = cl_char3; };
template <> struct ClType<char, 4> { using Type = cl_char4; };

template <> struct ClType<uchar, 1> { using Type = cl_uchar;  };
template <> struct ClType<uchar, 2> { using Type = cl_uchar2; };
template <> struct ClType<uchar, 3> { using Type = cl_uchar3; };
template <> struct ClType<uchar, 4> { using Type = cl_uchar4; };

template <> struct ClType<short, 1> { using Type = cl_short;  };
template <> struct ClType<short, 2> { using Type = cl_short2; };
template <> struct ClType<short, 3> { using Type = cl_short3; };
template <> struct ClType<short, 4> { using Type = cl_short4; };

template <> struct ClType<ushort, 1> { using Type = cl_ushort;  };
template <> struct ClType<ushort, 2> { using Type = cl_ushort2; };
template <> struct ClType<ushort, 3> { using Type = cl_ushort3; };
template <> struct ClType<ushort, 4> { using Type = cl_ushort4; };

template <> struct ClType<int, 1> { using Type = cl_int;  };
template <> struct ClType<int, 2> { using Type = cl_int2; };
template <> struct ClType<int, 3> { using Type = cl_int3; };
template <> struct ClType<int, 4> { using Type = cl_int4; };

template <> struct ClType<uint, 1> { using Type = cl_uint;  };
template <> struct ClType<uint, 2> { using Type = cl_uint2; };
template <> struct ClType<uint, 3> { using Type = cl_uint3; };
template <> struct ClType<uint, 4> { using Type = cl_uint4; };

template <> struct ClType<float, 1> { using Type = cl_float;  };
template <> struct ClType<float, 2> { using Type = cl_float2; };
template <> struct ClType<float, 3> { using Type = cl_float3; };
template <> struct ClType<float, 4> { using Type = cl_float4; };

template <> struct ClType<double, 1> { using Type = cl_double;  };
template <> struct ClType<double, 2> { using Type = cl_double2; };
template <> struct ClType<double, 3> { using Type = cl_double3; };
template <> struct ClType<double, 4> { using Type = cl_double4; };

SKINCAM_END_NAMESPACE();
