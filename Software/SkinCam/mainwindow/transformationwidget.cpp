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

#include "transformationwidget.h"
#include "ui_transformationwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

TransformationWidget::TransformationWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::TransformationWidget>())
{
    m_ui->setupUi(this);

    connect(m_ui->enable, SIGNAL(toggled(bool)), this, SIGNAL(enableToggled(bool)));

    connect(m_ui->hScale, SIGNAL(valueChanged(double)), this, SLOT(handleTransformationChanged()));
    connect(m_ui->vScale, SIGNAL(valueChanged(double)), this, SLOT(handleTransformationChanged()));
    connect(m_ui->hShift, SIGNAL(valueChanged(int)), this, SLOT(handleTransformationChanged()));
    connect(m_ui->vShift, SIGNAL(valueChanged(int)), this, SLOT(handleTransformationChanged()));
    connect(m_ui->rAngle, SIGNAL(valueChanged(double)), this, SLOT(handleTransformationChanged()));

    connect(m_ui->resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));
}

// ---------------------------------------------------------------------------------------------- //

TransformationWidget::~TransformationWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void TransformationWidget::applySettings(const Settings::TransformationGroup& settings)
{
    m_ui->enable->setChecked(settings.getEnabled());

    blockSignals(true);
    m_ui->hScale->setValue(settings.getHScale());
    m_ui->vScale->setValue(settings.getVScale());
    m_ui->hShift->setValue(settings.getHShift());
    m_ui->vShift->setValue(settings.getVShift());
    m_ui->rAngle->setValue(settings.getRAngle());
    blockSignals(false);

    handleTransformationChanged();
}

// ---------------------------------------------------------------------------------------------- //

void TransformationWidget::storeSettings(Settings::TransformationGroup* settings)
{
    settings->setEnabled(m_ui->enable->isChecked());

    settings->setHScale(m_ui->hScale->value());
    settings->setVScale(m_ui->vScale->value());
    settings->setHShift(m_ui->hShift->value());
    settings->setVShift(m_ui->vShift->value());
    settings->setRAngle(m_ui->rAngle->value());
}

// ---------------------------------------------------------------------------------------------- //

auto TransformationWidget::getTransformation() const -> Matrix3f
{
    constexpr double Pi = 3.1415926535897932385;

    auto hScale = static_cast<float>(m_ui->hScale->value());
    auto vScale = static_cast<float>(m_ui->vScale->value());
    auto hShift = static_cast<float>(m_ui->hShift->value());
    auto vShift = static_cast<float>(m_ui->vShift->value());
    auto rAngle = static_cast<float>(Pi / 180.0 * m_ui->rAngle->value());

    return Matrix3f::translate(hShift, vShift) *
           Matrix3f::scale(hScale, vScale) *
           Matrix3f::rotate(rAngle);
}

// ---------------------------------------------------------------------------------------------- //

void TransformationWidget::handleTransformationChanged()
{
    emit transformationChanged(getTransformation());
}

// ---------------------------------------------------------------------------------------------- //

void TransformationWidget::resetValues()
{
    blockSignals(true);
    m_ui->hScale->setValue(1.0);
    m_ui->vScale->setValue(1.0);
    m_ui->hShift->setValue(0);
    m_ui->vShift->setValue(0);
    m_ui->rAngle->setValue(0.0);
    blockSignals(false);

    handleTransformationChanged();
}

// ---------------------------------------------------------------------------------------------- //
