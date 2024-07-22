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

#include "countskinpixels.h"
#include "global.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

auto CountSkinPixels::operator()(const InputBuffer& buffer, float threshold_) -> uint
{
    return process(buffer, nullptr, threshold_);
}

// ---------------------------------------------------------------------------------------------- //

auto CountSkinPixels::operator()(const InputBuffer& buffer, const MaskBuffer& mask_,
                                 float threshold_) -> uint
{
    return process(buffer, &mask_, threshold_);
}

// ---------------------------------------------------------------------------------------------- //

auto CountSkinPixels::process(const InputBuffer& buffer, const MaskBuffer* mask_,
                              float threshold_) -> uint
{
    threshold(buffer, &m_skinMask, threshold_, 1.0F, 1, static_cast<uchar>(0));

    if (mask_)
        mask(m_skinMask, &m_skinMask, *mask_);

    return summate(m_skinMask);
}

// ---------------------------------------------------------------------------------------------- //
