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
#include "skinclassifier.h"

#include <QWidget>

namespace Ui {
    class ClassifierWidget;
}

SKINCAM_BEGIN_NAMESPACE();

class ClassifierWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClassifierWidget(QWidget* parent = nullptr);
    ~ClassifierWidget() override;

    void applySettings(const Settings::ClassifierGroup& settings);
    void storeSettings(Settings::ClassifierGroup* settings);

    void setCurrentMethod(SkinClassifier::Method method);
    auto currentMethod() const -> SkinClassifier::Method;

    auto scaling() const -> int;
    auto threshold() const -> int;

signals:
    void classifierChanged(SkinClassifier::Method classifier);
    void scalingChanged(int scaling);
    void thresholdChanged(int threshold);

private slots:
    void onClassifierChanged();

private:
    std::unique_ptr<Ui::ClassifierWidget> m_ui;
};

SKINCAM_END_NAMESPACE();
