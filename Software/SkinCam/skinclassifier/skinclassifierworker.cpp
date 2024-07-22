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

#include "skinclassifierworker.h"
#include "../skinclassifier.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

SkinClassifierWorker::SkinClassifierWorker(SkinClassifier* classifier)
    : m_classifier(classifier),
      m_boundingBoxBackend(std::make_unique<BoundingBoxBackend>(classifier)),
      m_gaussianBackend(std::make_unique<GaussianBackend>(classifier)),
      m_gaussianThresholdBackend(std::make_unique<GaussianThresholdBackend>(classifier))
{
    ASSERT_NOT_NULL(classifier);
    m_classifier->registerWorker(this);

    update();
}

// ---------------------------------------------------------------------------------------------- //

SkinClassifierWorker::~SkinClassifierWorker()
{
    m_classifier->unregisterWorker(this);
}

// ---------------------------------------------------------------------------------------------- //

void SkinClassifierWorker::update()
{
    std::lock_guard lock(m_mutex);

    if (m_classifier->getMethod() == SkinClassifier::Method::Gaussian)
        m_currentBackend = m_gaussianBackend.get();
    else if (m_classifier->getMethod() == SkinClassifier::Method::GaussianThreshold)
        m_currentBackend = m_gaussianThresholdBackend.get();
    else
        m_currentBackend = m_boundingBoxBackend.get();

    m_threshold = m_classifier->getThreshold();

    const std::vector<Vec2i>& roi = m_classifier->getRegionOfInterest();

    if (roi != m_regionOfInterest)
    {
        m_regionOfInterest = roi;

        if (m_regionOfInterest.empty())
            m_roiArea = ImageArea;
        else
            updateRegionOfInterest();
    }

    m_currentBackend->update(m_classifier);
}

// ---------------------------------------------------------------------------------------------- //

void SkinClassifierWorker::run(const InputBuffer& input, OutputBuffer* output, double* skinRatio)
{
    ASSERT_NOT_NULL(m_currentBackend);

    std::lock_guard lock(m_mutex);

    normDiff(input, &m_normalizedDifferences);
    m_currentBackend->process(m_normalizedDifferences, output);

    uint count = 0;

    if (m_regionOfInterest.empty())
        count = countSkinPixels(*output, m_threshold);
    else
        count = countSkinPixels(*output, m_roiMask, m_threshold);

    *skinRatio = 1.0 / m_roiArea * count;
}

// ---------------------------------------------------------------------------------------------- //

void SkinClassifierWorker::updateRegionOfInterest()
{
    ASSERT(!m_regionOfInterest.empty());

    const uint roiSize = static_cast<uint>(m_regionOfInterest.size());

    if (roiSize == m_roiPolygon.pixelCount())
        m_roiPolygon.copyFrom(m_regionOfInterest.data());
    else
        m_roiPolygon = ComputeBuffer2i(m_regionOfInterest, roiSize, 1, CL_MEM_READ_ONLY);

    generateMask(m_roiPolygon, &m_roiMask);
    m_roiArea = summatePixels(m_roiMask);
}

// ---------------------------------------------------------------------------------------------- //
