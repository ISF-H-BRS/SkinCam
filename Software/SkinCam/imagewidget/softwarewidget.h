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

#include "backend.h"

#include <QWidget>

SKINCAM_BEGIN_NAMESPACE();

class ImageWidget::SoftwareWidget : public QWidget, public Backend
{
    Q_OBJECT

public:
    SoftwareWidget(QWidget* parent);

    void setImage(const QImage& image) override;
    void setMirrored(bool mirror) override;

signals:
    void paintFinished(QWidget* widget);

private:
    void paintEvent(QPaintEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QImage m_image;
    bool m_mirrored = false;
};

SKINCAM_END_NAMESPACE();
