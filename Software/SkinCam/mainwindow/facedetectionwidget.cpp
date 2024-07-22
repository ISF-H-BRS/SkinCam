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

#include "facedetectionwidget.h"
#include "ui_facedetectionwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

FaceDetectionWidget::FaceDetectionWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::FaceDetectionWidget>())
{
    m_ui->setupUi(this);

    connect(m_ui->enable, SIGNAL(toggled(bool)), this, SIGNAL(enableToggled(bool)));
    connect(m_ui->minimumSkinRatio, SIGNAL(valueChanged(int)),
            this, SIGNAL(minimumSkinRatioChanged(int)));
    connect(m_ui->highlightSkinPixels, SIGNAL(toggled(bool)),
            this, SIGNAL(highlightSkinPixelsToggled(bool)));
}

// ---------------------------------------------------------------------------------------------- //

FaceDetectionWidget::~FaceDetectionWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void FaceDetectionWidget::applySettings(const Settings::FaceDetectionGroup& settings)
{
    m_ui->enable->setChecked(settings.getEnabled());
    m_ui->minimumSkinRatio->setValue(settings.getMinimumSkinRatio());
    m_ui->highlightSkinPixels->setChecked(settings.getSkinPixelsHighlighted());
}

// ---------------------------------------------------------------------------------------------- //

void FaceDetectionWidget::storeSettings(Settings::FaceDetectionGroup* settings)
{
    settings->setEnabled(m_ui->enable->isChecked());
    settings->setMinimumSkinRatio(m_ui->minimumSkinRatio->value());
    settings->setSkinPixelsHighlighted(m_ui->highlightSkinPixels->isChecked());
}

// ---------------------------------------------------------------------------------------------- //

auto FaceDetectionWidget::detectionEnabled() const -> bool
{
    return m_ui->enable->isChecked();
}

// ---------------------------------------------------------------------------------------------- //

auto FaceDetectionWidget::minimumSkinRatio() const -> int
{
    return m_ui->minimumSkinRatio->value();
}

// ---------------------------------------------------------------------------------------------- //

auto FaceDetectionWidget::skinPixelsHighlighted() const -> bool
{
    return m_ui->highlightSkinPixels->isChecked();
}

// ---------------------------------------------------------------------------------------------- //
