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

#include "filterworker.h"
#include "kernels.h"
#include "transformationfilter.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class TransformationFilterWorker : public FilterWorker
{
public:
    TransformationFilterWorker(TransformationFilter* filter);

    void updateImpl() override;
    void runImpl(const Buffer& input, Buffer* output) override;

private:
    void updateTransform();

private:
    TransformGenKernel transformGen;
    RemapKernel<Vec3f, RemapMode::Uniform> remap;

private:
    TransformationFilter* m_filter;

    Matrix3f m_matrix = Matrix3f::identity();
    ComputeBuffer2f m_map = { ImageWidth, ImageHeight, CL_MEM_READ_WRITE };
};

// ---------------------------------------------------------------------------------------------- //

TransformationFilter::TransformationFilter()
    : Filter("TransformationFilter")
{
}

// ---------------------------------------------------------------------------------------------- //

void TransformationFilter::setMatrix(const Matrix3f& matrix)
{
    m_matrix = matrix;
    update();
}

// ---------------------------------------------------------------------------------------------- //

auto TransformationFilter::getMatrix() const -> const Matrix3f&
{
    return m_matrix;
}

// ---------------------------------------------------------------------------------------------- //

auto TransformationFilter::createWorker() -> std::unique_ptr<FilterWorker>
{
    return std::make_unique<TransformationFilterWorker>(this);
}

// ---------------------------------------------------------------------------------------------- //

TransformationFilterWorker::TransformationFilterWorker(TransformationFilter* filter)
    : FilterWorker(filter),
      m_filter(filter)
{
    m_matrix = m_filter->getMatrix();
    updateTransform();
}

// ---------------------------------------------------------------------------------------------- //

void TransformationFilterWorker::updateImpl()
{
    const Matrix3f& matrix = m_filter->getMatrix();

    if (matrix != m_matrix)
    {
        m_matrix = matrix;
        updateTransform();
    }
}

// ---------------------------------------------------------------------------------------------- //

void TransformationFilterWorker::runImpl(const Buffer& input, Buffer* output)
{
    remap(input, output, m_map);
}

// ---------------------------------------------------------------------------------------------- //

void TransformationFilterWorker::updateTransform()
{
    transformGen(&m_map, m_matrix);
}

// ---------------------------------------------------------------------------------------------- //
