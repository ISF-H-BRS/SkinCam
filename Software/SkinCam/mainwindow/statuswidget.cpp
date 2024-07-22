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

#include "statuswidget.h"
#include "ui_statuswidget.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

StatusWidget::StatusWidget(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::StatusWidget>())
{
    m_ui->setupUi(this);

#ifdef SKINCAM_BUILD_LEGACY
    m_ui->ledPowerLabel->hide();
    m_ui->ledPower->hide();
#endif
}

// ---------------------------------------------------------------------------------------------- //

StatusWidget::~StatusWidget() = default;

// ---------------------------------------------------------------------------------------------- //

void StatusWidget::reset()
{
    m_packetStats = {};
    m_currentPacketLoss = 0;
}

// ---------------------------------------------------------------------------------------------- //

auto StatusWidget::currentPacketLoss() const -> size_t
{
    return m_currentPacketLoss;
}

// ---------------------------------------------------------------------------------------------- //

void StatusWidget::setCameraStatus(const Camera::Status& status)
{
    static constexpr std::array<const char*, Camera::TemperatureSetpointCount> strings = {
        "Error", "5 °C", "20 °C", "35 °C", "50 °C"
    };

    m_ui->temperatureSetpoint->setText(strings.at(Camera::indexOf(status.temperatureSetpoint)));
    m_ui->temperature->setText(QString("%1 °C").arg(status.temperature, 0, 'f', 1));

    // Apparent bug in camera firmware or Vimba library,
    // value returned from the device is often invalid
    if (status.humidity >= 0.0 && status.humidity <= 100.0)
        m_ui->humidity->setText(QString("%1 %").arg(status.humidity, 0, 'f', 1));

    m_ui->coolingPower->setText(QString("%1 mW").arg(status.coolingPower));

    m_currentPacketLoss = updatePacketLoss(status);
    m_ui->packetLoss->setText(QString::number(m_currentPacketLoss) + " ‰");

    if (m_currentPacketLoss > 0)
    {
        const QString text = m_ui->packetLoss->text();
        m_ui->packetLoss->setText("<font color=\"red\">" + text + "</font>");
    }
}

// ---------------------------------------------------------------------------------------------- //

#ifndef SKINCAM_BUILD_LEGACY
void StatusWidget::setPowerStatus(const Controller::PowerStatus& status)
{
    const auto voltage = QString::number(status.voltage, 'f', 1);
    const auto current = QString::number(status.current * 1000.0, 'f', 0);

    m_ui->ledPower->setText(QString("%1 V @ %2 mA").arg(voltage, current));
}
#endif

// ---------------------------------------------------------------------------------------------- //

auto StatusWidget::updatePacketLoss(const Camera::Status& status) -> size_t
{
    constexpr size_t QueueLength = 4; // Consider last three seconds

    m_packetStats.emplace(status.packetsRequested, status.packetsReceived);

    if (m_packetStats.size() < QueueLength)
        return 0;

    while (m_packetStats.size() > QueueLength)
        m_packetStats.pop();

    const auto& front = m_packetStats.front();
    const auto& back = m_packetStats.back();

    // No packets received yet or acquisition has been restarted
    if (back.first <= front.first || back.second <= front.second)
        return 0;

    size_t requested = back.first - front.first;
    size_t received = back.second - front.second;

    return requested * 1000 / received;
}

// ---------------------------------------------------------------------------------------------- //
