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

#include "assertions.h"
#include "pixelexportdialog.h"

#include "ui_pixelexportdialog.h"

#include <QPushButton>
#include <QSettings>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

PixelExportDialog::PixelExportDialog(Selection* selection, QWidget* parent)
    : QDialog(parent),
      m_ui(std::make_unique<Ui::PixelExportDialog>()),
      m_selection(selection)
{
    ASSERT_NOT_NULL(selection);

    m_ui->setupUi(this);

    adjustSize();
    restoreSettings();
    updateButtons();

    connect(m_ui->coords, SIGNAL(toggled(bool)), this, SLOT(updateButtons()));
    connect(m_ui->values, SIGNAL(toggled(bool)), this, SLOT(updateButtons()));
    connect(m_ui->diffs, SIGNAL(toggled(bool)), this, SLOT(updateButtons()));

    connect(this, SIGNAL(accepted()), this, SLOT(saveData()));
}

// ---------------------------------------------------------------------------------------------- //

PixelExportDialog::~PixelExportDialog() = default;

// ---------------------------------------------------------------------------------------------- //

void PixelExportDialog::updateButtons()
{
    const bool ok = m_ui->coords->isChecked() ||
                    m_ui->values->isChecked() ||
                    m_ui->diffs->isChecked();

    m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

// ---------------------------------------------------------------------------------------------- //

void PixelExportDialog::saveData()
{
    m_selection->coordinates = m_ui->coords->isChecked();
    m_selection->values = m_ui->values->isChecked();
    m_selection->normalizedDifferences = m_ui->diffs->isChecked();

    saveSettings();
}

// ---------------------------------------------------------------------------------------------- //

void PixelExportDialog::saveSettings()
{
    QSettings settings;
    settings.beginGroup("PixelExport");

    settings.setValue("Coords", m_ui->coords->isChecked());
    settings.setValue("Values", m_ui->values->isChecked());
    settings.setValue("Diffs", m_ui->diffs->isChecked());
}

// ---------------------------------------------------------------------------------------------- //

void PixelExportDialog::restoreSettings()
{
    QSettings settings;
    settings.beginGroup("PixelExport");

    m_ui->coords->setChecked(settings.value("Coords", m_ui->coords->isChecked()).toBool());
    m_ui->values->setChecked(settings.value("Values", m_ui->values->isChecked()).toBool());
    m_ui->diffs->setChecked(settings.value("Diffs", m_ui->diffs->isChecked()).toBool());
}

// ---------------------------------------------------------------------------------------------- //
