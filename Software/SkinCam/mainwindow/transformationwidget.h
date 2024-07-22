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

#include "matrix3.h"
#include "namespace.h"
#include "settings.h"

#include <QWidget>

namespace Ui {
    class TransformationWidget;
}

SKINCAM_BEGIN_NAMESPACE();

class TransformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransformationWidget(QWidget *parent = nullptr);
    ~TransformationWidget() override;

    void applySettings(const Settings::TransformationGroup& settings);
    void storeSettings(Settings::TransformationGroup* settings);

    auto getTransformation() const -> Matrix3f;

signals:
    void enableToggled(bool enabled);
    void transformationChanged(const Matrix3f& matrix);

private slots:
    void handleTransformationChanged();
    void resetValues();

private:
    std::unique_ptr<Ui::TransformationWidget> m_ui;
};

SKINCAM_END_NAMESPACE();
