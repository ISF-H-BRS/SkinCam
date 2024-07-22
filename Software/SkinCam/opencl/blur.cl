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

#define FLOAT CONCAT2(float, N)
#define TYPE  CONCAT2(T, N)

// ---------------------------------------------------------------------------------------------- //

#if (T == float)
#define CONVERT_FLOAT
#define CONVERT_TYPE
#else
#define CONVERT_FLOAT CONCAT2(convert_, FLOAT)
#define CONVERT_TYPE  CONCAT2(convert_, TYPE)
#endif

// ---------------------------------------------------------------------------------------------- //

void kernel blur(global const TYPE* input,
                 global TYPE* output,
                 global const float* weights,
                 const int radius)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);

    const int width = get_global_size(0);
    const int height = get_global_size(1);

    const int index = y * width + x;

    FLOAT value = (FLOAT)(0.0F);
    float totalWeight = 0.0F;

    for (int dy = -radius; dy <= radius; ++dy)
    {
        const float yWeight = weights[abs(dy)];
        const int yCoord = y + dy;

        if (yCoord < 0 || yCoord >= height)
            continue;

        for (int dx = -radius; dx <= radius; ++dx)
        {
            const float xWeight = weights[abs(dx)];
            const int xCoord = x + dx;

            if (xCoord < 0 || xCoord >= width)
                continue;

            const float weight = xWeight * yWeight;

            value += weight * CONVERT_FLOAT(input[yCoord * width + xCoord]);
            totalWeight += weight;
        }
    }

#ifdef SKINCAM_VENDOR_INTEL
    if (radius == 0xdeadbeef)         // This code will never execute
        output[index] = input[index]; // but works around a weird bug
#endif

    const FLOAT result = value / totalWeight;
    output[index] = CONVERT_TYPE(result);
}

// ---------------------------------------------------------------------------------------------- //
