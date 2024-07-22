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
    class LedPowerWidget;
}

SKINCAM_BEGIN_NAMESPACE();

#ifdef SKINCAM_BUILD_LEGACY
// Only create dummy widget, containing group box will be hidden for legacy builds
class LedPowerWidget : public QWidget
{
    Q_OBJECT
    using QWidget::QWidget;
};
#else

class LedPowerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LedPowerWidget(QWidget* parent = nullptr);
    ~LedPowerWidget() override;

    void applySettings(const Settings::LedPowerGroup& settings);
    void storeSettings(Settings::LedPowerGroup* settings);

    auto getPowerLevel0() const -> int;
    auto getPowerLevel1() const -> int;
    auto getPowerLevel2() const -> int;

signals:
    void powerLevelsChanged(int level0, int level1, int level2);

private slots:
    void handleLevelChange();
    void resetValues();

private:
    std::unique_ptr<Ui::LedPowerWidget> m_ui;
};

#endif // SKINCAM_BUILD_LEGACY

SKINCAM_END_NAMESPACE();
