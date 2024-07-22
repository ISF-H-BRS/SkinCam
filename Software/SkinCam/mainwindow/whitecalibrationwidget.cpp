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

#include "whitecalibrationwidget.h"
#include "ui_whitecalibrationwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

WhiteCalibrationWidget::WhiteCalibrationWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::WhiteCalibrationWidget)
{
    m_ui->setupUi(this);

    connect(m_ui->enable, SIGNAL(toggled(bool)), this, SIGNAL(enableToggled(bool)));

    connect(m_ui->factor0, SIGNAL(valueChanged(double)), this, SLOT(handleFactorChange()));
    connect(m_ui->factor1, SIGNAL(valueChanged(double)), this, SLOT(handleFactorChange()));
    connect(m_ui->factor2, SIGNAL(valueChanged(double)), this, SLOT(handleFactorChange()));

    connect(m_ui->resetButton, SIGNAL(clicked()), this, SLOT(resetFactors()));
}

// ---------------------------------------------------------------------------------------------- //

WhiteCalibrationWidget::~WhiteCalibrationWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void WhiteCalibrationWidget::applySettings(const Settings::WhiteCalibrationGroup& settings)
{
    m_ui->enable->setChecked(settings.getEnabled());

    blockSignals(true);
    m_ui->factor0->setValue(settings.getFactor0());
    m_ui->factor1->setValue(settings.getFactor1());
    m_ui->factor2->setValue(settings.getFactor2());
    blockSignals(false);

    handleFactorChange();
}

// ---------------------------------------------------------------------------------------------- //

void WhiteCalibrationWidget::storeSettings(Settings::WhiteCalibrationGroup* settings)
{
    settings->setEnabled(m_ui->enable->isChecked());
    settings->setFactor0(m_ui->factor0->value());
    settings->setFactor1(m_ui->factor1->value());
    settings->setFactor2(m_ui->factor2->value());
}

// ---------------------------------------------------------------------------------------------- //

void WhiteCalibrationWidget::handleFactorChange()
{
    if (signalsBlocked())
        return;

    double factor0 = m_ui->factor0->value();
    double factor1 = m_ui->factor1->value();
    double factor2 = m_ui->factor2->value();

    emit factorsChanged(factor0, factor1, factor2);
}

// ---------------------------------------------------------------------------------------------- //

void WhiteCalibrationWidget::resetFactors()
{
    blockSignals(true);

    for (auto factor : { m_ui->factor0, m_ui->factor1, m_ui->factor2 })
        factor->setValue(1.0);

    blockSignals(false);

    handleFactorChange();
}

// ---------------------------------------------------------------------------------------------- //
