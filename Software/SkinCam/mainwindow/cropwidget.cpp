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

#include "cropwidget.h"
#include "ui_cropwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

CropWidget::CropWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::CropWidget>())
{
    m_ui->setupUi(this);

    connect(m_ui->enable, SIGNAL(toggled(bool)), this, SIGNAL(enableToggled(bool)));

    connect(m_ui->left,   SIGNAL(valueChanged(int)), this, SLOT(handleValueChange()));
    connect(m_ui->right,  SIGNAL(valueChanged(int)), this, SLOT(handleValueChange()));
    connect(m_ui->top,    SIGNAL(valueChanged(int)), this, SLOT(handleValueChange()));
    connect(m_ui->bottom, SIGNAL(valueChanged(int)), this, SLOT(handleValueChange()));

    connect(m_ui->resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));
}

// ---------------------------------------------------------------------------------------------- //

CropWidget::~CropWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void CropWidget::applySettings(const Settings::CropGroup& settings)
{
    m_ui->enable->setChecked(settings.getEnabled());

    blockSignals(true);
    m_ui->left->setValue(settings.getLeft());
    m_ui->right->setValue(settings.getRight());
    m_ui->top->setValue(settings.getTop());
    m_ui->bottom->setValue(settings.getBottom());
    blockSignals(false);

    handleValueChange();
}

// ---------------------------------------------------------------------------------------------- //

void CropWidget::storeSettings(Settings::CropGroup* settings)
{
    settings->setEnabled(m_ui->enable->isChecked());

    settings->setLeft(m_ui->left->value());
    settings->setRight(m_ui->right->value());
    settings->setTop(m_ui->top->value());
    settings->setBottom(m_ui->bottom->value());
}

// ---------------------------------------------------------------------------------------------- //

void CropWidget::handleValueChange()
{
    if (signalsBlocked())
        return;

    int left = m_ui->left->value();
    int right = m_ui->right->value();
    int top = m_ui->top->value();
    int bottom = m_ui->bottom->value();

    emit valuesChanged(left, right, top, bottom);
}

// ---------------------------------------------------------------------------------------------- //

void CropWidget::resetValues()
{
    blockSignals(true);

    for (auto box : { m_ui->left, m_ui->right, m_ui->top, m_ui->bottom })
        box->setValue(0);

    blockSignals(false);

    emit valuesChanged(0, 0, 0, 0);
}

// ---------------------------------------------------------------------------------------------- //
