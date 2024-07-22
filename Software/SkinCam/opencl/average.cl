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

// T, N, COUNT

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

#if (COUNT == 2)
void kernel average(global const TYPE* input0,
                    global const TYPE* input1,
                    global TYPE* output)
{
    const int id = get_global_id(0);

    FLOAT sum = CONVERT_FLOAT(input0[id]) + CONVERT_FLOAT(input1[id]);
    output[id] = CONVERT_TYPE(sum / COUNT);
}
#endif

// ---------------------------------------------------------------------------------------------- //

#if (COUNT == 3)
void kernel average(global const TYPE* input0,
                    global const TYPE* input1,
                    global const TYPE* input2,
                    global TYPE* output)
{
    const int id = get_global_id(0);

    FLOAT sum = CONVERT_FLOAT(input0[id]) + CONVERT_FLOAT(input1[id]) + CONVERT_FLOAT(input2[id]);
    output[id] = CONVERT_TYPE(sum / COUNT);
}
#endif

// ---------------------------------------------------------------------------------------------- //

#if (COUNT == 4)
void kernel average(global const TYPE* input0,
                    global const TYPE* input1,
                    global const TYPE* input2,
                    global const TYPE* input3,
                    global TYPE* output)
{
    const int id = get_global_id(0);

    FLOAT sum = CONVERT_FLOAT(input0[id]) + CONVERT_FLOAT(input1[id])
              + CONVERT_FLOAT(input2[id]) + CONVERT_FLOAT(input3[id]);
    output[id] = CONVERT_TYPE(sum / COUNT);
}
#endif

// ---------------------------------------------------------------------------------------------- //
