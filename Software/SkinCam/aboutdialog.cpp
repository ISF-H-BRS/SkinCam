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

#include "aboutdialog.h"
#include "assertions.h"
#include "openclwrapper.h"
#include "vimbaloader.h"

#include "ui_aboutdialog.h"

#include <VimbaCPP/Include/VimbaCPP.h>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent),
      m_ui(std::make_unique<Ui::AboutDialog>())
{
    m_ui->setupUi(this);
    resize(sizeHint());
}

// ---------------------------------------------------------------------------------------------- //

AboutDialog::~AboutDialog() = default;

// ---------------------------------------------------------------------------------------------- //

void AboutDialog::showEvent(QShowEvent* event)
{
    constexpr int AboutIndex   = 0;
    constexpr int SystemIndex  = 1;
    constexpr int LicenseIndex = 2;

    QDialog::showEvent(event);

    m_ui->tabWidget->setCurrentIndex(AboutIndex);
    setupApplicationInfo();

    m_ui->tabWidget->setCurrentIndex(SystemIndex);
    setupOpenCLInfo();
    setupVimbaInfo();

    m_ui->tabWidget->setCurrentIndex(LicenseIndex);
    setupLicenseText();

    m_ui->tabWidget->setCurrentIndex(AboutIndex);
}

// ---------------------------------------------------------------------------------------------- //

void AboutDialog::setupApplicationInfo()
{
    QFont font = m_ui->applicationName->font();
    font.setPointSize(font.pointSize() + 4);
    m_ui->applicationName->setFont(font);

    m_ui->applicationName->setText("SkinCam Viewer");
    m_ui->applicationVersion->setText(QString("Version ") + SKINCAM_VERSION);

#ifdef SKINCAM_BUILD_LEGACY
    m_ui->applicationName->setText(m_ui->applicationName->text() + " (legacy)");
#endif
}

// ---------------------------------------------------------------------------------------------- //

void AboutDialog::setupOpenCLInfo()
{
    QFont font = m_ui->openclTitle->font();
    font.setPointSize(font.pointSize() + 1);
    m_ui->openclTitle->setFont(font);

    const auto& wrapper = OpenCLWrapper::getInstance();

    auto platform = QString::fromStdString(wrapper.platformName());
    auto device = QString::fromStdString(wrapper.deviceName());

    QFontMetrics fm(QApplication::font());

    if (fm.horizontalAdvance(platform) > m_ui->openclPlatform->width())
        m_ui->openclPlatform->setToolTip(platform);

    platform = fm.elidedText(platform, Qt::ElideRight, m_ui->openclPlatform->width());
    m_ui->openclPlatform->setText(platform);

    if (fm.horizontalAdvance(device) > m_ui->openclDevice->width())
        m_ui->openclDevice->setToolTip(device);

    device = fm.elidedText(device, Qt::ElideRight, m_ui->openclDevice->width());
    m_ui->openclDevice->setText(device);
}

// ---------------------------------------------------------------------------------------------- //

void AboutDialog::setupVimbaInfo()
{
    ASSERT(VimbaLoader::isInitialized());

    QFont font = m_ui->vimbaTitle->font();
    font.setPointSize(font.pointSize() + 1);
    m_ui->vimbaTitle->setFont(font);

    auto& vimba = AVT::VmbAPI::VimbaSystem::GetInstance();

    VmbVersionInfo_t info;
    vimba.QueryVersion(info);

    auto major = static_cast<int>(info.major);
    auto minor = static_cast<int>(info.minor);
    auto patch = static_cast<int>(info.patch);

    m_ui->vimbaVersion->setText(QString("%1.%2.%3").arg(major).arg(minor).arg(patch));
}

// ---------------------------------------------------------------------------------------------- //

void AboutDialog::setupLicenseText()
{
    m_ui->licensesText->clear();
    m_ui->licensesText->setAlignment(Qt::AlignHCenter);

    // FreeImage
    m_ui->licensesText->setFontWeight(QFont::Bold);
    m_ui->licensesText->append("FreeImage");

    m_ui->licensesText->setFontWeight(QFont::Normal);
    m_ui->licensesText->append("This software uses the FreeImage open source image library.\n"
                               "See https://freeimage.sourceforge.io for details.\n"
                               "FreeImage is used under the FIPL, version 1.0.");

    m_ui->licensesText->append("");

    // libfacedetection
    m_ui->licensesText->setFontWeight(QFont::Bold);
    m_ui->licensesText->append("libfacedetection");

    m_ui->licensesText->setFontWeight(QFont::Normal);
    m_ui->licensesText->append("This software uses libfacedetection, an open source library for "
                               "CNN-based face detection in images.\n"
                               "See https://github.com/ShiqiYu/libfacedetection for details.");

    m_ui->licensesText->append("");

    // libusb
    m_ui->licensesText->setFontWeight(QFont::Bold);
    m_ui->licensesText->append("libusb");

    m_ui->licensesText->setFontWeight(QFont::Normal);
    m_ui->licensesText->append("This software uses libusb, a C library that provides generic "
                               "access to USB devices.\n"
                               "See https://libusb.info for details.");

    m_ui->licensesText->append("");

    // libzip
    m_ui->licensesText->setFontWeight(QFont::Bold);
    m_ui->licensesText->append("libzip");

    m_ui->licensesText->setFontWeight(QFont::Normal);
    m_ui->licensesText->append("This software uses libzip, a C library for reading, creating, "
                               "and modifying zip archives.\n"
                               "See https://libzip.org for details.");

    m_ui->licensesText->append("");

    // Qt
    m_ui->licensesText->setFontWeight(QFont::Bold);
    m_ui->licensesText->append("Qt");

    m_ui->licensesText->setFontWeight(QFont::Normal);
    m_ui->licensesText->append("This software uses Qt, the cross-platform software development "
                               "framework.\n"
                               "See https://doc.qt.io for details.\n"
                               "Qt is used under the LGPL, version 3.");

    m_ui->licensesText->append("");

    // Vimba
    m_ui->licensesText->setFontWeight(QFont::Bold);
    m_ui->licensesText->append("Vimba");

    m_ui->licensesText->setFontWeight(QFont::Normal);
    m_ui->licensesText->append("This software uses the Vimba SDK for machine vision cameras.\n"
                               "See https://www.alliedvision.com/en/products/vimba-sdk for "
                               "details.");

    m_ui->licensesText->moveCursor(QTextCursor::Start);
}

// ---------------------------------------------------------------------------------------------- //
