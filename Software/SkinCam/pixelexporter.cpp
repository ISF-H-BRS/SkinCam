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
#include "exception.h"
#include "global.h"
#include "pixelexporter.h"

#include <fstream>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

static
void writeHeader(std::ofstream& file,
                 const std::vector<Vec2i>& coords,
                 const std::vector<Vec3f>& values,
                 const std::vector<Vec3f>& diffs)
{
    static constexpr const char* Channel0 = Global::getString(Channel::Wavelength0);
    static constexpr const char* Channel1 = Global::getString(Channel::Wavelength1);
    static constexpr const char* Channel2 = Global::getString(Channel::Wavelength2);

    if (!coords.empty())
    {
        file << "Row;Column";

        if (!values.empty() || !diffs.empty())
            file << ";";
    }

    if (!values.empty())
    {
        file << Channel0 << ";" << Channel1 << ";" << Channel2;

        if (!diffs.empty())
            file << ";";
    }

    if (!diffs.empty())
    {
        file << Channel0 << " - " << Channel1 << ";"
             << Channel0 << " - " << Channel2 << ";"
             << Channel1 << " - " << Channel2;
    }

    file << "\n";

    if (file.fail())
        throw Exception("Writing to data file failed.");
}

// ---------------------------------------------------------------------------------------------- //

static
void writeLines(std::ofstream& file,
               const std::vector<Vec2i>& coords,
               const std::vector<Vec3f>& values,
               const std::vector<Vec3f>& diffs)
{
    const auto hasCoord = [&](size_t index) {
        return index < coords.size();
    };

    const auto hasValue = [&](size_t index) {
        return index < values.size();
    };

    const auto hasDiff = [&](size_t index) {
        return index < diffs.size();
    };

    size_t size = std::max(coords.size(), values.size());
    size = std::max(size, diffs.size());

    for (size_t i = 0; i < size; ++i)
    {
        if (hasCoord(i))
        {
            const Vec2i& coord = coords[i];
            file << coord.y() << ";" << coord.x();

            if (hasValue(i) || hasDiff(i))
                file << ";";
        }

        if (hasValue(i))
        {
            const Vec3f& value = values[i];
            file << value.x() << ";" << value.y() << ";" << value.z();

            if (hasDiff(i))
                file << ";";
        }

        if (hasDiff(i))
        {
            const Vec3f& diff = diffs[i];
            file << diff.x()  << ";" << diff.y()  << ";" << diff.z();
        }

        file << "\n";

        if (file.fail())
            throw Exception("Writing to data file failed.");
    }
}

// ---------------------------------------------------------------------------------------------- //

void PixelExporter::write(const std::string& filename, const Data& data)
{
    std::ofstream file(filename);

    if (!file.is_open())
        throw Exception("Unable to open file " + filename + " for writing.");

    const std::vector<Vec2i>& coords = data.coordinates;
    const std::vector<Vec3f>& values = data.values;
    const std::vector<Vec3f>& diffs = data.normalizedDifferences;

    writeHeader(file, coords, values, diffs);
    writeLines(file, coords, values, diffs);
}

// ---------------------------------------------------------------------------------------------- //
