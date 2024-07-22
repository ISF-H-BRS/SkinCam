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

#include "assertions.h"
#include "computebuffer.h"
#include "openclexception.h"
#include "vectors.h"

SKINCAM_BEGIN_NAMESPACE();

class AbstractKernel
{
protected:
    AbstractKernel(const std::string& name, const std::vector<std::string>& defines = {});

    template <typename T>
    void setArg(cl_uint index, const ComputeBuffer<T>& buffer)
    {
        setArgOrThrow([&]{ m_kernel.setArg(index, buffer.buffer()); });
    }

    template <typename T>
    void setArg(cl_uint index, ComputeBuffer<T>* buffer)
    {
        setArgOrThrow([&]{ m_kernel.setArg(index, buffer->buffer()); });
    }

    template <typename T>
    void setArg(cl_uint index, const T& arg)
    {
        setArgOrThrow([&]{ m_kernel.setArg(index, arg); });
    }

    template <typename T>
    void setArg(cl_uint index, const std::vector<T>& arg)
    {
        setArgOrThrow([&]{ m_kernel.setArg(index, arg.size() * sizeof(T), arg.data()); });
    }

    void enqueue(uint count);
    void enqueue(uint width, uint height);
    void enqueue(const cl::NDRange& range);

private:
    void setArgOrThrow(const std::function<void()>& func)
    {
        try {
            func();
        }
        catch (const cl::Error& e) {
            throw OpenCLException("Unable to set kernel argument.", e);
        }
    }

private:
    cl::Kernel m_kernel;
};

SKINCAM_END_NAMESPACE();
