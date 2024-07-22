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

#include "classifierwidget.h"
#include "ui_classifierwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

ClassifierWidget::ClassifierWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::ClassifierWidget>())
{
    m_ui->setupUi(this);

    m_ui->scalingNumber->setMinimumTextWidth("-100");

    m_ui->thresholdNumber->setSuffix(" %");
    m_ui->thresholdNumber->setMinimumTextWidth("100 %");

    connect(m_ui->gaussianButton, SIGNAL(clicked()), this, SLOT(onClassifierChanged()));
    connect(m_ui->gaussianThresholdButton, SIGNAL(clicked()), this, SLOT(onClassifierChanged()));
    connect(m_ui->boundingBoxButton, SIGNAL(clicked()), this, SLOT(onClassifierChanged()));

    connect(m_ui->scalingSlider, SIGNAL(valueChanged(int)),
            m_ui->scalingNumber, SLOT(setValue(int)));
    connect(m_ui->scalingSlider, SIGNAL(valueChanged(int)),
            this, SIGNAL(scalingChanged(int)));

    connect(m_ui->thresholdSlider, SIGNAL(valueChanged(int)),
            m_ui->thresholdNumber, SLOT(setValue(int)));
    connect(m_ui->thresholdSlider, SIGNAL(valueChanged(int)),
            this, SIGNAL(thresholdChanged(int)));
}

// ---------------------------------------------------------------------------------------------- //

ClassifierWidget::~ClassifierWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void ClassifierWidget::applySettings(const Settings::ClassifierGroup& settings)
{
    setCurrentMethod(static_cast<SkinClassifier::Method>(settings.getMethod()));
    m_ui->scalingSlider->setValue(settings.getScaling());
    m_ui->thresholdSlider->setValue(settings.getThreshold());
}

// ---------------------------------------------------------------------------------------------- //

void ClassifierWidget::storeSettings(Settings::ClassifierGroup* settings)
{
    settings->setMethod(static_cast<int>(currentMethod()));
    settings->setScaling(m_ui->scalingSlider->value());
    settings->setThreshold(m_ui->thresholdSlider->value());
}

// ---------------------------------------------------------------------------------------------- //

void ClassifierWidget::setCurrentMethod(SkinClassifier::Method method)
{
    if (method == SkinClassifier::Method::Gaussian)
        m_ui->gaussianButton->setChecked(true);
    else if (method == SkinClassifier::Method::GaussianThreshold)
        m_ui->gaussianThresholdButton->setChecked(true);
    else
        m_ui->boundingBoxButton->setChecked(true);

    onClassifierChanged();
}

// ---------------------------------------------------------------------------------------------- //

auto ClassifierWidget::currentMethod() const -> SkinClassifier::Method
{
    if (m_ui->gaussianButton->isChecked())
        return SkinClassifier::Method::Gaussian;

    if (m_ui->gaussianThresholdButton->isChecked())
        return SkinClassifier::Method::GaussianThreshold;

    return SkinClassifier::Method::BoundingBox;
}

// ---------------------------------------------------------------------------------------------- //

auto ClassifierWidget::scaling() const -> int
{
    return m_ui->scalingSlider->value();
}

// ---------------------------------------------------------------------------------------------- //

auto ClassifierWidget::threshold() const -> int
{
    return m_ui->thresholdSlider->value();
}

// ---------------------------------------------------------------------------------------------- //

void ClassifierWidget::onClassifierChanged()
{
    const SkinClassifier::Method method = currentMethod();
    const bool isGaussian = (method == SkinClassifier::Method::Gaussian);

    m_ui->thresholdLabel->setEnabled(!isGaussian);
    m_ui->thresholdWidget->setEnabled(!isGaussian);

    emit classifierChanged(currentMethod());
}

// ---------------------------------------------------------------------------------------------- //
