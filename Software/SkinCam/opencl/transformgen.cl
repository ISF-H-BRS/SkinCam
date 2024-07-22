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

void kernel transformgen(global float2* output,
                         float e00, float e01, float e02,
                         float e10, float e11, float e12)
{
    const uint c = get_global_id(0);
    const uint r = get_global_id(1);

    const uint width = get_global_size(0);
    const uint height = get_global_size(1);

    const float x = (float)c;
    const float y = (float)r;

    float x_out = e00*x + e01*y + e02;
    float y_out = e10*x + e11*y + e12;

    output[r * width + c] = (float2)(x_out, y_out);
}
