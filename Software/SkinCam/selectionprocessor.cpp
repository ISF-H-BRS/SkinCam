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
#include "selectionprocessor.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

static
auto extractValues(const MemoryBuffer3f& image, const std::vector<Vec2i>& coords)
{
    std::vector<Vec3f> values;

    for (const auto& coord : coords)
        values.push_back(image.at(static_cast<uint>(coord.x()), static_cast<uint>(coord.y())));

    return values;
}

// ---------------------------------------------------------------------------------------------- //

static
auto computeNormalizedDifferences(const std::vector<Vec3f>& values)
{
    std::vector<Vec3f> diffs;
    diffs.reserve(values.size());

    for (const Vec3f& value : values)
        diffs.push_back(Global::getNormalizedDifferences(value));

    return diffs;
}

// ---------------------------------------------------------------------------------------------- //

static
auto computeMean(const std::vector<Vec3f>& values) -> Vec3f
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    for (const auto& value : values)
    {
        x += static_cast<double>(value.x());
        y += static_cast<double>(value.y());
        z += static_cast<double>(value.z());
    }

    const auto count = static_cast<double>(values.size());

    return Vec3f(static_cast<float>(x / count),
                 static_cast<float>(y / count),
                 static_cast<float>(z / count));
}

// ---------------------------------------------------------------------------------------------- //

static
auto computeVariance(const std::vector<Vec3f>& values, const Vec3f& mean) -> Vec3f
{
    const auto meanX = static_cast<double>(mean.x());
    const auto meanY = static_cast<double>(mean.y());
    const auto meanZ = static_cast<double>(mean.z());

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    for (const auto& value : values)
    {
        auto diffX = static_cast<double>(value.x()) - meanX;
        auto diffY = static_cast<double>(value.y()) - meanY;
        auto diffZ = static_cast<double>(value.z()) - meanZ;

        x += diffX*diffX;
        y += diffY*diffY;
        z += diffZ*diffZ;
    }

    const auto count = static_cast<double>(values.size());

    return Vec3f(static_cast<float>(x / count),
                 static_cast<float>(y / count),
                 static_cast<float>(z / count));
}

// ---------------------------------------------------------------------------------------------- //

auto SelectionProcessor::get(const MemoryBuffer3f& image, const std::vector<Polygon>& selection,
                             const PixelExport::Selection& fields) -> PixelExport::Data
{
    std::vector<Vec2i> coords;
    std::vector<Vec3f> values;
    std::vector<Vec3f> diffs;

    if (fields.coordinates || fields.values || fields.normalizedDifferences)
        coords = getCoords(selection);

    if (fields.values || fields.normalizedDifferences)
        values = extractValues(image, coords);

    if (fields.normalizedDifferences)
        diffs = computeNormalizedDifferences(values);

    PixelExport::Data data;

    if (fields.coordinates)
        data.coordinates = std::move(coords);

    if (fields.values)
        data.values = std::move(values);

    if (fields.normalizedDifferences)
        data.normalizedDifferences = std::move(diffs);

    return data;
}

// ---------------------------------------------------------------------------------------------- //

auto SelectionProcessor::getCoords(const std::vector<Polygon>& selection) -> std::vector<Vec2i>
{
    const auto getBoundingBox = [&]
    {
        Vec2i min(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
        Vec2i max(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

        for (const auto& polygon : selection)
        {
            for (const auto& vertex : polygon.vertices())
            {
                const int minX = std::min(min.x(), vertex.x());
                const int minY = std::min(min.y(), vertex.y());
                min = Vec2i(minX, minY);

                const int maxX = std::max(max.x(), vertex.x());
                const int maxY = std::max(max.y(), vertex.y());
                max = Vec2i(maxX, maxY);
            }
        }

        return std::make_tuple(min, max);
    };

    const auto pixelContained = [&](int x, int y)
    {
        for (const auto& polygon : selection)
        {
            if (polygon.contains(x, y))
                return true;
        }

        return false;
    };

    const auto [min, max] = getBoundingBox();

    std::vector<Vec2i> coords;

    for (int y = min.y(); y < max.y(); ++y)
    {
        for (int x = min.x(); x < max.x(); ++x)
        {
            if (pixelContained(x, y))
                coords.emplace_back(x, y);
        }
    }

    return coords;
}

// ---------------------------------------------------------------------------------------------- //

auto SelectionProcessor::getValues(const MemoryBuffer3f& image,
                                   const std::vector<Polygon>& selection) -> std::vector<Vec3f>
{
    const std::vector<Vec2i> coords = getCoords(selection);
    return extractValues(image, coords);
}

// ---------------------------------------------------------------------------------------------- //

auto SelectionProcessor::getNormalizedDifferences(const MemoryBuffer3f& image,
                                                  const std::vector<Polygon>& selection)
                                                                            -> std::vector<Vec3f>
{
    const std::vector<Vec3f> values = getValues(image, selection);
    return computeNormalizedDifferences(values);
}

// ---------------------------------------------------------------------------------------------- //

auto SelectionProcessor::getAverage(const MemoryBuffer3f& image,
                                    const std::vector<Polygon>& selection) -> Vec3f
{
    const std::vector<Vec3f> values = getValues(image, selection);
    return computeMean(values);
}

// ---------------------------------------------------------------------------------------------- //

auto SelectionProcessor::getNormDiffMeanAndVariance(const MemoryBuffer3f& image,
                                                    const std::vector<Polygon>& selection)
                                                                        -> std::tuple<Vec3f,Vec3f>
{
    const std::vector<Vec3f> diffs = getNormalizedDifferences(image, selection);

    const Vec3f mean = computeMean(diffs);
    const Vec3f variance = computeVariance(diffs, mean);

    return {
        mean,
        variance
    };
}

// ---------------------------------------------------------------------------------------------- //
