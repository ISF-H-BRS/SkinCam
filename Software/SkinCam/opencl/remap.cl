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

// TYPE, N, UNIFORM|SEPARATE

// ---------------------------------------------------------------------------------------------- //

#define CONCAT2X(a,b) a##b
#define CONCAT2(a,b) CONCAT2X(a,b)

// ---------------------------------------------------------------------------------------------- //

#define CONVERT_FLOAT CONCAT2(convert_float, N)
#define CONVERT_TYPE CONCAT2(convert_, TYPE)
#define FLOAT_TYPE CONCAT2(float, N)

// ---------------------------------------------------------------------------------------------- //

FLOAT_TYPE sample(global const TYPE* input, int x, int y, uint width, uint height)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return (FLOAT_TYPE)(0);

    return CONVERT_FLOAT(input[y * width + x]);
}

// ---------------------------------------------------------------------------------------------- //

TYPE lookup(global const TYPE* input, float2 coords, uint width, uint height)
{
    const int2 lookup0 = convert_int2(coords);
    const int2 lookup1 = lookup0 + (int2)(1, 1);

    const float2 factors = coords - convert_float2(lookup0);

    FLOAT_TYPE sample00 = sample(input, lookup0.x, lookup0.y, width, height);
    FLOAT_TYPE sample01 = sample(input, lookup1.x, lookup0.y, width, height);
    FLOAT_TYPE sample10 = sample(input, lookup0.x, lookup1.y, width, height);
    FLOAT_TYPE sample11 = sample(input, lookup1.x, lookup1.y, width, height);

    FLOAT_TYPE mix0 = mix(sample00, sample01, factors.x);
    FLOAT_TYPE mix1 = mix(sample10, sample11, factors.x);

    return CONVERT_TYPE(mix(mix0, mix1, factors.y));
}

// ---------------------------------------------------------------------------------------------- //

#ifdef UNIFORM
void kernel remap(global const TYPE* input,
                  global TYPE* output,
                  global const float2* map,
                  uint inputWidth,
                  uint inputHeight)
{
    const int index = get_global_id(0);
    output[index] = lookup(input, map[index], inputWidth, inputHeight);
}
#endif

// ---------------------------------------------------------------------------------------------- //

#ifdef SEPARATE
void kernel remap(global const TYPE* input,
                  global TYPE* output,
                  global const float2* map0,
                  global const float2* map1,
                  global const float2* map2,
                  uint inputWidth,
                  uint inputHeight)
{
    const int index = get_global_id(0);

    const float2 coords0 = map0[index];
    const float2 coords1 = map1[index];
    const float2 coords2 = map2[index];

    const TYPE value0 = lookup(input, coords0, inputWidth, inputHeight);
    const TYPE value1 = lookup(input, coords1, inputWidth, inputHeight);
    const TYPE value2 = lookup(input, coords2, inputWidth, inputHeight);

    output[index] = (TYPE)(value0.x, value1.y, value2.z);
}
#endif

// ---------------------------------------------------------------------------------------------- //
