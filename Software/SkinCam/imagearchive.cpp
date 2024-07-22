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

#include "global.h"
#include "image.h"
#include "imagearchive.h"

#ifdef _WIN32
#define ZIP_EXTERN
#endif

#include <zip.h>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

using ZipGuard = std::unique_ptr<zip, decltype(&zip_close)>;

// ---------------------------------------------------------------------------------------------- //

static
auto readFile(zip* handle, const std::string& filename) -> MemoryBuffer1w
{
    zip_flags_t flags = 0;

    struct zip_stat stat = {};
    zip_stat_init(&stat);

    if (zip_stat(handle, filename.c_str(), flags, &stat) < 0)
        throw Exception("Unable to find file " + filename + " in image archive.");

    zip_file* file = zip_fopen(handle, filename.c_str(), flags);

    if (!file)
        throw Exception("Unable to open file " + filename + " from image archive.");

    std::vector<uchar> buffer(stat.size);

    zip_int64_t read = zip_fread(file, buffer.data(), stat.size);

    zip_fclose(file);

    if (read != static_cast<zip_int64_t>(stat.size))
        throw Exception("Failed to read content of file " + filename + " from image archive.");

    Image image(buffer.data(), buffer.size());

    if (image.width() != ImageWidth || image.height() != ImageHeight)
    {
        throw Exception("Image in archive has invalid size. All images must have "
                        + std::to_string(ImageWidth) + "×"
                        + std::to_string(ImageHeight) + " pixels.");
    }

    return image.toBuffer<Vec1w>();
}

// ---------------------------------------------------------------------------------------------- //

static
void writeFile(zip* handle, const MemoryBuffer1w& buffer, const std::string& filename)
{
    Image image = Image::fromBuffer(buffer);
    std::vector<uchar> data = image.save(Image::FileType::PNG);

    // Data must remain valid until source is freed on zip_close()
    auto ptr = reinterpret_cast<uchar*>(std::malloc(data.size()));
    std::copy_n(data.data(), data.size(), ptr);

    // Let libzip free data
    const int freep = true;

    zip_source_t* source = zip_source_buffer(handle, ptr, data.size(), freep);

    if (!source)
    {
        std::free(ptr);
        throw Exception("Unable to allocate memory for archive entry.");
    }

    if (zip_file_add(handle, filename.c_str(), source, ZIP_FL_ENC_GUESS) < 0)
    {
        zip_source_free(source);
        throw Exception("Unable to add image file to archive.");
    }
}

// ---------------------------------------------------------------------------------------------- //

ImageArchive::ImageArchive(const std::string& filename)
{
    int error = 0;

    zip* handle = zip_open(filename.c_str(), ZIP_RDONLY, &error);

    if (!handle)
        throw Exception("Unable to open image archive \"" + filename + "\".");

    ZipGuard guard(handle, zip_close);

    m_dark = std::make_unique<MemoryBuffer1w>(readFile(handle, "dark.png"));
    m_channel0 = std::make_unique<MemoryBuffer1w>(readFile(handle, "channel0.png"));
    m_channel1 = std::make_unique<MemoryBuffer1w>(readFile(handle, "channel1.png"));
    m_channel2 = std::make_unique<MemoryBuffer1w>(readFile(handle, "channel2.png"));
}

// ---------------------------------------------------------------------------------------------- //

ImageArchive::ImageArchive(const MemoryBuffer1w& dark, const MemoryBuffer1w& channel0,
                           const MemoryBuffer1w& channel1, const MemoryBuffer1w& channel2)
    : m_dark(std::make_unique<MemoryBuffer1w>(dark)),
      m_channel0(std::make_unique<MemoryBuffer1w>(channel0)),
      m_channel1(std::make_unique<MemoryBuffer1w>(channel1)),
      m_channel2(std::make_unique<MemoryBuffer1w>(channel2))
{
}

// ---------------------------------------------------------------------------------------------- //

auto ImageArchive::getDark() const -> const MemoryBuffer1w&
{
    return *m_dark;
}

// ---------------------------------------------------------------------------------------------- //

auto ImageArchive::getChannel0() const -> const MemoryBuffer1w&
{
    return *m_channel0;
}

// ---------------------------------------------------------------------------------------------- //

auto ImageArchive::getChannel1() const -> const MemoryBuffer1w&
{
    return *m_channel1;
}

// ---------------------------------------------------------------------------------------------- //

auto ImageArchive::getChannel2() const -> const MemoryBuffer1w&
{
    return *m_channel2;
}

// ---------------------------------------------------------------------------------------------- //

void ImageArchive::save(const std::string& filename) const
{
    int error = 0;

    zip* handle = zip_open(filename.c_str(), ZIP_CREATE, &error);

    if (!handle)
        throw Exception("Unable to create image archive \"" + filename + "\".");

    ZipGuard guard(handle, zip_close);

    writeFile(handle, *m_dark, "dark.png");
    writeFile(handle, *m_channel0, "channel0.png");
    writeFile(handle, *m_channel1, "channel1.png");
    writeFile(handle, *m_channel2, "channel2.png");
}

// ---------------------------------------------------------------------------------------------- //
