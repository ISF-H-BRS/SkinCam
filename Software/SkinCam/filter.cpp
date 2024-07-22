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

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

Filter::Filter(const char* name)
    : m_name(name)
{
}

// ---------------------------------------------------------------------------------------------- //

Filter::~Filter()
{
    ASSERT(m_workers.empty());
}

// ---------------------------------------------------------------------------------------------- //

auto Filter::getName() const -> std::string
{
    return m_name;
}

// ---------------------------------------------------------------------------------------------- //

void Filter::setEnabled(bool enable)
{
    m_enabled = enable;
}

// ---------------------------------------------------------------------------------------------- //

auto Filter::isEnabled() const -> bool
{
    return m_enabled;
}

// ---------------------------------------------------------------------------------------------- //

void Filter::update()
{
    for (auto worker : m_workers)
        worker->update();
}

// ---------------------------------------------------------------------------------------------- //

void Filter::registerWorker(FilterWorker* worker)
{
    ASSERT(std::find(m_workers.begin(), m_workers.end(), worker) == m_workers.end());
    m_workers.push_back(worker);
}

// ---------------------------------------------------------------------------------------------- //

void Filter::unregisterWorker(FilterWorker* worker)
{
    auto it = std::find(m_workers.begin(), m_workers.end(), worker);
    ASSERT(it != m_workers.end());

    m_workers.erase(it);
}

// ---------------------------------------------------------------------------------------------- //
