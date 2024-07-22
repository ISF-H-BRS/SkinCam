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

// TYPE, N

// ---------------------------------------------------------------------------------------------- //

#define CONCAT2X(a,b) a##b
#define CONCAT2(a,b) CONCAT2X(a,b)

// ---------------------------------------------------------------------------------------------- //

#define OUTTYPE CONCAT2(TYPE, N)

// ---------------------------------------------------------------------------------------------- //

#if (N == 2)
void kernel merge(global const TYPE* input0,
                  global const TYPE* input1,
                  global OUTTYPE* output)
{
    const int id = get_global_id(0);
    output[id] = (OUTTYPE)(input0[id], input1[id]);
}
#endif

// ---------------------------------------------------------------------------------------------- //

#if (N == 3)
void kernel merge(global const TYPE* input0,
                  global const TYPE* input1,
                  global const TYPE* input2,
                  global OUTTYPE* output)
{
    const int id = get_global_id(0);
    output[id] = (OUTTYPE)(input0[id], input1[id], input2[id]);
}
#endif

// ---------------------------------------------------------------------------------------------- //

#if (N == 4)
void kernel merge(global const TYPE* input0,
                  global const TYPE* input1,
                  global const TYPE* input2,
                  global const TYPE* input3,
                  global OUTTYPE* output)
{
    const int id = get_global_id(0);
    output[id] = (OUTTYPE)(input0[id], input1[id], input2[id], input3[id]);
}
#endif

// ---------------------------------------------------------------------------------------------- //
