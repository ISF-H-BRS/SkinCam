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

#include "computebuffer.h"
#include "global.h"

#include <mutex>

SKINCAM_BEGIN_NAMESPACE();

class Filter;

class FilterWorker
{
public:
    using Buffer = ComputeBuffer3f;

public:
    FilterWorker(Filter* filter);
    virtual ~FilterWorker();

    auto getFilter() const -> Filter*;

    auto isEnabled() const -> bool;

    void update();
    void run(const Buffer& input, Buffer* output);

protected:
    virtual void updateImpl() = 0;
    virtual void runImpl(const Buffer& input, Buffer* output) = 0;

private:
    Filter* m_filter;
    std::mutex m_mutex;
};

SKINCAM_END_NAMESPACE();
