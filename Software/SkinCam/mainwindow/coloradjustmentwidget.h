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
    class ColorAdjustmentWidget;
}

SKINCAM_BEGIN_NAMESPACE();

class ColorAdjustmentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColorAdjustmentWidget(QWidget* parent = nullptr);
    ~ColorAdjustmentWidget() override;

    void applySettings(const Settings::ColorAdjustmentGroup& settings);
    void storeSettings(Settings::ColorAdjustmentGroup* settings);

signals:
    void enableToggled(bool enabled);
    void valuesChanged(int brightness, int contrast);

private slots:
    void handleValueChange();
    void resetValues();

private:
    std::unique_ptr<Ui::ColorAdjustmentWidget> m_ui;
};

SKINCAM_END_NAMESPACE();
