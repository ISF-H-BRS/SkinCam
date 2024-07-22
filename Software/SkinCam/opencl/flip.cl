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

#define MODE_HORIZONTAL 0
#define MODE_VERTICAL   1
#define MODE_BOTH       2

void kernel flip(global const TYPE* input,
                 global TYPE* output,
                 const uint mode)
{
    const uint x = get_global_id(0);
    const uint y = get_global_id(1);

    const uint width = get_global_size(0);
    const uint height = get_global_size(1);

    const uint out_x = (mode == MODE_HORIZONTAL || mode == MODE_BOTH) ? (width  - 1 - x) : x;
    const uint out_y = (mode == MODE_VERTICAL   || mode == MODE_BOTH) ? (height - 1 - y) : y;

    output[out_y * width + out_x] = input[y * width + x];
}
