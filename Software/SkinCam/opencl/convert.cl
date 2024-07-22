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

// IN, OUT, N, SAT

// ---------------------------------------------------------------------------------------------- //

#define CONCAT2X(a,b) a##b
#define CONCAT2(a,b) CONCAT2X(a,b)

// ---------------------------------------------------------------------------------------------- //

#define CONCAT3X(a,b,c) a##b##c
#define CONCAT3(a,b,c) CONCAT3X(a,b,c)

// ---------------------------------------------------------------------------------------------- //

#define CONCAT4X(a,b,c,d) a##b##c##d
#define CONCAT4(a,b,c,d) CONCAT4X(a,b,c,d)

// ---------------------------------------------------------------------------------------------- //

#define FLOAT_TYPE      CONCAT2(float, N)
#define CONVERT_FLOAT   CONCAT2(convert_float, N)
#define CONVERT_OUT     CONCAT3(convert_, OUT, SAT)

// ---------------------------------------------------------------------------------------------- //

void kernel convert(global const IN* input,
                    global OUT* output,
                    float alpha, float beta)
{
    const int index = get_global_id(0);

    const FLOAT_TYPE v = CONVERT_FLOAT(input[index]);
    const FLOAT_TYPE a = (FLOAT_TYPE)(alpha);
    const FLOAT_TYPE b = (FLOAT_TYPE)(beta);

    const FLOAT_TYPE result = a * v + b;
    output[index] = CONVERT_OUT(result);
}

// ---------------------------------------------------------------------------------------------- //
