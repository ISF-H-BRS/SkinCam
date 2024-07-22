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

#pragma once

#include "camera.h"
#include "controller.h"

#include <QWidget>

#include <queue>

namespace Ui {
    class StatusWidget;
}

SKINCAM_BEGIN_NAMESPACE();

class StatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatusWidget(QWidget* parent = nullptr);
    ~StatusWidget() override;

    void reset();

    auto currentPacketLoss() const -> size_t;

public slots:
    void setCameraStatus(const Camera::Status& status);

#ifndef SKINCAM_BUILD_LEGACY
    void setPowerStatus(const Controller::PowerStatus& status);
#endif

private:
    auto updatePacketLoss(const Camera::Status& status) -> size_t;

private:
    std::unique_ptr<Ui::StatusWidget> m_ui;

    std::queue<std::pair<size_t,size_t>> m_packetStats;
    size_t m_currentPacketLoss = 0;
};

SKINCAM_END_NAMESPACE();
