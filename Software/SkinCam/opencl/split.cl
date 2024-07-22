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

// IN, OUT, N

// ---------------------------------------------------------------------------------------------- //

#if (N == 2)
void kernel split(global const IN* input,
                  global OUT* output0,
                  global OUT* output1)
{
    const int id = get_global_id(0);
    output0[id] = input[id].x;
    output1[id] = input[id].y;
}
#endif

// ---------------------------------------------------------------------------------------------- //

#if (N == 3)
void kernel split(global const IN* input,
                  global OUT* output0,
                  global OUT* output1,
                  global OUT* output2)
{
    const int id = get_global_id(0);
    output0[id] = input[id].x;
    output1[id] = input[id].y;
    output2[id] = input[id].z;
}
#endif

// ---------------------------------------------------------------------------------------------- //

#if (N == 4)
void kernel split(global const IN* input,
                  global OUT* output0,
                  global OUT* output1,
                  global OUT* output2,
                  global OUT* output3)
{
    const int id = get_global_id(0);
    output0[id] = input[id].x;
    output1[id] = input[id].y;
    output2[id] = input[id].z;
    output3[id] = input[id].w;
}
#endif

// ---------------------------------------------------------------------------------------------- //
