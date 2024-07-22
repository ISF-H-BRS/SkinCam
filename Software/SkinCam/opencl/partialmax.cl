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

// T, N

// ---------------------------------------------------------------------------------------------- //

#define CONCAT2X(a,b) a##b
#define CONCAT2(a,b) CONCAT2X(a,b)

// ---------------------------------------------------------------------------------------------- //

#if N == 1
#define TYPE T
#else
#define TYPE CONCAT2(T, N)
#endif

// ---------------------------------------------------------------------------------------------- //

void kernel partialmax(global const TYPE* input,
                       global TYPE* output,
                       const uint stride)
{
    const uint index = get_global_id(0);

    TYPE maxValue = (TYPE)(0);

    for (uint i = 0; i < stride; ++i)
    {
        TYPE value = input[index * stride + i];

#if N == 1
        maxValue = max(value, maxValue);
#elif N >= 2
        maxValue.x = max(value.x, maxValue.x);
        maxValue.y = max(value.y, maxValue.y);
 #if N >= 3
        maxValue.z = max(value.z, maxValue.z);
  #if N >= 4
        maxValue.w = max(value.w, maxValue.w);
  #endif // 4
 #endif // 3
#endif // 1,2
    }

    output[index] = maxValue;
}

// ---------------------------------------------------------------------------------------------- //
