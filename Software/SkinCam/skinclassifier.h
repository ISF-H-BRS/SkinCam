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

#include "frame.h"
#include "skinclassifier/skinclassifierworker.h"

SKINCAM_BEGIN_NAMESPACE();

class SkinClassifier
{
public:
    enum class Method
    {
        Gaussian,
        GaussianThreshold,
        BoundingBox
    };

    static constexpr int MinimumScaling = -100;
    static constexpr int MaximumScaling =  100;
    static constexpr int DefaultScaling =    0;

    static constexpr float MinimumThreshold = 0.0F;
    static constexpr float MaximumThreshold = 1.0F;
    static constexpr float DefaultThreshold = 0.1F;

public:
    SkinClassifier(Method method);
    ~SkinClassifier();

    void setMethod(Method method);
    auto getMethod() const -> Method;

    void setParameters(const Vec3f& mean, const Vec3f& variance);
    auto getMean() const -> const Vec3f&;
    auto getVariance() const -> const Vec3f&;

    void setScaling(int scaling);
    auto getScaling() const -> int;

    void setThreshold(float threshold);
    auto getThreshold() const -> float;

    auto getScalingFactor() const -> float;

    void setRegionOfInterest(const std::vector<Vec2i>& vertices);
    auto getRegionOfInterest() const -> const std::vector<Vec2i>&;

    auto createWorker() -> std::unique_ptr<SkinClassifierWorker>;

private:
    void update();

    friend class SkinClassifierWorker;
    void registerWorker(SkinClassifierWorker* worker);
    void unregisterWorker(SkinClassifierWorker* worker);

private:
    Method m_method;

    Vec3f m_mean;
    Vec3f m_variance;

    int m_scaling = DefaultScaling;
    float m_threshold = DefaultThreshold;

    float m_scalingFactor = 1.0F;

    std::vector<Vec2i> m_regionOfInterest;

    std::vector<SkinClassifierWorker*> m_workers;
};

SKINCAM_END_NAMESPACE();
