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

#include "gaussianthresholdbackend.h"
#include "../skinclassifier.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

GaussianThresholdBackend::GaussianThresholdBackend(SkinClassifier* classifier)
{
    update(classifier);
}

// ---------------------------------------------------------------------------------------------- //

void GaussianThresholdBackend::update(SkinClassifier* classifier)
{
    m_mean = classifier->getMean();
    m_variance = ScalingMultiplier * classifier->getScalingFactor() * classifier->getVariance();
    m_threshold = classifier->getThreshold();
}

// ---------------------------------------------------------------------------------------------- //

void GaussianThresholdBackend::process(const InputBuffer& input, OutputBuffer* output)
{
    gaussian(input, &m_buffer, m_mean, m_variance);
    threshold(m_buffer, output, m_threshold, 1.0F, 1.0F, 0.0F);
}

// ---------------------------------------------------------------------------------------------- //
