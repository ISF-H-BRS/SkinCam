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

#include "clheader.h"
#include "openclexception.h"

#include <map>
#include <mutex>

SKINCAM_BEGIN_NAMESPACE();

class OpenCLWrapper
{
public:
    OpenCLWrapper(const std::string& platformFilter = "", const std::string& deviceFilter = "");
    ~OpenCLWrapper();

    auto platformName() const -> std::string;
    auto deviceName() const -> std::string;

    auto getPlatforms() -> std::vector<cl::Platform>;
    void getDevices(std::vector<cl::Device>* allDevices,
                    std::vector<cl::Device>* gpuDevices,
                    std::vector<cl::Device>* cpuDevices);

    auto getKernel(const std::string& name,
                   const std::vector<std::string>& defines) const -> cl::Kernel;

    auto createBuffer(cl_mem_flags flags, uint size) const -> cl::Buffer;
    auto createImage(cl_mem_flags flags, uint width, uint height,
                     const cl::ImageFormat& format) const -> cl::Image2D;

    void enqueueKernel(const cl::Kernel& kernel, const cl::NDRange& range) const;

    void enqueueWriteBuffer(const cl::Buffer& buffer, uint size, const void* ptr) const;
    void enqueueReadBuffer(const cl::Buffer& buffer, uint size, void* ptr) const;
    void enqueueCopyBuffer(const cl::Buffer& src, const cl::Buffer& dst, uint size) const;

    auto enqueueMapBuffer(const cl::Buffer& buffer, uint size) const -> void*;
    void enqueueUnmapBuffer(const cl::Buffer& buffer, void* ptr) const;

    void enqueueWriteImage(const cl::Image2D& image, uint width, uint height,
                           const void* ptr) const;
    void enqueueReadImage(const cl::Image2D& image, uint width, uint height, void* ptr) const;
    void enqueueCopyImage(const cl::Image2D& src, const cl::Image2D& dst,
                          uint width, uint height) const;

    auto enqueueMapImage(const cl::Image2D& image, uint width, uint height) const -> void*;
    void enqueueUnmapImage(const cl::Image2D& image, void* ptr) const;

    void enqueueCopyImageToBuffer(const cl::Image2D& image, const cl::Buffer& buffer,
                                  uint width, uint height) const;
    void enqueueCopyBufferToImage(const cl::Buffer& buffer, const cl::Image2D& image,
                                  uint width, uint height) const;

    static auto getInstance() -> const OpenCLWrapper&;

private:
    enum class Vendor
    {
        Intel,
        AMD,
        NVIDIA,
        Other
    };

    static constexpr size_t VendorCount = 4;

private:
    auto findPlatform(const std::string& filter) -> cl::Platform;
    auto findDevice(const std::string& filter) -> cl::Device;

    auto buildProgram(const std::string& name,
                      const std::vector<std::string>& defines) const -> cl::Program;

    static auto findVendor(const cl::Platform& platform, const cl::Device& device) -> Vendor;

private:
    cl::Platform m_platform;
    cl::Device m_device;
    cl::Context m_context;
    cl::CommandQueue m_queue;

    Vendor m_vendor = Vendor::Other;

    mutable std::mutex m_mutex;
    mutable std::map<std::string, cl::Program> m_programs;

    static OpenCLWrapper* s_instance;
};

SKINCAM_END_NAMESPACE();
