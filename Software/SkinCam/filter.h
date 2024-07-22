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

#include "filters/filterworker.h"
#include "global.h"

#include <string>
#include <vector>

SKINCAM_BEGIN_NAMESPACE();

class Filter
{
public:
    virtual ~Filter();

    auto getName() const -> std::string;

    void setEnabled(bool enable);
    auto isEnabled() const -> bool;

    virtual auto createWorker() -> std::unique_ptr<FilterWorker> = 0;

protected:
    Filter(const char* name);

    void update();

private:
    friend class FilterWorker;
    void registerWorker(FilterWorker* worker);
    void unregisterWorker(FilterWorker* worker);

private:
    std::string m_name;
    bool m_enabled = false;

    std::vector<FilterWorker*> m_workers;
};

SKINCAM_END_NAMESPACE();
