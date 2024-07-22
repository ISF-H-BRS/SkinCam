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

// IN, N

// ---------------------------------------------------------------------------------------------- //

#if (N == 4)
#define REDUCE(v) (v.x * v.y * v.z * v.w)
#elif (N == 3)
#define REDUCE(v) (v.x * v.y * v.z)
#elif (N == 2)
#define REDUCE(v) (v.x * v.y)
#else
#define REDUCE(v) (v)
#endif

// ---------------------------------------------------------------------------------------------- //

void kernel gaussian(global const IN* input,
                     global float* output,
                     const IN mean,
                     const IN variance)
{
    const int index = get_global_id(0);

    const IN diff = input[index] - mean;
    const IN scale = (IN)(-0.5F) / variance;
    const IN result = exp(diff * diff * scale);

    output[index] = REDUCE(result);
}

// ---------------------------------------------------------------------------------------------- //
