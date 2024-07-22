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

#include "exception.h"
#include "memorybuffer.h"

#include <string>

SKINCAM_BEGIN_NAMESPACE();

class Image
{
public:
    enum class Format
    {
        Grayscale8,
        Grayscale16,
        RGB8,
        RGB16,
        RGBA8,
        RGBA16,
        Invalid
    };

    static constexpr size_t FormatCount = 6;

    template <typename T = size_t>
    static auto indexOf(Format format)
    {
        ASSERT(format != Format::Invalid);
        return static_cast<T>(format);
    }

    struct Info
    {
        uint width = 0;
        uint height = 0;
        Format format = Format::Invalid;
        uint channelCount = 0;
        uint pixelStride = 0;
        uint bytesPerChannel = 0;
        uint sizeInBytes = 0;
    };

    enum FileType
    {
        PNG,
        JPG
    };

public:
    Image(const std::string& filename);
    Image(const uchar* buffer, size_t sizeInBytes);
    Image(const uchar* data, uint width, uint height, Format format);

    void save(const std::string& filename) const;
    auto save(FileType filetype) const -> std::vector<uchar>;

    auto width() const -> uint;
    auto height() const -> uint;
    auto format() const -> Format;

    auto getInfo() const -> Info;

    template <typename T>
    auto toBuffer() const -> MemoryBuffer<T>;

    template <typename T>
    static auto fromBuffer(const MemoryBuffer<T>& buffer) -> Image;

private:
    struct ImageWrapper;

    void initialize(ImageWrapper* image);
    auto exportImage() const -> ImageWrapper;

    static auto getSizeInBytes(uint width, uint height, Format format) -> uint;
    static auto parseImage(const ImageWrapper& image) -> Info;
    static auto makeInfo(uint width, uint height, Format format) -> Info;
    static void swapRedAndBlue(uchar* data, const Info& info);

private:
    Info m_info;
    std::vector<uchar> m_data;
};

SKINCAM_END_NAMESPACE();
