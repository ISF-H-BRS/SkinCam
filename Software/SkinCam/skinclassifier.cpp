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

#include "skinclassifier.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

SkinClassifier::SkinClassifier(Method method)
    : m_method(method)
{
}

// ---------------------------------------------------------------------------------------------- //

SkinClassifier::~SkinClassifier()
{
    ASSERT(m_workers.empty());
}

// ---------------------------------------------------------------------------------------------- //

void SkinClassifier::setMethod(Method method)
{
    m_method = method;
    update();
}

// ---------------------------------------------------------------------------------------------- //

auto SkinClassifier::getMethod() const -> Method
{
    return m_method;
}

// ---------------------------------------------------------------------------------------------- //

void SkinClassifier::setParameters(const Vec3f& mean, const Vec3f& variance)
{
    m_mean = mean;
    m_variance = variance;

    update();
}

// ---------------------------------------------------------------------------------------------- //

auto SkinClassifier::getMean() const -> const Vec3f&
{
    return m_mean;
}

// ---------------------------------------------------------------------------------------------- //

auto SkinClassifier::getVariance() const -> const Vec3f&
{
    return m_variance;
}

// ---------------------------------------------------------------------------------------------- //

void SkinClassifier::setScaling(int scaling)
{
    CLAMP_TO_RANGE(scaling, MinimumScaling, MaximumScaling);

    m_scaling = scaling;

    if (m_scaling >= 0)
        m_scalingFactor = 1.0F + (static_cast<float>(m_scaling) * 0.04F);
    else
        m_scalingFactor = 1.0F + (static_cast<float>(m_scaling) * 0.01F);

    update();
}

// ---------------------------------------------------------------------------------------------- //

auto SkinClassifier::getScaling() const -> int
{
    return m_scaling;
}

// ---------------------------------------------------------------------------------------------- //

void SkinClassifier::setThreshold(float threshold)
{
    CLAMP_TO_RANGE(threshold, MinimumThreshold, MaximumThreshold);

    m_threshold = threshold;
    update();
}

// ---------------------------------------------------------------------------------------------- //

auto SkinClassifier::getThreshold() const -> float
{
    return m_threshold;
}

// ---------------------------------------------------------------------------------------------- //

auto SkinClassifier::getScalingFactor() const -> float
{
    return m_scalingFactor;
}

// ---------------------------------------------------------------------------------------------- //

void SkinClassifier::setRegionOfInterest(const std::vector<Vec2i>& vertices)
{
    m_regionOfInterest = vertices;
    update();
}

// ---------------------------------------------------------------------------------------------- //

auto SkinClassifier::getRegionOfInterest() const -> const std::vector<Vec2i>&
{
    return m_regionOfInterest;
}

// ---------------------------------------------------------------------------------------------- //

auto SkinClassifier::createWorker() -> std::unique_ptr<SkinClassifierWorker>
{
    return std::make_unique<SkinClassifierWorker>(this);
}

// ---------------------------------------------------------------------------------------------- //

void SkinClassifier::update()
{
    for (auto worker : m_workers)
        worker->update();
}

// ---------------------------------------------------------------------------------------------- //

void SkinClassifier::registerWorker(SkinClassifierWorker* worker)
{
    ASSERT(std::find(m_workers.begin(), m_workers.end(), worker) == m_workers.end());
    m_workers.push_back(worker);
}

// ---------------------------------------------------------------------------------------------- //

void SkinClassifier::unregisterWorker(SkinClassifierWorker* worker)
{
    auto it = std::find(m_workers.begin(), m_workers.end(), worker);
    ASSERT(it != m_workers.end());

    m_workers.erase(it);
}

// ---------------------------------------------------------------------------------------------- //
