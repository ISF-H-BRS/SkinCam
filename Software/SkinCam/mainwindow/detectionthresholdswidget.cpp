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

#include "detectionthresholdswidget.h"
#include "ui_detectionthresholdswidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

DetectionThresholdsWidget::DetectionThresholdsWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::DetectionThresholdsWidget>())
{
    m_ui->setupUi(this);
}

// ---------------------------------------------------------------------------------------------- //

DetectionThresholdsWidget::~DetectionThresholdsWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void DetectionThresholdsWidget::applySettings(const Settings::DetectionThresholdsGroup& settings)
{
    m_ui->skinRatio->setValue(settings.getSkinRatio());
}

// ---------------------------------------------------------------------------------------------- //

void DetectionThresholdsWidget::storeSettings(Settings::DetectionThresholdsGroup* settings)
{
    settings->setSkinRatio(m_ui->skinRatio->value());
}

// ---------------------------------------------------------------------------------------------- //

auto DetectionThresholdsWidget::getSkinRatio() const -> double
{
    return m_ui->skinRatio->value() * 0.001;
}

// ---------------------------------------------------------------------------------------------- //
