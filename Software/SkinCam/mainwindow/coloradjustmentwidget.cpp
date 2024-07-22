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

#include "coloradjustmentwidget.h"
#include "ui_coloradjustmentwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

ColorAdjustmentWidget::ColorAdjustmentWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::ColorAdjustmentWidget>())
{
    m_ui->setupUi(this);

    m_ui->brightnessLabel->setMinimumTextWidth("-100");
    m_ui->contrastLabel->setMinimumTextWidth("-100");

    connect(m_ui->enable, SIGNAL(toggled(bool)), this, SIGNAL(enableToggled(bool)));

    connect(m_ui->brightnessSlider, SIGNAL(valueChanged(int)),
            m_ui->brightnessLabel, SLOT(setValue(int)));
    connect(m_ui->contrastSlider, SIGNAL(valueChanged(int)),
            m_ui->contrastLabel, SLOT(setValue(int)));

    connect(m_ui->brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(handleValueChange()));
    connect(m_ui->contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(handleValueChange()));

    connect(m_ui->resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));
}

// ---------------------------------------------------------------------------------------------- //

ColorAdjustmentWidget::~ColorAdjustmentWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void ColorAdjustmentWidget::applySettings(const Settings::ColorAdjustmentGroup& settings)
{
    m_ui->enable->setChecked(settings.getEnabled());

    blockSignals(true);
    m_ui->brightnessSlider->setValue(settings.getBrightness());
    m_ui->contrastSlider->setValue(settings.getContrast());
    blockSignals(false);

    handleValueChange();
}

// ---------------------------------------------------------------------------------------------- //

void ColorAdjustmentWidget::storeSettings(Settings::ColorAdjustmentGroup* settings)
{
    settings->setEnabled(m_ui->enable->isChecked());
    settings->setBrightness(m_ui->brightnessSlider->value());
    settings->setContrast(m_ui->contrastSlider->value());
}

// ---------------------------------------------------------------------------------------------- //

void ColorAdjustmentWidget::handleValueChange()
{
    if (signalsBlocked())
        return;

    int brightness = m_ui->brightnessSlider->value();
    int contrast = m_ui->contrastSlider->value();

    emit valuesChanged(brightness, contrast);
}

// ---------------------------------------------------------------------------------------------- //

void ColorAdjustmentWidget::resetValues()
{
    blockSignals(true);
    m_ui->brightnessSlider->setValue(0);
    m_ui->contrastSlider->setValue(0);
    blockSignals(false);

    emit valuesChanged(0, 0);
}

// ---------------------------------------------------------------------------------------------- //
