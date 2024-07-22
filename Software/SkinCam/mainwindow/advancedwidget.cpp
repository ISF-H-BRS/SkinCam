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

#include "advancedwidget.h"
#include "camera.h"

#include "ui_advancedwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

AdvancedWidget::AdvancedWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::AdvancedWidget>())
{
    m_ui->setupUi(this);

    connect(m_ui->temperatureSetpoint, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleSetpointChange(int)));

}

// ---------------------------------------------------------------------------------------------- //

AdvancedWidget::~AdvancedWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void AdvancedWidget::applySettings(const Settings::AdvancedGroup& settings)
{
    int setpoint = std::min(settings.getTemperatureSetpoint(),
                            static_cast<int>(Camera::TemperatureSetpointCount - 1));

    m_ui->temperatureSetpoint->setCurrentIndex(setpoint);
}

// ---------------------------------------------------------------------------------------------- //

void AdvancedWidget::storeSettings(Settings::AdvancedGroup* settings)
{
    settings->setTemperatureSetpoint(m_ui->temperatureSetpoint->currentIndex());
}

// ---------------------------------------------------------------------------------------------- //

auto AdvancedWidget::getTemperatureSetpoint() const -> Camera::TemperatureSetpoint
{
    return Camera::toTemperatureSetpoint(m_ui->temperatureSetpoint->currentIndex());
}

// ---------------------------------------------------------------------------------------------- //

void AdvancedWidget::handleSetpointChange(int index)
{
    emit temperatureSetpointChanged(Camera::toTemperatureSetpoint(index));
}

// ---------------------------------------------------------------------------------------------- //
