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

#include "mainwindow.h"
#include "openclwrapper.h"
#include "skincamsystem.h"

#ifdef SKINCAM_ENABLE_VIMBA
#include "vimbaloader.h"
#endif

#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

auto main(int argc, char* argv[]) -> int
{
    // Needed for queued connections
    qRegisterMetaType<Frame>("Frame");
    qRegisterMetaType<std::vector<Rect>>("std::vector<Rect>");

    QApplication::setOrganizationName("Bonn-Rhein-Sieg University of Applied Sciences");
    QApplication::setApplicationName("SkinCam Viewer");
    QApplication::setApplicationVersion(SKINCAM_VERSION);

    // Needed for toggling fullscreen mode without crashing
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QCommandLineParser parser;
    parser.setApplicationDescription("Frontend to the SkinCam camera system.");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption platformOption("platform-match",
                                      "Prefer OpenCL platforms whose names contain given string.",
                                      "filter");
    parser.addOption(platformOption);

    QCommandLineOption deviceOption("device-match",
                                    "Prefer OpenCL devices whose names contain given string.",
                                    "filter");
    parser.addOption(deviceOption);

    QCommandLineOption testmodeOption("testmode", "Enables some extra features for testing.");
    parser.addOption(testmodeOption);

    QApplication application(argc, argv);
    parser.process(application);

    Logger::info("SkinCam started.\n");

    try {
        const std::string platformFilter = parser.value(platformOption).toStdString();
        const std::string deviceFilter = parser.value(deviceOption).toStdString();

        OpenCLWrapper opencl(platformFilter, deviceFilter);

#ifdef SKINCAM_ENABLE_VIMBA
        VimbaLoader vimba;
#endif

        SkinCamSystem skinCam;

        MainWindow window(&skinCam, parser.isSet(testmodeOption));
        window.show();

        return QApplication::exec();
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(nullptr, "Error", e.what());
        return -1;
    }
}

// ---------------------------------------------------------------------------------------------- //
