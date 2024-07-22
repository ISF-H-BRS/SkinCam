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

#ifndef SKINCAM_BUILD_LEGACY

// ---------------------------------------------------------------------------------------------- //

#include "camerasystem.h"
#include "ledpowerwidget.h"

#include "ui_ledpowerwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

LedPowerWidget::LedPowerWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::LedPowerWidget>())
{
    m_ui->setupUi(this);

    m_ui->label0->setSuffix(" %");
    m_ui->label1->setSuffix(" %");
    m_ui->label2->setSuffix(" %");

    m_ui->label0->setMinimumTextWidth("100 %");
    m_ui->label1->setMinimumTextWidth("100 %");
    m_ui->label2->setMinimumTextWidth("100 %");

    connect(m_ui->slider0, SIGNAL(valueChanged(int)), m_ui->label0, SLOT(setValue(int)));
    connect(m_ui->slider1, SIGNAL(valueChanged(int)), m_ui->label1, SLOT(setValue(int)));
    connect(m_ui->slider2, SIGNAL(valueChanged(int)), m_ui->label2, SLOT(setValue(int)));

    connect(m_ui->slider0, SIGNAL(valueChanged(int)), this, SLOT(handleLevelChange()));
    connect(m_ui->slider1, SIGNAL(valueChanged(int)), this, SLOT(handleLevelChange()));
    connect(m_ui->slider2, SIGNAL(valueChanged(int)), this, SLOT(handleLevelChange()));

    connect(m_ui->resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));
}

// ---------------------------------------------------------------------------------------------- //

LedPowerWidget::~LedPowerWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void LedPowerWidget::applySettings(const Settings::LedPowerGroup& settings)
{
    blockSignals(true);
    m_ui->slider0->setValue(settings.getPowerLevel0());
    m_ui->slider1->setValue(settings.getPowerLevel1());
    m_ui->slider2->setValue(settings.getPowerLevel2());
    blockSignals(false);

    handleLevelChange();
}

// ---------------------------------------------------------------------------------------------- //

void LedPowerWidget::storeSettings(Settings::LedPowerGroup* settings)
{
    settings->setPowerLevel0(m_ui->slider0->value());
    settings->setPowerLevel1(m_ui->slider1->value());
    settings->setPowerLevel2(m_ui->slider2->value());
}

// ---------------------------------------------------------------------------------------------- //

auto LedPowerWidget::getPowerLevel0() const -> int
{
    return m_ui->slider0->value();
}

// ---------------------------------------------------------------------------------------------- //

auto LedPowerWidget::getPowerLevel1() const -> int
{
    return m_ui->slider1->value();
}

// ---------------------------------------------------------------------------------------------- //

auto LedPowerWidget::getPowerLevel2() const -> int
{
    return m_ui->slider2->value();
}

// ---------------------------------------------------------------------------------------------- //

void LedPowerWidget::handleLevelChange()
{
    if (signalsBlocked())
        return;

    int level0 = m_ui->slider0->value();
    int level1 = m_ui->slider1->value();
    int level2 = m_ui->slider2->value();

    emit powerLevelsChanged(level0, level1, level2);
}

// ---------------------------------------------------------------------------------------------- //

void LedPowerWidget::resetValues()
{
    blockSignals(true);
    m_ui->slider0->setValue(CameraSystem::DefaultPowerLevel);
    m_ui->slider1->setValue(CameraSystem::DefaultPowerLevel);
    m_ui->slider2->setValue(CameraSystem::DefaultPowerLevel);
    blockSignals(false);

    handleLevelChange();
}

// ---------------------------------------------------------------------------------------------- //

#endif // !SKINCAM_BUILD_LEGACY

// ---------------------------------------------------------------------------------------------- //
