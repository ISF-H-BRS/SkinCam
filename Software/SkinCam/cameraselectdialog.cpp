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
#include "camerasystem.h"

#include "ui_cameraselectdialog.h"

#include <QPushButton>
#include <QTimer>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

CameraSelectDialog::CameraSelectDialog(const QString& current, QString* selected, QWidget* parent)
    : QDialog(parent),
      m_ui(std::make_unique<Ui::CameraSelectDialog>()),
      m_current(current),
      m_selected(selected)
{
    Q_ASSERT(selected != nullptr);

    m_ui->setupUi(this);
    updateList();

    connect(this, SIGNAL(accepted()), this, SLOT(saveSelection()));
    connect(m_ui->cameraList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(accept()));
}

// ---------------------------------------------------------------------------------------------- //

CameraSelectDialog::~CameraSelectDialog() = default;

// ---------------------------------------------------------------------------------------------- //

void CameraSelectDialog::updateList()
{
    QTimer::singleShot(1s, this, SLOT(updateList()));

    std::vector<std::string> cameras = CameraSystem::getAvailableCameras();

    if (cameras == m_cameras)
        return;

    m_cameras = cameras;

    auto item = m_ui->cameraList->currentItem();
    QString currentSerial = item ? item->data(Qt::UserRole).toString() : "";

    m_ui->cameraList->clear();

    for (const auto& camera : m_cameras)
    {
        const auto serial = QString::fromStdString(camera);

        auto item = new QListWidgetItem;
        item->setData(Qt::UserRole, serial);

        if (serial == m_current)
        {
            item->setText(serial + " (current)");

            QFont font;
            font.setBold(true);
            item->setFont(font);
        }
        else
            item->setText(serial);

        m_ui->cameraList->addItem(item);

        if (serial == currentSerial)
            m_ui->cameraList->setCurrentItem(item);
    }

    if (m_ui->cameraList->count() == 0)
    {
        auto item = new QListWidgetItem("No cameras available");
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);

        m_ui->cameraList->addItem(item);
    }

    auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok);
    Q_ASSERT(button != nullptr);

    button->setEnabled(m_ui->cameraList->count() > 0);
}

// ---------------------------------------------------------------------------------------------- //

void CameraSelectDialog::saveSelection()
{
    auto item = m_ui->cameraList->currentItem();

    if (item)
        *m_selected = item->data(Qt::UserRole).toString();
}

// ---------------------------------------------------------------------------------------------- //
