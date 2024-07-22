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

#include "image.h"
#include "vectors.h"

#include <FreeImagePlus.h>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

namespace {
    const std::array<uint, Image::FormatCount> g_channelCounts = {
        1, // Grayscale8,
        1, // Grayscale16,
        3, // RGB8,
        3, // RGB16,
        4, // RGBA8,
        4, // RGBA16
    };

    const std::array<uint, Image::FormatCount> g_pixelStrides = {
        1, // Grayscale8,
        1, // Grayscale16,
        4, // RGB8,
        4, // RGB16,
        4, // RGBA8,
        4, // RGBA16
    };

    const std::array<uint, Image::FormatCount> g_bytesPerChannel = {
        1, // Grayscale8,
        2, // Grayscale16,
        1, // RGB8,
        2, // RGB16,
        1, // RGBA8,
        2, // RGBA16
    };
}

// ---------------------------------------------------------------------------------------------- //

struct Image::ImageWrapper
{
    fipImage image;
};

// ---------------------------------------------------------------------------------------------- //

Image::Image(const std::string& filename)
{
    ImageWrapper wrapper;

    if (!wrapper.image.load(filename.c_str()))
        throw Exception("Unable to load image from file " + filename + ".");

    initialize(&wrapper);
}

// ---------------------------------------------------------------------------------------------- //

Image::Image(const uchar* buffer, size_t sizeInBytes)
{
    auto data = const_cast<uchar*>(buffer);
    auto size = static_cast<uint>(sizeInBytes);

    fipMemoryIO memory(data, size);

    ImageWrapper wrapper;

    if (!wrapper.image.loadFromMemory(memory))
        throw Exception("Unable to load image from memory buffer.");

    initialize(&wrapper);
}

// ---------------------------------------------------------------------------------------------- //

Image::Image(const uchar* data, uint width, uint height, Format format)
{
    m_info = makeInfo(width, height, format);

    m_data.resize(m_info.sizeInBytes);
    std::copy_n(data, m_info.sizeInBytes, m_data.data());
}

// ---------------------------------------------------------------------------------------------- //

void Image::initialize(ImageWrapper* image)
{
    m_info = parseImage(*image);

    if (m_info.format == Format::RGB8)
        image->image.convertTo32Bits();
    else if (m_info.format == Format::RGB16)
        image->image.convertToRGBA16();

    image->image.flipVertical();

    uchar* data = image->image.accessPixels();

    if (m_info.channelCount >= 3)
        swapRedAndBlue(data, m_info);

    m_data.resize(m_info.sizeInBytes);
    std::copy_n(data, m_info.sizeInBytes, m_data.data());
}

// ---------------------------------------------------------------------------------------------- //

auto Image::exportImage() const -> ImageWrapper
{
    ASSERT(m_data.size() == m_info.sizeInBytes);

    FREE_IMAGE_TYPE type = FIT_BITMAP;

    if (m_info.format == Format::Grayscale16)
        type = FIT_UINT16;
    else if (m_info.format == Format::RGB16)
        type = FIT_RGB16;
    else if (m_info.format == Format::RGBA16)
        type = FIT_RGBA16;

    ImageWrapper wrapper {
        fipImage(type, m_info.width, m_info.height,
                 m_info.pixelStride * m_info.bytesPerChannel * 8)
    };

    uchar* data = wrapper.image.accessPixels();
    std::copy_n(m_data.data(), m_data.size(), data);

    if (m_info.channelCount >= 3)
        swapRedAndBlue(data, m_info);

    if (m_info.format == Format::RGB8)
        wrapper.image.convertTo24Bits();
    else if (m_info.format == Format::RGB16)
        wrapper.image.convertToRGB16();

    wrapper.image.flipVertical();

    return wrapper;
}

// ---------------------------------------------------------------------------------------------- //

void Image::save(const std::string& filename) const
{
    ImageWrapper wrapper = exportImage();

    if (!wrapper.image.save(filename.c_str()))
        throw Exception("Unable to save image to file " + filename + ".");
}

// ---------------------------------------------------------------------------------------------- //

auto Image::save(FileType filetype) const -> std::vector<uchar>
{
    ImageWrapper wrapper = exportImage();

    FREE_IMAGE_FORMAT format = (filetype == FileType::PNG) ? FIF_PNG : FIF_JPEG;
    fipMemoryIO memory;

    BYTE* data = nullptr;
    DWORD sizeInBytes = 0;

    if (!wrapper.image.saveToMemory(format, memory) || !memory.acquire(&data, &sizeInBytes))
        throw Exception("Unable to save image to memory buffer.");

    std::vector<uchar> buffer(sizeInBytes);
    std::copy_n(data, sizeInBytes, buffer.data());

    return buffer;
}

// ---------------------------------------------------------------------------------------------- //

auto Image::width() const -> uint
{
    return m_info.width;
}

// ---------------------------------------------------------------------------------------------- //

auto Image::height() const -> uint
{
    return m_info.height;
}

// ---------------------------------------------------------------------------------------------- //

auto Image::format() const -> Format
{
    return m_info.format;
}

// ---------------------------------------------------------------------------------------------- //

auto Image::getInfo() const -> Info
{
    return m_info;
}

// ---------------------------------------------------------------------------------------------- //

auto Image::getSizeInBytes(uint width, uint height, Format format) -> uint
{
    size_t index = indexOf(format);
    return width * height * g_pixelStrides.at(index) * g_bytesPerChannel.at(index);
}

// ---------------------------------------------------------------------------------------------- //

auto Image::parseImage(const ImageWrapper& image) -> Info
{
    Info info;
    info.width = image.image.getWidth();
    info.height = image.image.getHeight();

    FREE_IMAGE_TYPE type = image.image.getImageType();
    uint bitsPerPixel = image.image.getBitsPerPixel();

    if (type == FIT_BITMAP && bitsPerPixel == 8)
    {
        info.channelCount = 1;
        info.pixelStride = 1;
        info.bytesPerChannel = 1;
        info.format = Format::Grayscale8;
    }
    else if (type == FIT_UINT16)
    {
        info.channelCount = 1;
        info.pixelStride = 1;
        info.bytesPerChannel = 2;
        info.format = Format::Grayscale16;
    }
    else if (type == FIT_BITMAP && bitsPerPixel == 24)
    {
        info.channelCount = 3;
        info.pixelStride = 4;
        info.bytesPerChannel = 1;
        info.format = Format::RGB8;
    }
    else if (type == FIT_RGB16)
    {
        info.channelCount = 3;
        info.pixelStride = 4;
        info.bytesPerChannel = 2;
        info.format = Format::RGB16;
    }
    else if (type == FIT_BITMAP && bitsPerPixel == 32)
    {
        info.channelCount = 4;
        info.pixelStride = 4;
        info.bytesPerChannel = 1;
        info.format = Format::RGBA8;
    }
    else if (type == FIT_RGBA16)
    {
        info.channelCount = 4;
        info.pixelStride = 4;
        info.bytesPerChannel = 2;
        info.format = Format::RGBA16;
    }
    else
        throw Exception("Only grayscale, RGB and RGBA images with "
                        "8 or 16 bits per channel are currently supported.");

    info.sizeInBytes = getSizeInBytes(info.width, info.height, info.format);
    return info;
}

// ---------------------------------------------------------------------------------------------- //

auto Image::makeInfo(uint width, uint height, Format format) -> Info
{
    size_t index = indexOf(format);

    return {
        width,
        height,
        format,
        g_channelCounts.at(index),
        g_pixelStrides.at(index),
        g_bytesPerChannel.at(index),
        getSizeInBytes(width, height, format)
    };
}

// ---------------------------------------------------------------------------------------------- //

void Image::swapRedAndBlue(uchar* data, const Info& info)
{
    ASSERT(info.channelCount >= 3);
    ASSERT(info.bytesPerChannel == 1 || info.bytesPerChannel == 2);

    if (info.bytesPerChannel == 1)
    {
        for (uint i = 0; i < info.width * info.height; ++i)
            std::swap(data[i * info.pixelStride + 0], data[i * info.pixelStride + 2]);
    }
    else
    {
        auto words = reinterpret_cast<ushort*>(data);

        for (uint i = 0; i < info.width * info.height; ++i)
            std::swap(words[i * info.pixelStride + 0], words[i * info.pixelStride + 2]);
    }
}

// ---------------------------------------------------------------------------------------------- //

#define IMAGE_SPECIALIZE_TO_BUFFER(FORMAT, TYPE)                                \
template <>                                                                     \
auto Image::toBuffer() const -> MemoryBuffer<TYPE>                              \
{                                                                               \
    if (m_info.format != Format::FORMAT)                                        \
        throw Exception("Cannot convert image to buffer of type " #TYPE ".");   \
                                                                                \
    auto data = reinterpret_cast<const TYPE*>(m_data.data());                   \
    return MemoryBuffer<TYPE>(data, m_info.width, m_info.height);               \
}                                                                               \

// ---------------------------------------------------------------------------------------------- //

IMAGE_SPECIALIZE_TO_BUFFER(Grayscale8,  Vec1b)
IMAGE_SPECIALIZE_TO_BUFFER(Grayscale16, Vec1w)
IMAGE_SPECIALIZE_TO_BUFFER(RGB8,        Vec3b)
IMAGE_SPECIALIZE_TO_BUFFER(RGB16,       Vec3w)
IMAGE_SPECIALIZE_TO_BUFFER(RGBA8,       Vec4b)
IMAGE_SPECIALIZE_TO_BUFFER(RGBA16,      Vec4w)

// ---------------------------------------------------------------------------------------------- //

#define IMAGE_SPECIALIZE_FROM_BUFFER(FORMAT, TYPE)                          \
template <>                                                                 \
auto Image::fromBuffer(const MemoryBuffer<TYPE>& buffer) -> Image           \
{                                                                           \
    auto data = reinterpret_cast<const uint8_t*>(buffer.data());            \
    return Image(data, buffer.width(), buffer.height(), Format::FORMAT);    \
}                                                                           \

// ---------------------------------------------------------------------------------------------- //

IMAGE_SPECIALIZE_FROM_BUFFER(Grayscale8,  Vec1b)
IMAGE_SPECIALIZE_FROM_BUFFER(Grayscale16, Vec1w)
IMAGE_SPECIALIZE_FROM_BUFFER(RGB8,        Vec3b)
IMAGE_SPECIALIZE_FROM_BUFFER(RGB16,       Vec3w)
IMAGE_SPECIALIZE_FROM_BUFFER(RGBA8,       Vec4b)
IMAGE_SPECIALIZE_FROM_BUFFER(RGBA16,      Vec4w)

// ---------------------------------------------------------------------------------------------- //
