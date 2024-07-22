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

#include "namespace.h"
#include "settings.h"

#include <QWidget>

namespace Ui {
    class DetectionThresholdsWidget;
}

SKINCAM_BEGIN_NAMESPACE();

class DetectionThresholdsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DetectionThresholdsWidget(QWidget* parent = nullptr);
    ~DetectionThresholdsWidget() override;

    void applySettings(const Settings::DetectionThresholdsGroup& settings);
    void storeSettings(Settings::DetectionThresholdsGroup* settings);

    auto getSkinRatio() const -> double;

private:
    std::unique_ptr<Ui::DetectionThresholdsWidget> m_ui;
};

SKINCAM_END_NAMESPACE();
