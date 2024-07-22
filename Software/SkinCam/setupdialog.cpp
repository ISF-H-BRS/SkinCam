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

#include "cameraselectdialog.h"
#include "setupdialog.h"

#include "ui_setupdialog.h"

#ifdef SKINCAM_BUILD_LEGACY
#include <QSerialPortInfo>
#include <QToolButton>
#endif

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

SetupDialog::SetupDialog(Settings* settings, QWidget* parent)
    : QDialog(parent),
      m_ui(std::make_unique<Ui::SetupDialog>()),
      m_settings(settings)
{
    Q_ASSERT(settings != nullptr);

    m_ui->setupUi(this);

#ifdef SKINCAM_BUILD_LEGACY
    addControllerPortRow();
#endif

    Settings::SetupGroup* setup = settings->getSetupGroup();

    m_ui->serialNumber->setText(setup->getSerialNumber());
    m_ui->enableServer->setChecked(setup->getServerEnabled());
    m_ui->serverPort->setValue(setup->getServerPort());
    m_ui->saveOnExit->setChecked(setup->getSaveOnExit());
    m_ui->threadCount->setValue(setup->getThreadCount());
    m_ui->useOpenGL->setChecked(setup->getRenderer() == "OpenGL");

#ifdef SKINCAM_BUILD_LEGACY
    m_controllerPort->setCurrentText(setup->getControllerPort());
#endif

    resize(sizeHint());
    setMinimumSize(sizeHint());

    connect(m_ui->selectButton, SIGNAL(clicked()), this, SLOT(selectCamera()));
    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
}

// ---------------------------------------------------------------------------------------------- //

SetupDialog::~SetupDialog() = default;

// ---------------------------------------------------------------------------------------------- //

void SetupDialog::selectCamera()
{
    QString current = m_ui->serialNumber->text();
    QString selected;

    CameraSelectDialog dialog(current, &selected, this);
    dialog.exec();

    if (!selected.isEmpty())
        m_ui->serialNumber->setText(selected);
}

// ---------------------------------------------------------------------------------------------- //

#ifdef SKINCAM_BUILD_LEGACY
void SetupDialog::refreshControllerPorts()
{
    Q_ASSERT(m_controllerPort != nullptr);

    const QString current = m_controllerPort->currentText();

    m_controllerPort->clear();
    m_controllerPort->addItem(QString());

    const auto ports = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo& info : ports)
        m_controllerPort->addItem(info.portName());

    m_controllerPort->setCurrentText(current);
}
#endif

// ---------------------------------------------------------------------------------------------- //

void SetupDialog::saveSettings()
{
    Settings::SetupGroup* setup = m_settings->getSetupGroup();

    setup->setSerialNumber(m_ui->serialNumber->text());
    setup->setServerEnabled(m_ui->enableServer->isChecked());
    setup->setServerPort(m_ui->serverPort->value());
    setup->setSaveOnExit(m_ui->saveOnExit->isChecked());
    setup->setThreadCount(m_ui->threadCount->value());
    setup->setRenderer(m_ui->useOpenGL->isChecked() ? "OpenGL" : "Software");

#ifdef SKINCAM_BUILD_LEGACY
    setup->setControllerPort(m_controllerPort->currentText());
#endif

    m_settings->saveToDisk();
}

// ---------------------------------------------------------------------------------------------- //

#ifdef SKINCAM_BUILD_LEGACY
void SetupDialog::addControllerPortRow()
{
    Q_ASSERT(m_controllerPort == nullptr);

    m_controllerPort = new QComboBox;
    m_controllerPort->setMinimumWidth(100);

    auto refreshButton = new QToolButton;
    refreshButton->setIcon(QIcon(":/res/refresh.svg"));
    connect(refreshButton, SIGNAL(clicked()), this, SLOT(refreshControllerPorts()));

    auto layout = new QHBoxLayout;
    layout->addWidget(m_controllerPort);
    layout->addWidget(refreshButton);

    auto form = qobject_cast<QFormLayout*>(m_ui->cameraGroup->layout());
    form->addRow("Controller port:", layout);

    refreshControllerPorts();
}
#endif

// ---------------------------------------------------------------------------------------------- //
