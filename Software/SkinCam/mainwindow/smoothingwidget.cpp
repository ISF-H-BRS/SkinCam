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

#include "filters.h"
#include "smoothingwidget.h"

#include "ui_smoothingwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

SmoothingWidget::SmoothingWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::SmoothingWidget>())
{
    m_ui->setupUi(this);

    connect(m_ui->enable, SIGNAL(toggled(bool)), this, SIGNAL(enableToggled(bool)));
    connect(m_ui->radius, SIGNAL(valueChanged(int)), this, SLOT(handleRadiusChange()));
}

// ---------------------------------------------------------------------------------------------- //

SmoothingWidget::~SmoothingWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void SmoothingWidget::applySettings(const Settings::SmoothingGroup& settings)
{
    m_ui->enable->setChecked(settings.getEnabled());

    blockSignals(true);
    m_ui->radius->setValue(settings.getRadius());
    blockSignals(false);

    handleRadiusChange();
}

// ---------------------------------------------------------------------------------------------- //

void SmoothingWidget::storeSettings(Settings::SmoothingGroup* settings)
{
    settings->setEnabled(m_ui->enable->isChecked());
    settings->setRadius(m_ui->radius->value());
}

// ---------------------------------------------------------------------------------------------- //

void SmoothingWidget::handleRadiusChange()
{
    if (signalsBlocked())
        return;

    auto radius = static_cast<uint>(m_ui->radius->value());
    emit radiusChanged(radius);
}

// ---------------------------------------------------------------------------------------------- //
