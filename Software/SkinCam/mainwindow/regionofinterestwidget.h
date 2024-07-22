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
    class RegionOfInterestWidget;
}

SKINCAM_BEGIN_NAMESPACE();

class RegionOfInterestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegionOfInterestWidget(QWidget* parent = nullptr);
    ~RegionOfInterestWidget() override;

    void applySettings(const Settings::RegionOfInterestGroup& settings);
    void storeSettings(Settings::RegionOfInterestGroup* settings);

    void setEditChecked(bool checked);

signals:
    void enableToggled(bool enabled);
    void editToggled(bool enable);
    void resetClicked();

private:
    std::unique_ptr<Ui::RegionOfInterestWidget> m_ui;
};

SKINCAM_END_NAMESPACE();
