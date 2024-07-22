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

void kernel cropgen(global float2* output,
                    float scale,
                    float left,
                    float top,
                    float h_border,
                    float v_border)
{
    const uint c = get_global_id(0);
    const uint r = get_global_id(1);

    const uint width = get_global_size(0);
    const uint height = get_global_size(1);

    float x = -1.0F;
    float y = -1.0F;

    if (c >= h_border && c < width - h_border)
        x = (float)left + (float)(c - h_border) / scale;

    if (r >= v_border && r < height - v_border)
        y = (float)top + (float)(r - v_border) / scale;

    output[r * width + c] = (float2)(x, y);
}
