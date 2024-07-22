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

#include "facedetectoroverlay.h"
#include "facedetectorwrapper.h"
#include "fpswidget.h"
#include "imagewidget.h"
#include "logwindow.h"
#include "regionselector.h"
#include "settings.h"
#include "selectionwindow.h"
#include "skincamsystem.h"
#include "tcpserver.h"

#include <QMainWindow>

#include <memory>

namespace Ui {
    class MainWindow;
}

SKINCAM_BEGIN_NAMESPACE();

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(SkinCamSystem* skinCam, bool testMode = false);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void openCamera();
    void openImage();
    void saveImage();
    void setPaused(bool pause);
    void closeImageProvider();

    void handleNewFrame(const Frame& frame);
    void handleNewCameraStatus(const Camera::Status& status);

#ifndef SKINCAM_BUILD_LEGACY
    void handleNewPowerStatus(const Controller::PowerStatus& status);
#endif

    void handleError(const QString& msg);

    void updateRegionOfInterest();
    void showRegionOfInterestChange(const QPoint& position);

    void handleWhiteSelection(const std::vector<Polygon>& selection);
    void handleSkinSelection(const std::vector<Polygon>& selection);
    void handlePixelSelection(const std::vector<Polygon>& selection);

    void setFaceDetectionEnabled(bool enable);
    void handleFaceDetectionResults(const std::vector<Rect>& realRects,
                                    const std::vector<Rect>& fakeRects);
    void loadSettings();
    void loadSettingsFrom();

    void saveSettings();
    void saveSettingsTo();

    void toggleLogWindow(bool visible);

    void showSetupDialog();
    void showAboutDialog();

#ifdef _WIN32
    void showSupportNotice();
#endif

private:
    auto checkAndGetCameraSetup(CameraSystem::Setup* setup) -> bool;
    void setupImageProcessor();

    void setupToolMenus();
    void setupImageWidgets();
    void setupTcpServer();

    void connectSignals();

    void loadSettingsFromDisk(QString filename = QString());
    void saveSettingsToDisk(QString filename = QString());

    void applySettings();
    void storeSettings();

    void restoreWindow();
    void saveWindow();

    void updateUserInterface();

private:
    std::unique_ptr<Ui::MainWindow> m_ui;

    std::unique_ptr<QMenu> m_saveMenu;
    std::unique_ptr<QMenu> m_loadMenu;

    std::unique_ptr<LogWindow> m_logWindow;

    ImageWidget* m_overviewChannel0 = nullptr;
    ImageWidget* m_overviewChannel1 = nullptr;
    ImageWidget* m_overviewChannel2 = nullptr;
    ImageWidget* m_overviewComposite = nullptr;

    ImageWidget* m_rawDark = nullptr;
    ImageWidget* m_rawImage0 = nullptr;
    ImageWidget* m_rawImage1 = nullptr;
    ImageWidget* m_rawImage2 = nullptr;

    ImageWidget* m_imageChannel0 = nullptr;
    ImageWidget* m_imageChannel1 = nullptr;
    ImageWidget* m_imageChannel2 = nullptr;
    ImageWidget* m_imageComposite = nullptr;
    ImageWidget* m_imageSkinMask = nullptr;
    ImageWidget* m_imageFaceDetection = nullptr;

    std::vector<ImageWidget*> m_imageWidgets = {};

    FaceDetectorOverlay* m_faceDetectorOverlay = nullptr;
    RegionSelector* m_skinMaskSelector = nullptr;

    FpsWidget* m_fpsWidget = nullptr;

    std::unique_ptr<SelectionWindow> m_whiteSelectionWindow;
    std::unique_ptr<SelectionWindow> m_skinSelectionWindow;
    std::unique_ptr<SelectionWindow> m_pixelSelectionWindow;

    std::unique_ptr<TcpServer> m_server;

    std::unique_ptr<FaceDetectorWrapper> m_faceDetector;

    SkinCamSystem* m_skinCam;

    bool m_cameraOpened = false;
    bool m_imageOpened = false;

    bool m_testMode = false;
};

SKINCAM_END_NAMESPACE();
