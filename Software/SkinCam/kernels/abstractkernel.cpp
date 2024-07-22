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

#include "abstractkernel.h"
#include "openclwrapper.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

AbstractKernel::AbstractKernel(const std::string& name, const std::vector<std::string>& defines)
    : m_kernel(OpenCLWrapper::getInstance().getKernel(name, defines))
{
}

// ---------------------------------------------------------------------------------------------- //

void AbstractKernel::enqueue(uint count)
{
    enqueue(cl::NDRange(count));
}

// ---------------------------------------------------------------------------------------------- //

void AbstractKernel::enqueue(uint width, uint height)
{
    enqueue(cl::NDRange(width, height));
}

// ---------------------------------------------------------------------------------------------- //

void AbstractKernel::enqueue(const cl::NDRange& range)
{
    OpenCLWrapper::getInstance().enqueueKernel(m_kernel, range);
}

// ---------------------------------------------------------------------------------------------- //
