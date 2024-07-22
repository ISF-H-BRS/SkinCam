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

#include "filter.h"
#include "filterworker.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

FilterWorker::FilterWorker(Filter* filter)
    : m_filter(filter)
{
    ASSERT_NOT_NULL(filter);
    m_filter->registerWorker(this);
}

// ---------------------------------------------------------------------------------------------- //

FilterWorker::~FilterWorker()
{
    m_filter->unregisterWorker(this);
}

// ---------------------------------------------------------------------------------------------- //

auto FilterWorker::getFilter() const -> Filter*
{
    return m_filter;
}

// ---------------------------------------------------------------------------------------------- //

auto FilterWorker::isEnabled() const -> bool
{
    return m_filter->isEnabled();
}

// ---------------------------------------------------------------------------------------------- //

void FilterWorker::update()
{
    std::lock_guard lock(m_mutex);
    updateImpl();
}

// ---------------------------------------------------------------------------------------------- //

void FilterWorker::run(const Buffer& input, Buffer* output)
{
    std::lock_guard lock(m_mutex);
    runImpl(input, output);
}

// ---------------------------------------------------------------------------------------------- //
