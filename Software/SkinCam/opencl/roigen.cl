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

void swap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// ---------------------------------------------------------------------------------------------- //

void kernel roigen(global const int2* roi,
                   global uchar* output,
                   const uint vertex_count)
{
    const int x = get_global_id(0);
    const int y = get_global_id(1);

    const int width = get_global_size(0);

    uint intersection_count = 0;

    for (uint i = 1; i <= vertex_count; ++i)
    {
        const int2 v1 = roi[i - 1];
        const int2 v2 = roi[i == vertex_count ? 0 : i];

        int y1 = v1.y;
        int y2 = v2.y;

        if (y1 == y2) // Skip horizontal edges
            continue;

        const bool need_swap = y2 < y1;

        if (need_swap)
            swap(&y1, &y2);

        if (y < y1 || y >= y2)
            continue;

        int x1 = v1.x;
        int x2 = v2.x;

        if (need_swap)
            swap(&x1, &x2);

        int intersection = x1 + (int)((float)(x2 - x1) / (y2 - y1) * (y - y1));

        if (intersection >= x)
            ++intersection_count;
    }

    output[y * width + x] = (intersection_count % 2 != 0) ? 1 : 0;
}

// ---------------------------------------------------------------------------------------------- //
