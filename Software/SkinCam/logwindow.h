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

#include "logger.h"

#include <QWidget>

#include <memory>

namespace Ui {
    class LogWindow;
}

SKINCAM_BEGIN_NAMESPACE();

class LogWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget* parent = nullptr);
    ~LogWindow() override;

signals:
    void visibilityChanged(bool visible);

private slots:
    void onLogAppended(const QString& msg);

private:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    std::unique_ptr<Ui::LogWindow> m_ui;

    class Listener;
    std::unique_ptr<Listener> m_listener;
};

SKINCAM_END_NAMESPACE();
