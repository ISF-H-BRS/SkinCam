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

#include "regionofinterestwidget.h"
#include "ui_regionofinterestwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

RegionOfInterestWidget::RegionOfInterestWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::RegionOfInterestWidget>())
{
    m_ui->setupUi(this);

    connect(m_ui->enable, SIGNAL(toggled(bool)), this, SIGNAL(enableToggled(bool)));
    connect(m_ui->editButton, SIGNAL(toggled(bool)), this, SIGNAL(editToggled(bool)));
    connect(m_ui->resetButton, SIGNAL(clicked()), this, SIGNAL(resetClicked()));
}

// ---------------------------------------------------------------------------------------------- //

RegionOfInterestWidget::~RegionOfInterestWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void RegionOfInterestWidget::applySettings(const Settings::RegionOfInterestGroup& settings)
{
    m_ui->enable->setChecked(settings.getEnabled());
}

// ---------------------------------------------------------------------------------------------- //

void RegionOfInterestWidget::storeSettings(Settings::RegionOfInterestGroup* settings)
{
    settings->setEnabled(m_ui->enable->isChecked());
}

// ---------------------------------------------------------------------------------------------- //

void RegionOfInterestWidget::setEditChecked(bool checked)
{
    m_ui->editButton->setChecked(checked);
}

// ---------------------------------------------------------------------------------------------- //
