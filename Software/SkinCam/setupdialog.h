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

#include "settings.h"

#ifdef SKINCAM_BUILD_LEGACY
#include <QComboBox>
#endif

#include <QDialog>

#include <memory>

namespace Ui {
    class SetupDialog;
}

SKINCAM_BEGIN_NAMESPACE();

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog(Settings* settings, QWidget* parent = nullptr);
    ~SetupDialog() override;

private slots:
    void selectCamera();

#ifdef SKINCAM_BUILD_LEGACY
    void refreshControllerPorts();
#endif

    void saveSettings();

#ifdef SKINCAM_BUILD_LEGACY
private:
    void addControllerPortRow();
#endif

private:
    std::unique_ptr<Ui::SetupDialog> m_ui;

#ifdef SKINCAM_BUILD_LEGACY
    QComboBox* m_controllerPort = nullptr;
#endif

    Settings* m_settings;
};

SKINCAM_END_NAMESPACE();
