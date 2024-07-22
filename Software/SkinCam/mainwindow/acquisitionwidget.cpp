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

#include "acquisitionwidget.h"
#include "ui_acquisitionwidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

AcquisitionWidget::AcquisitionWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::AcquisitionWidget>())
{
    m_ui->setupUi(this);

    m_ui->frameRateNumber->setSuffix(" Hz");
    m_ui->frameRateNumber->setMinimumTextWidth("xx Hz");

    m_ui->illuminationTime->setMinimum(MinimumIlluminationTime);
    m_ui->illuminationTime->setMaximum(MaximumIlluminationTime);

    m_ui->exposureTime->setMinimum(MinimumExposureTime);
    m_ui->exposureTime->setMaximum(MaximumExposureTime);

#ifdef SKINCAM_BUILD_LEGACY
    m_ui->illuminationTimeLabel->hide();
    m_ui->illuminationTime->hide();
    m_ui->lockWidget->hide();
#endif

    m_ui->cameraGain->addItem("Low");
#ifndef SKINCAM_BUILD_LEGACY
    m_ui->cameraGain->addItem("Medium");
#endif
    m_ui->cameraGain->addItem("High");

    const auto labels = {
        m_ui->frameRateLabel,
        m_ui->illuminationTimeLabel,
        m_ui->exposureTimeLabel,
        m_ui->cameraGainLabel
    };

    const Qt::Alignment alignment(style()->styleHint(QStyle::SH_FormLayoutLabelAlignment));

    for (auto label : labels)
        label->setAlignment(alignment);

    connect(m_ui->frameRateSlider, SIGNAL(valueChanged(int)),
            m_ui->frameRateNumber, SLOT(setValue(int)));
    connect(m_ui->frameRateSlider, SIGNAL(valueChanged(int)),
            this, SLOT(handleFrameRateChange(int)));

    connect(m_ui->exposureTime, SIGNAL(valueChanged(double)),
            this, SLOT(handleExposureTimeChange(double)));
    connect(m_ui->illuminationTime, SIGNAL(valueChanged(double)),
            this, SLOT(handleIlluminationTimeChange(double)));

    connect(m_ui->lockTimesButton, SIGNAL(clicked()), this, SLOT(handleLockButtonClicked()));

    connect(m_ui->cameraGain, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleCameraGainChange(int)));

    connect(m_ui->resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));
}

// ---------------------------------------------------------------------------------------------- //

AcquisitionWidget::~AcquisitionWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void AcquisitionWidget::applySettings(const Settings::AcquisitionGroup& settings)
{
    int frameRate = std::clamp(settings.getFrameRate(), MinimumFrameRate, MaximumFrameRate);
    m_ui->frameRateSlider->setValue(frameRate);

    double illuminationTime = std::clamp(settings.getIlluminationTime(),
                                         m_ui->illuminationTime->minimum(),
                                         m_ui->illuminationTime->maximum());
    m_ui->illuminationTime->setValue(illuminationTime);

    double exposureTime = std::clamp(settings.getExposureTime(),
                                     m_ui->exposureTime->minimum(),
                                     m_ui->exposureTime->maximum());
    m_ui->exposureTime->setValue(exposureTime);

    setTimesLocked(settings.getTimesLocked());

    int gain = std::clamp(settings.getCameraGain(), 0, static_cast<int>(Camera::GainCount - 1));
    m_ui->cameraGain->setCurrentIndex(gain);
}

// ---------------------------------------------------------------------------------------------- //

void AcquisitionWidget::storeSettings(Settings::AcquisitionGroup* settings)
{
    settings->setFrameRate(m_ui->frameRateSlider->value());
    settings->setIlluminationTime(m_ui->illuminationTime->value());
    settings->setExposureTime(m_ui->exposureTime->value());
    settings->setTimesLocked(m_timesLocked);
    settings->setCameraGain(m_ui->cameraGain->currentIndex());
}

// ---------------------------------------------------------------------------------------------- //

void AcquisitionWidget::setMinMaxFrameRate(int minimum, int maximum)
{
    m_ui->frameRateSlider->setMinimum(minimum);
    m_ui->frameRateSlider->setMaximum(maximum);

    m_ui->frameRateNumber->setMinimumTextWidth(QString("%1 Hz").arg(maximum));
}

// ---------------------------------------------------------------------------------------------- //

auto AcquisitionWidget::getFrameRate() const -> int
{
    return m_ui->frameRateSlider->value();
}

// ---------------------------------------------------------------------------------------------- //

auto AcquisitionWidget::getIlluminationTime() const -> std::chrono::microseconds
{
    return millisecondsToTime(m_ui->illuminationTime->value());
}

// ---------------------------------------------------------------------------------------------- //

auto AcquisitionWidget::getExposureTime() const -> std::chrono::microseconds
{
    return millisecondsToTime(m_ui->exposureTime->value());
}

// ---------------------------------------------------------------------------------------------- //

auto AcquisitionWidget::getCameraGain() const -> Camera::Gain
{
    auto index = static_cast<size_t>(m_ui->cameraGain->currentIndex());
    Q_ASSERT(index < Camera::GainCount);

    return Camera::toGain(index);
}

// ---------------------------------------------------------------------------------------------- //

void AcquisitionWidget::handleFrameRateChange(int value)
{
    // Round to 0.1 ms
    const double ms = std::floor(10000.0 / value / Camera::ImagesPerFrame) * 0.1;

#ifdef SKINCAM_BUILD_LEGACY
    const double time = std::clamp(ms, MinimumExposureTime, MaximumExposureTime);
    m_ui->exposureTime->setMaximum(time);
#else
    const double time = std::clamp(ms, MinimumIlluminationTime, MaximumIlluminationTime);
    m_ui->illuminationTime->setMaximum(time);
#endif

    emit frameRateChanged(value);
}

// ---------------------------------------------------------------------------------------------- //

void AcquisitionWidget::handleIlluminationTimeChange(double value)
{
    const double time = std::clamp(m_ui->illuminationTime->value(),
                                   MinimumExposureTime, MaximumExposureTime);
    m_ui->exposureTime->setMaximum(time);

    emit illuminationTimeChanged(millisecondsToTime(value));

    if (m_timesLocked)
        m_ui->exposureTime->setValue(value);
}

// ---------------------------------------------------------------------------------------------- //

void AcquisitionWidget::handleExposureTimeChange(double value)
{
    emit exposureTimeChanged(millisecondsToTime(value));
}

// ---------------------------------------------------------------------------------------------- //

void AcquisitionWidget::handleLockButtonClicked()
{
    setTimesLocked(!m_timesLocked);
}

// ---------------------------------------------------------------------------------------------- //

void AcquisitionWidget::handleCameraGainChange(int index)
{
    Q_ASSERT(static_cast<size_t>(index) < Camera::GainCount);
    emit cameraGainChanged(Camera::toGain(index));
}

// ---------------------------------------------------------------------------------------------- //

void AcquisitionWidget::resetValues()
{
    m_ui->frameRateSlider->setValue(CameraSystem::DefaultRefreshRate);
    m_ui->illuminationTime->setValue(timeToMilliseconds(CameraSystem::DefaultIlluminationTime));
    m_ui->exposureTime->setValue(timeToMilliseconds(Camera::DefaultExposureTime));
    m_ui->cameraGain->setCurrentIndex(0);
}

// ---------------------------------------------------------------------------------------------- //

void AcquisitionWidget::setTimesLocked(bool lock)
{
#ifdef SKINCAM_BUILD_LEGACY
    lock = false;
#endif

    m_timesLocked = lock;

    if (m_timesLocked)
    {
        m_ui->lockTimesButton->setIcon(QIcon(":/res/lock.svg"));
        m_ui->exposureTime->setValue(m_ui->illuminationTime->value());
    }
    else
        m_ui->lockTimesButton->setIcon(QIcon(":/res/unlock.svg"));

    m_ui->exposureTime->setEnabled(!m_timesLocked);
}

// ---------------------------------------------------------------------------------------------- //
