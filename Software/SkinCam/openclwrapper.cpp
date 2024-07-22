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

#include "assertions.h"
#include "logger.h"
#include "openclwrapper.h"
#include "program_map.h"

#include <cassert>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

OpenCLWrapper* OpenCLWrapper::s_instance = nullptr;

// ---------------------------------------------------------------------------------------------- //

OpenCLWrapper::OpenCLWrapper(const std::string& platformFilter, const std::string& deviceFilter)
{
    ASSERT_NULL(s_instance);

    Logger::info("Initializing OpenCL subsystem.");

    m_platform = findPlatform(platformFilter);
    Logger::info("Using platform " + m_platform.getInfo<CL_PLATFORM_NAME>() + "\n");

    m_device = findDevice(deviceFilter);
    Logger::info("Using device " + m_device.getInfo<CL_DEVICE_NAME>() + "\n");

    m_context = cl::Context(m_device);
    m_queue = cl::CommandQueue(m_context, m_device);

    m_vendor = findVendor(m_platform, m_device);

    s_instance = this;
}

// ---------------------------------------------------------------------------------------------- //

OpenCLWrapper::~OpenCLWrapper()
{
    ASSERT(s_instance == this);
    s_instance = nullptr;
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::platformName() const -> std::string
{
    return m_platform.getInfo<CL_PLATFORM_NAME>();
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::deviceName() const -> std::string
{
    return m_device.getInfo<CL_DEVICE_NAME>();
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::getPlatforms() -> std::vector<cl::Platform>
{
    std::vector<cl::Platform> platforms;

    try {
        cl::Platform::get(&platforms);
    }
    catch (const cl::Error& e) {
        throw OpenCLException("Unable to retrieve available OpenCL platforms. Please make sure "
                              "that the correct drivers for your system are installed.", e);
    }

    // Skip platforms that are known to have bugs (only one so far)
    static const std::vector<const char*> unsupportedPlatforms = {
        "Clover"
    };

    static const auto predicate = [&](const cl::Platform& platform)
    {
        const auto& platforms = unsupportedPlatforms;
        const auto name = platform.getInfo<CL_PLATFORM_NAME>();

        if (std::find(platforms.begin(), platforms.end(), name) != platforms.end())
        {
            Logger::info("Skipping unsupported platform \"" + name + "\".\n");
            return true;
        }

        return false;
    };

    std::erase_if(platforms, predicate);

    if (platforms.empty())
        throw Exception("No OpenCL platforms found.");

    return platforms;
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::getDevices(std::vector<cl::Device>* allDevices,
                               std::vector<cl::Device>* gpuDevices,
                               std::vector<cl::Device>* cpuDevices)
{
    ASSERT_NOT_NULL(gpuDevices);
    ASSERT_NOT_NULL(cpuDevices);

    std::vector<cl::Device> devices;

    try {
        m_platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    }
    catch (const cl::Error& e) {
        throw OpenCLException("Unable to retrieve available OpenCL devices.", e);
    }

    if (devices.empty())
        throw Exception("No OpenCL devices found.");

    for (const auto& device : devices)
    {
        auto type = device.getInfo<CL_DEVICE_TYPE>();

        if (type == CL_DEVICE_TYPE_GPU)
            gpuDevices->push_back(device);
        else
            cpuDevices->push_back(device);

        allDevices->push_back(device);
    }
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::getKernel(const std::string& name,
                              const std::vector<std::string>& defines) const -> cl::Kernel
{
    std::string key = name;
    for (const auto& define : defines)
        key += "//" + define;

    auto it = m_programs.find(key);

    if (it == m_programs.end())
        m_programs[key] = buildProgram(name, defines);

    return cl::Kernel(m_programs.at(key), name.c_str());
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::createBuffer(cl_mem_flags flags, uint size) const -> cl::Buffer
{
    return cl::Buffer(m_context, flags, size);
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::createImage(cl_mem_flags flags, uint width, uint height,
                                const cl::ImageFormat& format) const -> cl::Image2D
{
    return cl::Image2D(m_context, flags, format, width, height);
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::enqueueKernel(const cl::Kernel& kernel, const cl::NDRange& range) const
{
    std::lock_guard lock(m_mutex);
    m_queue.enqueueNDRangeKernel(kernel, cl::NullRange, range, cl::NullRange);
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::enqueueWriteBuffer(const cl::Buffer& buffer, uint size, const void* ptr) const
{
    std::lock_guard lock(m_mutex);
    m_queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, size, ptr);
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::enqueueReadBuffer(const cl::Buffer& buffer, uint size, void* ptr) const
{
    std::lock_guard lock(m_mutex);
    m_queue.enqueueReadBuffer(buffer, CL_TRUE, 0, size, ptr);
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::enqueueCopyBuffer(const cl::Buffer& src, const cl::Buffer& dst, uint size) const
{
    std::lock_guard lock(m_mutex);
    m_queue.enqueueCopyBuffer(src, dst, 0, 0, size);
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::enqueueMapBuffer(const cl::Buffer& buffer, uint size) const -> void*
{
    std::lock_guard lock(m_mutex);
    return m_queue.enqueueMapBuffer(buffer, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, size);
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::enqueueUnmapBuffer(const cl::Buffer& buffer, void* ptr) const
{
    std::lock_guard lock(m_mutex);
    m_queue.enqueueUnmapMemObject(buffer, ptr);
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::enqueueWriteImage(const cl::Image2D& image, uint width, uint height,
                                      const void* ptr) const
{
    std::lock_guard lock(m_mutex);
    m_queue.enqueueWriteImage(image, CL_TRUE, { 0, 0, 0 }, { width, height, 0 }, 0, 0, ptr);
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::enqueueReadImage(const cl::Image2D& image, uint width, uint height,
                                     void* ptr) const
{
    std::lock_guard lock(m_mutex);
    m_queue.enqueueReadImage(image, CL_TRUE, { 0, 0, 0 }, { width, height, 0 }, 0, 0, ptr);
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::enqueueCopyImage(const cl::Image2D& src, const cl::Image2D& dst,
                                     uint width, uint height) const
{
    std::lock_guard lock(m_mutex);
    m_queue.enqueueCopyImage(src, dst, { 0, 0, 0 }, { 0, 0, 0 }, { width, height, 0 });
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::enqueueMapImage(const cl::Image2D& image,
                                    uint width, uint height) const -> void*
{
    std::lock_guard lock(m_mutex);
    return m_queue.enqueueMapImage(image, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE,
                                   { 0, 0, 0 }, { width, height, 0 }, nullptr, nullptr);
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::enqueueUnmapImage(const cl::Image2D& image, void* ptr) const
{
    std::lock_guard lock(m_mutex);
    m_queue.enqueueUnmapMemObject(image, ptr);
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::enqueueCopyImageToBuffer(const cl::Image2D& image, const cl::Buffer& buffer,
                                             uint width, uint height) const
{
    std::lock_guard lock(m_mutex);
    m_queue.enqueueCopyImageToBuffer(image, buffer, { 0, 0, 0 }, { width, height, 0 }, 0);
}

// ---------------------------------------------------------------------------------------------- //

void OpenCLWrapper::enqueueCopyBufferToImage(const cl::Buffer& buffer, const cl::Image2D& image,
                                             uint width, uint height) const
{
    std::lock_guard lock(m_mutex);
    m_queue.enqueueCopyBufferToImage(buffer, image, 0, { 0, 0, 0 }, { width, height, 0 });
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::getInstance() -> const OpenCLWrapper&
{
    ASSERT_NOT_NULL(s_instance);
    return *s_instance;
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::findPlatform(const std::string& filter) -> cl::Platform
{
    std::vector<cl::Platform> platforms = getPlatforms();

    Logger::info("Available OpenCL platforms:");

    for (const auto& platform : platforms)
        Logger::info("* " + platform.getInfo<CL_PLATFORM_NAME>());

    for (const auto& platform : platforms)
    {
        std::string name = platform.getInfo<CL_PLATFORM_NAME>();

        if (name.find(filter) != std::string::npos)
            return platform;
    }

    if (!filter.empty())
        Logger::info("No platform matching \"" + filter + "\" found.");

    return platforms.at(0);
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::findDevice(const std::string& filter) -> cl::Device
{
    std::vector<cl::Device> allDevices;
    std::vector<cl::Device> gpuDevices;
    std::vector<cl::Device> cpuDevices;

    getDevices(&allDevices, &gpuDevices, &cpuDevices);

    Logger::info("Available OpenCL devices:");

    for (const auto& device : allDevices)
    {
        bool imageSupport = device.getInfo<CL_DEVICE_IMAGE_SUPPORT>();

        Logger::info("* " + device.getInfo<CL_DEVICE_NAME>() +
                     " (image support: " + (imageSupport ? "yes" : "no") + ")");
    }

    for (const auto& device : gpuDevices)
    {
        std::string name = device.getInfo<CL_DEVICE_NAME>();

        if (name.find(filter) != std::string::npos)
            return device;
    }

    for (const auto& device : cpuDevices)
    {
        std::string name = device.getInfo<CL_DEVICE_NAME>();

        if (name.find(filter) != std::string::npos)
            return device;
    }

    if (!filter.empty())
        Logger::info("No device matching \"" + filter + "\" found.");

    if (!gpuDevices.empty())
        return gpuDevices.at(0);

    ASSERT(!cpuDevices.empty());
    return cpuDevices.at(0);
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::buildProgram(const std::string& name,
                                 const std::vector<std::string>& defines) const -> cl::Program
{
    static const std::array<std::string, VendorCount> vendors = {
        "SKINCAM_VENDOR_INTEL",
        "SKINCAM_VENDOR_AMD",
        "SKINCAM_VENDOR_NVIDIA",
        "SKINCAM_VENDOR_OTHER"
    };

    auto it = program_map.find(name + ".cl");
    ASSERT(it != program_map.end());

    std::string options;
    for (const auto& define : defines)
        options += "-D " + define + " ";

    options += "-D " + vendors.at(static_cast<size_t>(m_vendor));

    cl::Program program(m_context, it->second);

    try {
        program.build(options.c_str());
    }
    catch (const cl::Error& e)
    {
        const std::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_device);

        Logger::error("Error building program \"" + name + ".cl\". Build log: " + log);
        throw OpenCLException("Error building program \"" + name + ".cl\".", e);
    }

    Logger::info("Program \"" + name + "\" successfully built.");

    return program;
}

// ---------------------------------------------------------------------------------------------- //

auto OpenCLWrapper::findVendor(const cl::Platform& platform, const cl::Device& device) -> Vendor
{
    static const auto contains = [](std::string string, std::string pattern)
    {
        static const auto tolower = [](unsigned char c){ return std::tolower(c); };
        std::transform(string.begin(), string.end(), string.begin(), tolower);
        std::transform(pattern.begin(), pattern.end(), pattern.begin(), tolower);

        return string.find(pattern) != std::string::npos;
    };

    const std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
    const std::string deviceName = device.getInfo<CL_DEVICE_NAME>();

    if (contains(platformName, "Intel") || contains(deviceName, "Intel"))
        return Vendor::Intel;

    if (contains(platformName, "AMD") || contains(deviceName, "AMD"))
        return Vendor::AMD;

    if (contains(platformName, "NVIDIA") || contains(deviceName, "NVIDIA"))
        return Vendor::AMD;

    return Vendor::Other;
}

// ---------------------------------------------------------------------------------------------- //
