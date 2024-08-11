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
#include "mainwindow.h"
#include "pixelexportdialog.h"
#include "pixelexporter.h"
#include "setupdialog.h"

#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QProgressDialog>
#include <QToolButton>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

namespace {
#ifdef SKINCAM_BUILD_LEGACY
    constexpr const char* SettingsFileName = "SettingsLegacy.ini";
    constexpr const char* WindowTitle = "ISF SkinCam Viewer (legacy)";
#else
    constexpr const char* SettingsFileName = "Settings.ini";
    constexpr const char* WindowTitle = "ISF SkinCam Viewer";
#endif

    constexpr int StatusbarMessageTimeout = 2000;
}

// ---------------------------------------------------------------------------------------------- //

MainWindow::MainWindow(SkinCamSystem* skinCam, bool testMode)
    : m_ui(std::make_unique<Ui::MainWindow>()),
      m_saveMenu(std::make_unique<QMenu>()),
      m_loadMenu(std::make_unique<QMenu>()),
      m_logWindow(std::make_unique<LogWindow>()),
      m_whiteSelectionWindow(std::make_unique<SelectionWindow>()),
      m_skinSelectionWindow(std::make_unique<SelectionWindow>()),
      m_pixelSelectionWindow(std::make_unique<SelectionWindow>()),
      m_skinCam(skinCam),
      m_testMode(testMode)
{
    m_ui->setupUi(this);

#ifndef SKINCAM_ENABLE_VIMBA
    m_ui->actionOpenCamera->setVisible(false);
    m_ui->actionSaveImage->setVisible(false);
    m_ui->actionPause->setVisible(false);

    const int controlIndex = m_ui->controlTabWidget->indexOf(m_ui->cameraTab);
    m_ui->controlTabWidget->setTabVisible(controlIndex, false);
#endif

#ifdef SKINCAM_BUILD_LEGACY
    m_ui->ledPowerGroup->hide();
#endif

    setWindowTitle(WindowTitle);

    loadSettingsFromDisk();

    setupToolMenus();
    setupImageWidgets();
    setupTcpServer();

    const int faceDetectionIndex = m_ui->imageTabWidget->indexOf(m_ui->faceDetectionTab);
    m_ui->imageTabWidget->setTabEnabled(faceDetectionIndex, false);

    m_ui->controlTabWidget->setMinimumWidth(m_ui->controlTabWidget->sizeHint().width());
    m_ui->toolBar->setMinimumWidth(m_ui->toolBar->sizeHint().width());

    m_ui->statusbar->setStyleSheet("QStatusBar::item{border: 0px}");

    m_fpsWidget = new FpsWidget;
    m_ui->statusbar->addPermanentWidget(m_fpsWidget);

    m_whiteSelectionWindow->setWindowTitle("White Selection");
    m_whiteSelectionWindow->setVisible(false);

    m_skinSelectionWindow->setWindowTitle("Skin Selection");
    m_skinSelectionWindow->setVisible(false);

    m_pixelSelectionWindow->setWindowTitle("Pixel Selection");
    m_pixelSelectionWindow->setAcceptButtonText("Export Selection");
    m_pixelSelectionWindow->setVisible(false);

    const bool serverEnabled = Settings::getConstInstance().getSetupGroup().getServerEnabled();
    m_ui->detectionThresholdsGroup->setVisible(serverEnabled);

    connectSignals();

    restoreWindow();
    applySettings();

    updateUserInterface();

#ifdef _WIN32
    QTimer::singleShot(0, this, SLOT(showSupportNotice()));
#endif
}

// ---------------------------------------------------------------------------------------------- //

MainWindow::~MainWindow()
{
    saveWindow();

    if (Settings::getConstInstance().getSetupGroup().getSaveOnExit())
        saveSettings();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::closeEvent(QCloseEvent* event)
{
    m_logWindow->close();
    m_whiteSelectionWindow->close();
    m_skinSelectionWindow->close();

    QMainWindow::closeEvent(event);
}

// ---------------------------------------------------------------------------------------------- //

auto MainWindow::checkAndGetCameraSetup(CameraSystem::Setup* setup) -> bool
{
    const auto& settings = Settings::getConstInstance().getSetupGroup();

#ifdef SKINCAM_BUILD_LEGACY
    const QString controllerPort = settings.getControllerPort();

    if (controllerPort.isEmpty())
    {
        QMessageBox::information(this, "Note", "No serial port is currently assigned to the "
                                               "camera controller. Please use the setup dialog "
                                               "to assign one.");
        return false;
    }
#endif

    const QString serialNumber = settings.getSerialNumber();

    if (serialNumber.isEmpty())
    {
        QMessageBox::information(this, "Note", "No serial number is currently assigned to the "
                                               "camera. Please use the setup dialog "
                                               "to assign one.");
        return false;
    }

    if (!CameraSystem::isCameraAvailable(serialNumber.toStdString()))
    {
        QMessageBox::warning(this, "Error", "The camera with the serial number " + serialNumber +
                                            " is not available.");
        return false;
    }

    const auto serial = serialNumber.toStdString();
    const auto exposure = m_ui->acquisitionWidget->getExposureTime();
    const auto setpoint = m_ui->advancedWidget->getTemperatureSetpoint();
    const auto gain = m_ui->acquisitionWidget->getCameraGain();

    setup->cameraSetup = { serial, exposure, setpoint, gain };

    setup->refreshRate = m_ui->acquisitionWidget->getFrameRate();
    setup->illuminationTime = m_ui->acquisitionWidget->getIlluminationTime();

#ifdef SKINCAM_BUILD_LEGACY
    setup->controllerPort = controllerPort.toStdString();
#else
    setup->powerLevel0 = m_ui->ledPowerWidget->getPowerLevel0();
    setup->powerLevel1 = m_ui->ledPowerWidget->getPowerLevel1();
    setup->powerLevel2 = m_ui->ledPowerWidget->getPowerLevel2();
#endif

    return true;
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::setupImageProcessor()
{
    const char* text = "<b>Preparing image processor, please wait ...</b>";

    QProgressDialog dialog(text, QString(), 0, 0, this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.show();

    m_skinCam->setupImageProcessor();
    m_fpsWidget->setMaximum(m_ui->acquisitionWidget->getFrameRate());

    updateUserInterface();
    setPaused(false);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::openCamera()
{
    m_ui->acquisitionWidget->setMinMaxFrameRate(CameraSystem::MinimumRefreshRate,
                                                CameraSystem::MaximumRefreshRate);
    CameraSystem::Setup setup;

    if (!checkAndGetCameraSetup(&setup))
        return;

    try {
        const char* text = "<b>Connecting to camera system, please wait ...</b>";

        QProgressDialog dialog(text, QString(), 0, 0, this);
        dialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        dialog.setWindowModality(Qt::WindowModal);
        dialog.show();

        m_skinCam->openCamera(setup);
        m_cameraOpened = true;

        dialog.close();

        setupImageProcessor();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::openImage()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Image File",
                                                    QString(), "Image Archives (*.zip)");
    if (filename.isEmpty())
        return;

    m_ui->acquisitionWidget->setMinMaxFrameRate(ImageReader::MinimumRefreshRate,
                                                ImageReader::MaximumRefreshRate);
    try {
        m_skinCam->openImage(filename, m_ui->acquisitionWidget->getFrameRate());
        m_imageOpened = true;

        setupImageProcessor();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::saveImage()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Image File",
                                                    QString(), "Image Archives (*.zip)");
    if (filename.isEmpty())
        return;

    if (QFile::exists(filename))
        QFile::remove(filename);

    try {
        m_skinCam->saveImage(filename);
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::setPaused(bool pause)
{
    if (pause)
        m_skinCam->stop();
    else
    {
        m_ui->statusWidget->reset();
        m_skinCam->start();
    }

    // Don't allow changes during capture
    m_ui->acquisitionWidget->setEnabled(pause);
    m_ui->ledPowerWidget->setEnabled(pause);
    m_ui->advancedWidget->setEnabled(pause);
    m_ui->cameraInfoLabel->setVisible(!pause);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::closeImageProvider()
{
    if (m_server)
        m_server->setSkinDetected(false);

    m_whiteSelectionWindow->close();
    m_skinSelectionWindow->close();
    m_pixelSelectionWindow->close();

    try {
        m_skinCam->close();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }

    m_cameraOpened = false;
    m_imageOpened = false;

    updateUserInterface();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::handleNewFrame(const Frame& frame)
{
    if (m_imageOpened && !m_testMode) // Only one image needed
    {
        setPaused(true);
        m_ui->actionPause->setEnabled(false);
    }

    m_rawDark->setImage(frame.rawDarkImage());
    m_rawImage0->setImage(frame.raw0Image());
    m_rawImage1->setImage(frame.raw1Image());
    m_rawImage2->setImage(frame.raw2Image());

    {
        const QImage& image = frame.channel0Image();
        m_imageChannel0->setImage(image);
        m_overviewChannel0->setImage(image);
    }

    {
        const QImage& image = frame.channel1Image();
        m_imageChannel1->setImage(image);
        m_overviewChannel1->setImage(image);
    }

    {
        const QImage& image = frame.channel2Image();
        m_imageChannel2->setImage(image);
        m_overviewChannel2->setImage(image);
    }

    {
        const QImage& image = frame.compositeImage();
        m_overviewComposite->setImage(image);
        m_imageComposite->setImage(image);
        m_imageFaceDetection->setImage(image);
        m_whiteSelectionWindow->setImage(image);
        m_skinSelectionWindow->setImage(image);
        m_pixelSelectionWindow->setImage(image);
    }

    {
        const QImage& image = frame.skinMaskImage();
        m_imageSkinMask->setImage(image);

        if (m_faceDetector)
            m_faceDetectorOverlay->setSkinMask(image);
    }

    const auto permille = static_cast<int>(std::round(frame.skinRatio() * 1000.0));
    m_imageSkinMask->setText(QString("Skin ratio: %1 ‰").arg(permille));

    m_fpsWidget->setValues(m_skinCam->fpsProcessed(), m_skinCam->fpsDisplayed());

    if (m_server)
    {
        const double threshold = m_ui->detectionThresholdsWidget->getSkinRatio();
        m_server->setSkinDetected(frame.skinRatio() >= threshold);
    }

    if (m_faceDetector)
        m_faceDetector->setNextFrame(frame);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::handleNewCameraStatus(const Camera::Status& status)
{
    constexpr size_t MaximumPacketLoss = 50; // Permille

    m_ui->statusWidget->setCameraStatus(status);

    if (m_ui->statusWidget->currentPacketLoss() > MaximumPacketLoss)
    {
        const QString msg = "Packet loss exceeded tolerable limit.\n\n"
                            "Please check the ethernet connection and ensure a sufficient "
                            "number of receive buffers for the network adapter. Try reducing "
                            "the target frame rate if the problem persists.\n\n"
                            "Consult the chapter \"Configuring the host computer\" in the "
                            "Goldeye G/CL Technical Manual for further instructions.";
        handleError(msg);
    }
}

// ---------------------------------------------------------------------------------------------- //

#ifndef SKINCAM_BUILD_LEGACY
void MainWindow::handleNewPowerStatus(const Controller::PowerStatus& status)
{
    m_ui->statusWidget->setPowerStatus(status);
}
#endif

// ---------------------------------------------------------------------------------------------- //

void MainWindow::handleError(const QString& msg)
{
    closeImageProvider();
    QMessageBox::critical(this, "Error", msg);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::updateRegionOfInterest()
{
    std::vector<Vec2i> vertices;

    if (m_skinMaskSelector->isVisible())
        vertices = m_skinMaskSelector->getPolygon().vertices();

    m_skinCam->updateRegionOfInterest(vertices);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::showRegionOfInterestChange(const QPoint& position)
{
    const auto msg = QString("New position at (%1, %2)").arg(position.x()).arg(position.y());
    m_ui->statusbar->showMessage(msg, StatusbarMessageTimeout);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::handleWhiteSelection(const std::vector<Polygon>& selection)
{
    try {
        auto factors = m_skinCam->processWhiteSelection(selection);

        auto factor0 = static_cast<double>(factors.x());
        auto factor1 = static_cast<double>(factors.y());
        auto factor2 = static_cast<double>(factors.z());

        auto settings = Settings::getInstance()->getWhiteCalibrationGroup();
        settings->setFactor0(factor0);
        settings->setFactor1(factor1);
        settings->setFactor2(factor2);

        m_ui->whiteCalibrationWidget->applySettings(*settings);
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::handleSkinSelection(const std::vector<Polygon>& selection)
{
    try {
        auto [mean, variance] = m_skinCam->processSkinSelection(selection);
        m_skinCam->updateClassifierParameters(mean, variance);

        auto settings = Settings::getInstance()->getSkinClassificationGroup();

        settings->setMean0(static_cast<double>(mean.x()));
        settings->setMean1(static_cast<double>(mean.y()));
        settings->setMean2(static_cast<double>(mean.z()));

        settings->setVariance0(static_cast<double>(variance.x()));
        settings->setVariance1(static_cast<double>(variance.y()));
        settings->setVariance2(static_cast<double>(variance.z()));
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::handlePixelSelection(const std::vector<Polygon>& selection)
{
    PixelExport::Selection fields = {};
    PixelExportDialog dialog(&fields, m_pixelSelectionWindow.get());

    const int result = dialog.exec();

    if (result != PixelExportDialog::Accepted)
        return;

    const QString filename = QFileDialog::getSaveFileName(m_pixelSelectionWindow.get(),
                                                          "Export Data", QString(),
                                                          "CSV Files (*.csv)");
    if (filename.isEmpty())
        return;

    try {
        const PixelExport::Data data = m_skinCam->processPixelSelection(selection, fields);
        PixelExporter::write(filename.toStdString(), data);

        const QString msg = "Selection saved to " + filename + ".";
        m_ui->statusbar->showMessage(msg, StatusbarMessageTimeout);
    }
    catch (const std::exception& e) {
        handleError(e.what());
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::setFaceDetectionEnabled(bool enable)
{
    if (enable)
    {
        m_faceDetector = std::make_unique<FaceDetectorWrapper>();

        m_faceDetector->setSkinValueThreshold(m_ui->classifierWidget->threshold());
        m_faceDetector->setMinimumSkinRatio(m_ui->faceDetectionWidget->minimumSkinRatio());

        connect(m_ui->classifierWidget, SIGNAL(thresholdChanged(int)),
                m_faceDetector.get(), SLOT(setSkinValueThreshold(int)));
        connect(m_ui->faceDetectionWidget, SIGNAL(minimumSkinRatioChanged(int)),
                m_faceDetector.get(), SLOT(setMinimumSkinRatio(int)));

        connect(m_faceDetector.get(), SIGNAL(resultsAvailable(std::vector<Rect>,std::vector<Rect>)),
                this, SLOT(handleFaceDetectionResults(std::vector<Rect>,std::vector<Rect>)));

        if (m_ui->imageTabWidget->currentWidget() == m_ui->compositeTab)
            m_ui->imageTabWidget->setCurrentWidget(m_ui->faceDetectionTab);

        m_skinCam->reprocessIfPaused();
    }
    else
    {
        m_faceDetector = nullptr;
        m_faceDetectorOverlay->clear();

        if (m_ui->imageTabWidget->currentWidget() == m_ui->faceDetectionTab)
            m_ui->imageTabWidget->setCurrentWidget(m_ui->compositeTab);
    }

    const int tabIndex = m_ui->imageTabWidget->indexOf(m_ui->faceDetectionTab);
    m_ui->imageTabWidget->setTabEnabled(tabIndex, enable);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::handleFaceDetectionResults(const std::vector<Rect>& realRects,
                                            const std::vector<Rect>& fakeRects)
{
    m_faceDetectorOverlay->setRects(realRects, fakeRects);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::loadSettings()
{
    loadSettingsFromDisk();
    applySettings();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::loadSettingsFrom()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load Settings",
                                                    QString(), "Settings (*.ini)");
    if (filename.isEmpty())
        return;

    loadSettingsFromDisk(filename);
    applySettings();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::saveSettings()
{
    storeSettings();
    saveSettingsToDisk();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::saveSettingsTo()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Settings",
                                                    QString(), "Settings (*.ini)");
    if (filename.isEmpty())
        return;

    storeSettings();
    saveSettingsToDisk(filename);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::toggleLogWindow(bool visible)
{
    m_logWindow->setVisible(visible);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::showSetupDialog()
{
    SetupDialog dialog(Settings::getInstance(), this);

    int result = dialog.exec();

    if (result == SetupDialog::Accepted)
        saveSettingsToDisk();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::showAboutDialog()
{
    AboutDialog dialog(this);
    dialog.exec();
}

// ---------------------------------------------------------------------------------------------- //

#ifdef _WIN32
void MainWindow::showSupportNotice()
{
    QSettings settings;

    if (!settings.value("showSupportNotice", true).toBool())
        return;

    const char* msg = "<b>Note:</b><br>"
                      "Running this software on Windows is discouraged and unsupported!<br><br>"
                      "While it should work reasonably well on most mid- to high-end systems, "
                      "the processing overhead inherent to the operating system conflicts with "
                      "the real-time requirements of this application and may have a significant "
                      "impact on its reliability.<br><br>"
                      "For best performance, please consider using the Linux version instead.<br>";

    QMessageBox messageBox(QMessageBox::Warning, "Unsupported System", msg, QMessageBox::Ok, this);
    QCheckBox checkBox("Don't show this message again");

    messageBox.setCheckBox(&checkBox);
    messageBox.exec();

    if (checkBox.isChecked())
        settings.setValue("showSupportNotice", false);
}
#endif

// ---------------------------------------------------------------------------------------------- //

void MainWindow::setupToolMenus()
{
    // Save as
    auto action = m_saveMenu->addAction("Save as...");
    connect(action, SIGNAL(triggered()), this, SLOT(saveSettingsTo()));

    auto widget = m_ui->toolBar->widgetForAction(m_ui->actionSaveSettings);
    auto button = qobject_cast<QToolButton*>(widget);
    Q_ASSERT(button != nullptr);

    button->setMenu(m_saveMenu.get());
    button->setPopupMode(QToolButton::MenuButtonPopup);

    // Load from
    action = m_loadMenu->addAction("Load from...");
    connect(action, SIGNAL(triggered()), this, SLOT(loadSettingsFrom()));

    widget = m_ui->toolBar->widgetForAction(m_ui->actionLoadSettings);
    button = qobject_cast<QToolButton*>(widget);
    Q_ASSERT(button != nullptr);

    button->setMenu(m_loadMenu.get());
    button->setPopupMode(QToolButton::MenuButtonPopup);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::setupImageWidgets()
{
    const auto& settings = Settings::getConstInstance();

    ImageWidget::Renderer renderer = (settings.getSetupGroup().getRenderer() == "OpenGL")
                                     ? ImageWidget::Renderer::OpenGL
                                     : ImageWidget::Renderer::Software;

    auto setupWidget = [&](ImageWidget*& widget, QWidget* parent, const QString& text)
    {
        widget = new ImageWidget(renderer, parent);
        widget->setText(text);
        parent->layout()->addWidget(widget);
    };

    setupWidget(m_overviewChannel0, m_ui->overviewImage0, "1050 nm");
    setupWidget(m_overviewChannel1, m_ui->overviewImage1, "1300 nm");
    setupWidget(m_overviewChannel2, m_ui->overviewImage2, "1550 nm");
    setupWidget(m_overviewComposite, m_ui->overviewComposite, "Composite");

    setupWidget(m_rawDark, m_ui->rawDark, "Dark");
    setupWidget(m_rawImage0, m_ui->rawImage0, "1050 nm");
    setupWidget(m_rawImage1, m_ui->rawImage1, "1300 nm");
    setupWidget(m_rawImage2, m_ui->rawImage2, "1550 nm");

    setupWidget(m_imageChannel0, m_ui->imageTab0, "1050 nm");
    setupWidget(m_imageChannel1, m_ui->imageTab1, "1300 nm");
    setupWidget(m_imageChannel2, m_ui->imageTab2, "1550 nm");
    setupWidget(m_imageComposite, m_ui->compositeTab, "Composite");
    setupWidget(m_imageSkinMask, m_ui->skinMaskTab, "");
    setupWidget(m_imageFaceDetection, m_ui->faceDetectionTab, "Face Detection");

    m_imageWidgets = {
        m_rawDark, m_rawImage0, m_rawImage1, m_rawImage2,
        m_overviewChannel0, m_overviewChannel1, m_overviewChannel2, m_overviewComposite,
        m_imageChannel0, m_imageChannel1, m_imageChannel2,
        m_imageComposite, m_imageSkinMask, m_imageFaceDetection
    };

    m_faceDetectorOverlay = new FaceDetectorOverlay(ImageWidth, ImageHeight, m_imageFaceDetection);
    m_skinMaskSelector = new RegionSelector(ImageWidth, ImageHeight, m_imageSkinMask);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::setupTcpServer()
{
    ASSERT_NULL(m_server);

    const Settings::SetupGroup& setup = Settings::getConstInstance().getSetupGroup();

    const bool enabled = setup.getServerEnabled();
    const int port = setup.getServerPort();

    if (enabled)
    {
        try {
            m_server = std::make_unique<TcpServer>(static_cast<quint16>(port));
        }
        catch (const std::exception& e)
        {
            m_ui->detectionThresholdsGroup->hide();
            QMessageBox::critical(this, "Error", e.what());
        }
    }
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::connectSignals()
{
    // Tool buttons
    connect(m_ui->actionOpenCamera, SIGNAL(triggered()), this, SLOT(openCamera()));
    connect(m_ui->actionOpenImage, SIGNAL(triggered()), this, SLOT(openImage()));
    connect(m_ui->actionSaveImage, SIGNAL(triggered()), this, SLOT(saveImage()));
    connect(m_ui->actionPause, SIGNAL(triggered(bool)), this, SLOT(setPaused(bool)));
    connect(m_ui->actionCloseImage, SIGNAL(triggered()), this, SLOT(closeImageProvider()));

    for (auto image : m_imageWidgets)
        connect(m_ui->actionCloseImage, SIGNAL(triggered()), image, SLOT(clear()));

    connect(m_ui->actionSetup, SIGNAL(triggered()), this, SLOT(showSetupDialog()));
    connect(m_ui->actionSaveSettings, SIGNAL(triggered()), this, SLOT(saveSettings()));
    connect(m_ui->actionLoadSettings, SIGNAL(triggered()), this, SLOT(loadSettings()));

    connect(m_ui->actionExportPixels, SIGNAL(triggered(bool)),
            m_pixelSelectionWindow.get(), SLOT(setVisible(bool)));
    connect(m_pixelSelectionWindow.get(), SIGNAL(visiblilityChanged(bool)),
            m_ui->actionExportPixels, SLOT(setChecked(bool)));
    connect(m_pixelSelectionWindow.get(), SIGNAL(selectionAccepted(std::vector<Polygon>)),
            this, SLOT(handlePixelSelection(std::vector<Polygon>)));

    connect(m_ui->actionShowLog, SIGNAL(triggered(bool)), this, SLOT(toggleLogWindow(bool)));
    connect(m_logWindow.get(), SIGNAL(visibilityChanged(bool)),
            m_ui->actionShowLog, SLOT(setChecked(bool)));

    connect(m_ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutDialog()));

    // Frames, status & errors
    connect(m_skinCam, SIGNAL(newFrameAvailable(Frame)), this, SLOT(handleNewFrame(Frame)));
    connect(m_skinCam, SIGNAL(newCameraStatusAvailable(Camera::Status)),
            this, SLOT(handleNewCameraStatus(Camera::Status)));

#ifndef SKINCAM_BUILD_LEGACY
    connect(m_skinCam, SIGNAL(newPowerStatusAvailable(Controller::PowerStatus)),
            this, SLOT(handleNewPowerStatus(Controller::PowerStatus)));
#endif

    connect(m_skinCam, SIGNAL(error(QString)), this, SLOT(handleError(QString)));

    // Image widgets
    for (auto image : m_imageWidgets)
        connect(image, SIGNAL(fullscreenChanged(bool)), this, SLOT(setHidden(bool)));

    // Color adjustment
    connect(m_ui->colorAdjustmentWidget, SIGNAL(enableToggled(bool)),
            m_skinCam, SLOT(setColorAdjustmentEnabled(bool)));
    connect(m_ui->colorAdjustmentWidget, SIGNAL(valuesChanged(int,int)),
            m_skinCam, SLOT(updateColorAdjustment(int,int)));

    // Smoothing
    connect(m_ui->smoothingWidget, SIGNAL(enableToggled(bool)),
            m_skinCam, SLOT(setSmoothingEnabled(bool)));
    connect(m_ui->smoothingWidget, SIGNAL(radiusChanged(uint)),
            m_skinCam, SLOT(updateSmoothing(uint)));

    // Sharpening
    connect(m_ui->sharpeningWidget, SIGNAL(enableToggled(bool)),
            m_skinCam, SLOT(setSharpeningEnabled(bool)));
    connect(m_ui->sharpeningWidget, SIGNAL(valuesChanged(uint,float,float)),
            m_skinCam, SLOT(updateSharpening(uint,float,float)));

    // Averaging
    connect(m_ui->averageWidget, SIGNAL(enableToggled(bool)),
            m_skinCam, SLOT(setAveragingEnabled(bool)));
    connect(m_ui->averageWidget, SIGNAL(sampleCountChanged(int)),
            m_skinCam, SLOT(updateAveraging(int)));

    // Mirroring
    for (auto image : m_imageWidgets)
        connect(m_ui->enableMirroring, SIGNAL(toggled(bool)), image, SLOT(setMirrored(bool)));

    connect(m_ui->enableMirroring, SIGNAL(toggled(bool)),
            m_faceDetectorOverlay, SLOT(setMirrored(bool)));

    connect(m_ui->enableMirroring, SIGNAL(toggled(bool)),
            m_skinMaskSelector, SLOT(setMirrored(bool)));

    // Barrel
    connect(m_ui->barrelWidget, SIGNAL(enableToggled(bool)),
            m_skinCam, SLOT(setBarrelEnabled(bool)));
    connect(m_ui->barrelWidget, SIGNAL(factorChanged(int)),
            m_skinCam, SLOT(updateBarrel(int)));

    // Transformation
    connect(m_ui->transformationWidget, SIGNAL(enableToggled(bool)),
            m_skinCam, SLOT(setTransformationEnabled(bool)));
    connect(m_ui->transformationWidget, SIGNAL(transformationChanged(Matrix3f)),
            m_skinCam, SLOT(updateTransformation(Matrix3f)));

    // Crop
    connect(m_ui->cropWidget, SIGNAL(enableToggled(bool)), m_skinCam, SLOT(setCropEnabled(bool)));
    connect(m_ui->cropWidget, SIGNAL(valuesChanged(int,int,int,int)),
            m_skinCam, SLOT(updateCrop(int,int,int,int)));

    // White calibration
    connect(m_ui->whiteCalibrationWidget, SIGNAL(enableToggled(bool)),
            m_skinCam, SLOT(setWhiteCalibrationEnabled(bool)));
    connect(m_ui->whiteCalibrationWidget, SIGNAL(factorsChanged(double,double,double)),
            m_skinCam, SLOT(updateWhiteCalibration(double,double,double)));
    connect(m_ui->whiteCalibrationWidget, SIGNAL(enableToggled(bool)),
            m_ui->selectWhiteButton, SLOT(setEnabled(bool)));

    connect(m_ui->selectWhiteButton, SIGNAL(clicked(bool)),
            m_whiteSelectionWindow.get(), SLOT(setVisible(bool)));
    connect(m_whiteSelectionWindow.get(), SIGNAL(visiblilityChanged(bool)),
            m_ui->selectWhiteButton, SLOT(setChecked(bool)));

    connect(m_whiteSelectionWindow.get(), SIGNAL(selectionAccepted(std::vector<Polygon>)),
            this, SLOT(handleWhiteSelection(std::vector<Polygon>)));

    // Classifier
    connect(m_ui->classifierWidget, SIGNAL(classifierChanged(SkinClassifier::Method)),
            m_skinCam, SLOT(setClassifier(SkinClassifier::Method)));

    connect(m_ui->classifierWidget, SIGNAL(scalingChanged(int)),
            m_skinCam, SLOT(updateClassifierScaling(int)));
    connect(m_ui->classifierWidget, SIGNAL(thresholdChanged(int)),
            m_skinCam, SLOT(updateClassifierThreshold(int)));

    // Region of interest
    connect(m_ui->regionOfInterestWidget, SIGNAL(enableToggled(bool)),
            m_skinMaskSelector, SLOT(setVisible(bool)));
    connect(m_ui->regionOfInterestWidget, SIGNAL(editToggled(bool)),
            m_skinMaskSelector, SLOT(setEditEnabled(bool)));
    connect(m_ui->regionOfInterestWidget, SIGNAL(resetClicked()),
            m_skinMaskSelector, SLOT(reset()));

    // Region-of-interest overlay
    connect(m_skinMaskSelector, SIGNAL(selectionChanged()), this, SLOT(updateRegionOfInterest()));
    connect(m_skinMaskSelector, SIGNAL(nodePositionChanged(QPoint)),
            this, SLOT(showRegionOfInterestChange(QPoint)));

    // Skin selector
    connect(m_ui->selectSkinButton, SIGNAL(clicked(bool)),
            m_skinSelectionWindow.get(), SLOT(setVisible(bool)));
    connect(m_skinSelectionWindow.get(), SIGNAL(visiblilityChanged(bool)),
            m_ui->selectSkinButton, SLOT(setChecked(bool)));
    connect(m_skinSelectionWindow.get(), SIGNAL(selectionAccepted(std::vector<Polygon>)),
            this, SLOT(handleSkinSelection(std::vector<Polygon>)));

    // Face detection
    connect(m_ui->faceDetectionWidget, SIGNAL(enableToggled(bool)),
            m_faceDetectorOverlay, SLOT(setVisible(bool)));
    connect(m_ui->faceDetectionWidget, SIGNAL(enableToggled(bool)),
            this, SLOT(setFaceDetectionEnabled(bool)));
    connect(m_ui->faceDetectionWidget, SIGNAL(minimumSkinRatioChanged(int)),
            m_skinCam, SLOT(reprocessIfPaused()));
    connect(m_ui->faceDetectionWidget, SIGNAL(highlightSkinPixelsToggled(bool)),
            m_faceDetectorOverlay, SLOT(setSkinPixelsHighlighted(bool)));

    // Acquisition control
    connect(m_ui->acquisitionWidget, SIGNAL(frameRateChanged(int)),
            m_skinCam, SLOT(updateTargetFrameRate(int)));
    connect(m_ui->acquisitionWidget, SIGNAL(frameRateChanged(int)),
            m_fpsWidget, SLOT(setMaximum(int)));
    connect(m_ui->acquisitionWidget, SIGNAL(illuminationTimeChanged(std::chrono::microseconds)),
            m_skinCam, SLOT(updateIlluminationTime(std::chrono::microseconds)));
    connect(m_ui->acquisitionWidget, SIGNAL(exposureTimeChanged(std::chrono::microseconds)),
            m_skinCam, SLOT(updateExposureTime(std::chrono::microseconds)));
    connect(m_ui->acquisitionWidget, SIGNAL(cameraGainChanged(Camera::Gain)),
            m_skinCam, SLOT(updateGain(Camera::Gain)));

#ifndef SKINCAM_BUILD_LEGACY
    // LED power
    connect(m_ui->ledPowerWidget, SIGNAL(powerLevelsChanged(int,int,int)),
            m_skinCam, SLOT(updatePowerLevels(int,int,int)));
#endif

    connect(m_ui->advancedWidget, SIGNAL(temperatureSetpointChanged(Camera::TemperatureSetpoint)),
            m_skinCam, SLOT(updateTemperatureSetpoint(Camera::TemperatureSetpoint)));

}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::loadSettingsFromDisk(QString filename)
{
    if (filename.isEmpty())
        filename = SettingsFileName;

    auto settings = Settings::getInstance();
    settings->loadFromDisk(filename);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::saveSettingsToDisk(QString filename)
{
    if (filename.isEmpty())
        filename = SettingsFileName;

    auto settings = Settings::getInstance();
    settings->saveToDisk(filename);
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::applySettings()
{
    const auto& settings = Settings::getConstInstance();

    m_skinCam->setReprocessBlocked(true);

    m_ui->colorAdjustmentWidget->applySettings(settings.getColorAdjustmentGroup());
    m_ui->sharpeningWidget->applySettings(settings.getSharpeningGroup());
    m_ui->smoothingWidget->applySettings(settings.getSmoothingGroup());
    m_ui->averageWidget->applySettings(settings.getAverageGroup());

    m_ui->barrelWidget->applySettings(settings.getBarrelGroup());
    m_ui->transformationWidget->applySettings(settings.getTransformationGroup());
    m_ui->cropWidget->applySettings(settings.getCropGroup());

    const auto& imageOutput = settings.getImageOutputGroup();
    m_ui->enableMirroring->setChecked(imageOutput.getMirrored());

    m_ui->whiteCalibrationWidget->applySettings(settings.getWhiteCalibrationGroup());

    m_ui->classifierWidget->applySettings(settings.getClassifierGroup());
    m_ui->detectionThresholdsWidget->applySettings(settings.getDetectionThresholdsGroup());
    m_ui->regionOfInterestWidget->applySettings(settings.getRegionOfInterestGroup());
    m_ui->faceDetectionWidget->applySettings(settings.getFaceDetectionGroup());

    const auto& regionOfInterest = settings.getRegionOfInterestGroup();

    std::vector<QPoint> vertices = {
        regionOfInterest.getVertex0(),
        regionOfInterest.getVertex1(),
        regionOfInterest.getVertex2(),
        regionOfInterest.getVertex3()
    };

    m_skinMaskSelector->setVertices(vertices);

    const auto& skinClassification = settings.getSkinClassificationGroup();

    Vec3f mean(static_cast<float>(skinClassification.getMean0()),
               static_cast<float>(skinClassification.getMean1()),
               static_cast<float>(skinClassification.getMean2()));

    Vec3f variance(static_cast<float>(skinClassification.getVariance0()),
                   static_cast<float>(skinClassification.getVariance1()),
                   static_cast<float>(skinClassification.getVariance2()));

    m_skinCam->updateClassifierParameters(mean, variance);

    m_ui->acquisitionWidget->applySettings(settings.getAcquisitionGroup());

#ifndef SKINCAM_BUILD_LEGACY
    m_ui->ledPowerWidget->applySettings(settings.getLedPowerGroup());
#endif

    m_ui->advancedWidget->applySettings(settings.getAdvancedGroup());

    m_skinCam->setReprocessBlocked(false);
    m_skinCam->reprocessIfPaused();
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::storeSettings()
{
    auto settings = Settings::getInstance();

    m_ui->colorAdjustmentWidget->storeSettings(settings->getColorAdjustmentGroup());
    m_ui->smoothingWidget->storeSettings(settings->getSmoothingGroup());
    m_ui->sharpeningWidget->storeSettings(settings->getSharpeningGroup());
    m_ui->averageWidget->storeSettings(settings->getAverageGroup());

    m_ui->barrelWidget->storeSettings(settings->getBarrelGroup());
    m_ui->transformationWidget->storeSettings(settings->getTransformationGroup());
    m_ui->cropWidget->storeSettings(settings->getCropGroup());

    auto imageOutput = settings->getImageOutputGroup();
    imageOutput->setMirrored(m_ui->enableMirroring->isChecked());

    m_ui->whiteCalibrationWidget->storeSettings(settings->getWhiteCalibrationGroup());

    m_ui->classifierWidget->storeSettings(settings->getClassifierGroup());
    m_ui->detectionThresholdsWidget->storeSettings(settings->getDetectionThresholdsGroup());
    m_ui->regionOfInterestWidget->storeSettings(settings->getRegionOfInterestGroup());
    m_ui->faceDetectionWidget->storeSettings(settings->getFaceDetectionGroup());

    auto regionOfInterest = settings->getRegionOfInterestGroup();

    const std::vector<QPoint> vertices = m_skinMaskSelector->getVertices();
    regionOfInterest->setVertex0(vertices.at(0));
    regionOfInterest->setVertex1(vertices.at(1));
    regionOfInterest->setVertex2(vertices.at(2));
    regionOfInterest->setVertex3(vertices.at(3));

    m_ui->acquisitionWidget->storeSettings(settings->getAcquisitionGroup());

#ifndef SKINCAM_BUILD_LEGACY
    m_ui->ledPowerWidget->storeSettings(settings->getLedPowerGroup());
#endif

    m_ui->advancedWidget->storeSettings(settings->getAdvancedGroup());
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::restoreWindow()
{
    QSettings settings;
    restoreGeometry(settings.value("WindowGeometry").toByteArray());
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::saveWindow()
{
    QSettings settings;
    settings.setValue("WindowGeometry", saveGeometry());
}

// ---------------------------------------------------------------------------------------------- //

void MainWindow::updateUserInterface()
{
    const bool providerOpened = m_cameraOpened || m_imageOpened;

    m_ui->actionOpenCamera->setEnabled(!providerOpened);
    m_ui->actionOpenImage->setEnabled(!providerOpened);
    m_ui->actionSaveImage->setEnabled(m_cameraOpened); // No point saving identical image
    m_ui->actionCloseImage->setEnabled(providerOpened);
    m_ui->actionPause->setEnabled(providerOpened);
    m_ui->actionPause->setChecked(false);

    m_ui->actionSetup->setEnabled(!providerOpened);
    m_ui->actionSaveSettings->setEnabled(providerOpened);
    m_ui->actionLoadSettings->setEnabled(providerOpened);

    m_ui->actionExportPixels->setEnabled(providerOpened);

    m_ui->imageTabWidget->setEnabled(providerOpened);
    m_ui->imageTabWidget->setCurrentIndex(0);

    m_ui->controlTabWidget->setEnabled(providerOpened);

    m_ui->regionOfInterestWidget->setEditChecked(false);

    const int index = m_ui->controlTabWidget->indexOf(m_ui->cameraTab);

    if (m_testMode)
    {
        m_ui->controlTabWidget->setTabEnabled(index, true);
        m_fpsWidget->setVisible(providerOpened);
    }
    else
    {
        m_ui->controlTabWidget->setTabEnabled(index, !m_imageOpened);
        m_fpsWidget->setVisible(m_cameraOpened);
    }

    m_ui->statusGroup->setVisible(!m_imageOpened);
    m_ui->advancedGroup->setVisible(m_testMode);
}

// ---------------------------------------------------------------------------------------------- //
