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

#include "boundingboxbackend.h"
#include "skinclassifier.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

BoundingBoxBackend::BoundingBoxBackend(SkinClassifier* classifier)
{
    update(classifier);
}

// ---------------------------------------------------------------------------------------------- //

void BoundingBoxBackend::update(SkinClassifier* classifier)
{
    // Scale multiplier to get results comparable to gaussian-threshold classifier
    static constexpr float ScalingMultiplier = 0.6F * ClassifierBackend::ScalingMultiplier;

    const Vec3f& mean = classifier->getMean();

    const Vec3f variance =
            ScalingMultiplier * classifier->getScalingFactor() * classifier->getVariance();

    // Compute non-uniform bounding-box bounds based on threshold by solving
    // the unscaled gaussian density function p = exp(-1/2 * (x/σ)²) for x.
    const Vec3f bound = Vec3f::sqrt(2.0F * std::log(1.0F / classifier->getThreshold()) * variance);

    m_lower = mean - bound;
    m_upper = mean + bound;
}

// ---------------------------------------------------------------------------------------------- //

void BoundingBoxBackend::process(const InputBuffer& input, OutputBuffer* output)
{
    threshold(input, output, m_lower, m_upper, 1.0F, 0.0F);
}

// ---------------------------------------------------------------------------------------------- //
