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

#include "barrelwidget.h"
#include "ui_barrelwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

BarrelWidget::BarrelWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::BarrelWidget>())
{
    m_ui->setupUi(this);
    m_ui->number->setMinimumTextWidth("-100");

    connect(m_ui->enable, SIGNAL(toggled(bool)), this, SIGNAL(enableToggled(bool)));

    connect(m_ui->slider, SIGNAL(valueChanged(int)), m_ui->number, SLOT(setValue(int)));
    connect(m_ui->slider, SIGNAL(valueChanged(int)), this, SIGNAL(factorChanged(int)));

    connect(m_ui->resetButton, SIGNAL(clicked()), this, SLOT(resetFactor()));
}

// ---------------------------------------------------------------------------------------------- //

BarrelWidget::~BarrelWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void BarrelWidget::applySettings(const Settings::BarrelGroup& settings)
{
    m_ui->enable->setChecked(settings.getEnabled());
    m_ui->slider->setValue(settings.getFactor());
}

// ---------------------------------------------------------------------------------------------- //

void BarrelWidget::storeSettings(Settings::BarrelGroup* settings)
{
    settings->setEnabled(m_ui->enable->isChecked());
    settings->setFactor(m_ui->slider->value());
}

// ---------------------------------------------------------------------------------------------- //

void BarrelWidget::resetFactor()
{
    m_ui->slider->setValue(0);
}

// ---------------------------------------------------------------------------------------------- //
