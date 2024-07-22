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

#include "convertimages.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

void ConvertImages::operator()(const InputBuffer& dark, const InputBuffer& channel0,
                               const InputBuffer& channel1, const InputBuffer& channel2,
                               RawBuffer* rawDark, RawBuffer* rawChannel0,
                               RawBuffer* rawChannel1, RawBuffer* rawChannel2,
                               CompositeBuffer* composite)
{
    dark.copyTo(&m_dark);
    channel0.copyTo(&m_channel0);
    channel1.copyTo(&m_channel1);
    channel2.copyTo(&m_channel2);

    merge(m_channel0, m_channel1, m_channel2, &m_merged);
    subtract(m_merged, m_dark, &m_subtracted);

    // 14-bit pixel format to 8-bit
    const float scaleRaw = 1.0F / (2<<14) * 255.0F;
    convertRaw(m_dark, rawDark, scaleRaw);
    convertRaw(m_channel0, rawChannel0, scaleRaw);
    convertRaw(m_channel1, rawChannel1, scaleRaw);
    convertRaw(m_channel2, rawChannel2, scaleRaw);

    // 14-bit pixel format to [0,1]
    const float scaleComposite = 1.0F / (2<<14);
    convertComposite(m_subtracted, composite, scaleComposite);
}

// ---------------------------------------------------------------------------------------------- //
