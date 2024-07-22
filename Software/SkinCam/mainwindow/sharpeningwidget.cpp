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
#include "sharpeningwidget.h"

#include "ui_sharpeningwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

SharpeningWidget::SharpeningWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::SharpeningWidget>())
{
    m_ui->setupUi(this);

    connect(m_ui->enable, SIGNAL(toggled(bool)), this, SIGNAL(enableToggled(bool)));

    connect(m_ui->radius, SIGNAL(valueChanged(int)), this, SLOT(handleValueChange()));
    connect(m_ui->amount, SIGNAL(valueChanged(double)), this, SLOT(handleValueChange()));
    connect(m_ui->threshold, SIGNAL(valueChanged(int)), this, SLOT(handleValueChange()));

    connect(m_ui->resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));
}

// ---------------------------------------------------------------------------------------------- //

SharpeningWidget::~SharpeningWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void SharpeningWidget::applySettings(const Settings::SharpeningGroup& settings)
{
    m_ui->enable->setChecked(settings.getEnabled());

    blockSignals(true);
    m_ui->radius->setValue(settings.getRadius());
    m_ui->amount->setValue(settings.getAmount());
    m_ui->threshold->setValue(settings.getThreshold());
    blockSignals(false);

    handleValueChange();
}

// ---------------------------------------------------------------------------------------------- //

void SharpeningWidget::storeSettings(Settings::SharpeningGroup* settings)
{
    settings->setEnabled(m_ui->enable->isChecked());
    settings->setRadius(m_ui->radius->value());
    settings->setAmount(m_ui->amount->value());
    settings->setThreshold(m_ui->threshold->value());
}

// ---------------------------------------------------------------------------------------------- //

void SharpeningWidget::handleValueChange()
{
    if (signalsBlocked())
        return;

    auto radius = static_cast<uint>(m_ui->radius->value());
    auto amount = static_cast<float>(m_ui->amount->value());
    auto threshold = static_cast<float>(m_ui->threshold->value()) * 0.0001F;

    emit valuesChanged(radius, amount, threshold);
}

// ---------------------------------------------------------------------------------------------- //

void SharpeningWidget::resetValues()
{
    static const auto radius = static_cast<int>(SharpeningFilter::DefaultRadius);
    static const auto amount = static_cast<double>(SharpeningFilter::DefaultAmount);
    static const auto threshold = static_cast<int>(SharpeningFilter::DefaultThreshold * 10000.0F);

    m_ui->radius->blockSignals(true);
    m_ui->radius->setValue(radius);
    m_ui->radius->blockSignals(false);

    m_ui->amount->blockSignals(true);
    m_ui->amount->setValue(amount);
    m_ui->amount->blockSignals(false);

    m_ui->threshold->blockSignals(true);
    m_ui->threshold->setValue(threshold);
    m_ui->threshold->blockSignals(false);

    handleValueChange();
}

// ---------------------------------------------------------------------------------------------- //
