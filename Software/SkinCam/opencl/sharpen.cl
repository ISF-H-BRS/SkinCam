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

#if (N > 1)
#define ALL all
#else
#define ALL
#endif

// ---------------------------------------------------------------------------------------------- //

void kernel sharpen(global const TYPE* input,
                    global TYPE* output,
                    global const TYPE* blurred,
                    const TYPE threshold,
                    const float amount)
{
    const int index = get_global_id(0);

    const FLOAT diff = CONVERT_FLOAT(input[index] - blurred[index]);

    FLOAT result = CONVERT_FLOAT(input[index]);

    if (ALL(fabs(diff) > CONVERT_FLOAT(threshold)))
        result += amount * diff;

    output[index] = CONVERT_TYPE(result);
}

// ---------------------------------------------------------------------------------------------- //
