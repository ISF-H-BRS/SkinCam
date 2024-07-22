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

// IN

// ---------------------------------------------------------------------------------------------- //

float3 _normdiff(const float3 input)
{
    return (float3)((input.x - input.y) / (input.x + input.y),
                    (input.x - input.z) / (input.x + input.z),
                    (input.y - input.z) / (input.y + input.z));
}

// ---------------------------------------------------------------------------------------------- //

void kernel normdiff(global const IN* input,
                     global float3* output)
{
    const int index = get_global_id(0);

    const float3 value = convert_float3(input[index]);
    output[index] = _normdiff(value);
}

// ---------------------------------------------------------------------------------------------- //
