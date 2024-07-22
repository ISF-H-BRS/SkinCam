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

#include "runfilters.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

void RunFilters::operator()(const ComputeBuffer3f& input, ComputeBuffer3f* output,
                            const FilterList& filters)
{
    ComputeBuffer3f* in = &m_workBuffer;
    ComputeBuffer3f* out = output;

    bool first = true;

    for (auto& filter : filters)
    {
        if (filter->isEnabled())
        {
            if (first)
            {
                filter->run(input, out);
                first = false;
            }
            else
                filter->run(*in, out);

            std::swap(in, out);
        }
    }

    if (first)
        input.copyTo(output);
    else if (in != output)
        in->swap(output);
}

// ---------------------------------------------------------------------------------------------- //
