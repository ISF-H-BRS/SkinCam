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

#pragma once

#include "memorybuffer.h"

SKINCAM_BEGIN_NAMESPACE();

class ImageArchive
{
public:
    ImageArchive(const std::string& filename);
    ImageArchive(const MemoryBuffer1w& dark,
                 const MemoryBuffer1w& channel0,
                 const MemoryBuffer1w& channel1,
                 const MemoryBuffer1w& channel2);

    auto getDark() const -> const MemoryBuffer1w&;
    auto getChannel0() const -> const MemoryBuffer1w&;
    auto getChannel1() const -> const MemoryBuffer1w&;
    auto getChannel2() const -> const MemoryBuffer1w&;

    void save(const std::string& filename) const;

private:
    std::unique_ptr<MemoryBuffer1w> m_dark;
    std::unique_ptr<MemoryBuffer1w> m_channel0;
    std::unique_ptr<MemoryBuffer1w> m_channel1;
    std::unique_ptr<MemoryBuffer1w> m_channel2;
};

SKINCAM_END_NAMESPACE();
