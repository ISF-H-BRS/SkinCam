####################################################################################################
#                                                                                                  #
#   This file is part of the ISF SkinCam project.                                                  #
#                                                                                                  #
#   Author:                                                                                        #
#   Marcel Hasler <mahasler@gmail.com>                                                             #
#                                                                                                  #
#   Copyright (c) 2020 - 2024                                                                      #
#   Bonn-Rhein-Sieg University of Applied Sciences                                                 #
#                                                                                                  #
#   This program is free software: you can redistribute it and/or modify it under the terms        #
#   of the GNU General Public License as published by the Free Software Foundation, either         #
#   version 3 of the License, or (at your option) any later version.                               #
#                                                                                                  #
#   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      #
#   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      #
#   See the GNU General Public License for more details.                                           #
#                                                                                                  #
#   You should have received a copy of the GNU General Public License along with this program.     #
#   If not, see <https:# www.gnu.org/licenses/>.                                                   #
#                                                                                                  #
####################################################################################################

cmake_minimum_required(VERSION 3.15)
project(SkinCam)

set(SKINCAM_VERSION_MAJOR 5)
set(SKINCAM_VERSION_MINOR 0)
set(SKINCAM_VERSION_PATCH 0)

set(SKINCAM_VERSION ${SKINCAM_VERSION_MAJOR}.${SKINCAM_VERSION_MINOR}.${SKINCAM_VERSION_PATCH} )

set(SKINCAM_BUILD_LEGACY OFF CACHE BOOL "Build additional target for legacy hardware")
set(SKINCAM_USE_DUMMY_CAMERA OFF CACHE BOOL "Use dummy camera backend for testing purposes")

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_INSTALL_RPATH $ORIGIN)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(libzip REQUIRED)
find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt6 COMPONENTS Network OpenGL OpenGLWidgets SerialPort Widgets REQUIRED)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libusb REQUIRED libusb-1.0)

if (NOT DEFINED ENV{VIMBA_HOME})
    message(FATAL_ERROR "Environment variable VIMBA_HOME is not defined. "
                        "Please make sure it points to latest available version.")
endif()

set(VIMBA_HOME $ENV{VIMBA_HOME})
set(VIMBA_LIBRARY VimbaCPP)

if (NOT DEFINED ENV{LIBIFD_HOME})
    message(FATAL_ERROR "Environment variable LIBIFD_HOME is not defined. "
                        "Please make sure it points to latest available version.")
endif()

set(LIBIFD_HOME $ENV{LIBIFD_HOME})

set(EXTRA_LIBRARIES
    freeimageplus
    freeimage
    IFD
    usb-1.0
    zip
)

if (WIN32)
    set(OpenCL_LIBRARY C:/Windows/System32/OpenCL.dll)
    set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} ws2_32)
endif()

if (UNIX)
    link_directories(${VIMBA_HOME}/VimbaCPP/DynamicLib/x86_64bit)
elseif(WIN32)
    link_directories(${VIMBA_HOME}/mingw)
endif()

add_compile_definitions(SKINCAM_VERSION="${SKINCAM_VERSION}")

if (SKINCAM_USE_DUMMY_CAMERA)
    add_compile_definitions(SKINCAM_USE_DUMMY_CAMERA)
endif()

# Suppress warnings from OpenCL and Vimba headers
add_compile_options(
    -Wno-ignored-qualifiers
    -Wno-unused-parameter
    -Wno-deprecated-enum-enum-conversion
)

if (WIN32)
    # Workaround for buggy Vimba headers
    add_compile_options(-mnop-fun-dllimport)
endif()

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${VIMBA_HOME} ${LIBIFD_HOME}/include)
link_directories(${CMAKE_BINARY_DIR})

set(SKINCAM_SOURCES
    CoSeSt2/CoSeSt2.cpp
    aboutdialog.cpp
    assertions.cpp
    camera.cpp
    cameraexception.cpp
    cameraselectdialog.cpp
    camerasystem.cpp
    controller.cpp
    dummycamera.cpp
    elapsedtimer.cpp
    exception.cpp
    facedetector.cpp
    facedetectoroverlay.cpp
    facedetectorwrapper.cpp
    filter.cpp
    filters/averagefilter.cpp
    filters/barrelfilter.cpp
    filters/coloradjustfilter.cpp
    filters/cropfilter.cpp
    filters/filterworker.cpp
    filters/sharpeningfilter.cpp
    filters/smoothingfilter.cpp
    filters/transformationfilter.cpp
    filters/whitecalibrationfilter.cpp
    fpscounter.cpp
    fpswidget.cpp
    frame.cpp
    image.cpp
    imagearchive.cpp
    imageoverlay.cpp
    imageprocessor.cpp
    imageprocessor/convertimages.cpp
    imageprocessor/convertresults.cpp
    imageprocessor/runfilters.cpp
    imageprocessor/runskinclassifier.cpp
    imageprocessor/worker.cpp
    imagereader.cpp
    imagewidget.cpp
    imagewidget/openglwidget.cpp
    imagewidget/softwarewidget.cpp
    kernels/abstractkernel.cpp
    logger.cpp
    logwindow.cpp
    main.cpp
    mainwindow/acquisitionwidget.cpp
    mainwindow/advancedwidget.cpp
    mainwindow/averagewidget.cpp
    mainwindow/barrelwidget.cpp
    mainwindow/classifierwidget.cpp
    mainwindow/coloradjustmentwidget.cpp
    mainwindow/cropwidget.cpp
    mainwindow/detectionthresholdswidget.cpp
    mainwindow/facedetectionwidget.cpp
    mainwindow/ledpowerwidget.cpp
    mainwindow/regionofinterestwidget.cpp
    mainwindow/sharpeningwidget.cpp
    mainwindow/smoothingwidget.cpp
    mainwindow.cpp
    mainwindow/statuswidget.cpp
    mainwindow/transformationwidget.cpp
    mainwindow/whitecalibrationwidget.cpp
    numberlabel.cpp
    openclexception.cpp
    openclwrapper.cpp
    pixelexportdialog.cpp
    pixelexporter.cpp
    polygon.cpp
    polygongraphicsitem.cpp
    polygonselector.cpp
    polygontraceritem.cpp
    rectaverager.cpp
    regionselector.cpp
    selectionprocessor.cpp
    selectionwindow.cpp
    serialport.cpp
    setupdialog.cpp
    skincamsystem.cpp
    skinclassifier.cpp
    skinclassifier/boundingboxbackend.cpp
    skinclassifier/countskinpixels.cpp
    skinclassifier/gaussianbackend.cpp
    skinclassifier/gaussianthresholdbackend.cpp
    skinclassifier/skinclassifierworker.cpp
    skinclassifier/summatepixels.cpp
    tcpserver.cpp
    vimbaloader.cpp
)

set(SKINCAM_HEADERS
    CoSeSt2/CoSeSt2.h
    aboutdialog.h
    abstractbuffer.h
    assertions.h
    camera.h
    cameraexception.h
    cameraselectdialog.h
    camerasystem.h
    clheader.h
    computebuffer.h
    controller.h
    dummycamera.h
    elapsedtimer.h
    endianess.h
    exception.h
    facedetector.h
    facedetectoroverlay.h
    facedetectorwrapper.h
    filter.h
    filters.h
    filters/averagefilter.h
    filters/barrelfilter.h
    filters/coloradjustfilter.h
    filters/cropfilter.h
    filters/filterworker.h
    filters/sharpeningfilter.h
    filters/smoothingfilter.h
    filters/transformationfilter.h
    filters/whitecalibrationfilter.h
    fpscounter.h
    fpswidget.h
    frame.h
    gaussian.h
    global.h
    image.h
    imagearchive.h
    imageoverlay.h
    imageprocessor.h
    imageprocessor/convertimages.h
    imageprocessor/convertresults.h
    imageprocessor/runfilters.h
    imageprocessor/runskinclassifier.h
    imageprocessor/worker.h
    imageprovider.h
    imagereader.h
    imagewidget.h
    imagewidget/backend.h
    imagewidget/openglwidget.h
    imagewidget/softwarewidget.h
    kernels.h
    kernels/abstractkernel.h
    kernels/averagekernel.h
    kernels/barrelgenkernel.h
    kernels/blurkernel.h
    kernels/convertkernel.h
    kernels/cropgenkernel.h
    kernels/flipkernel.h
    kernels/gaussiankernel.h
    kernels/maskkernel.h
    kernels/mergekernel.h
    kernels/normdiffkernel.h
    kernels/partialmaxkernel.h
    kernels/partialsumkernel.h
    kernels/remapkernel.h
    kernels/roigenkernel.h
    kernels/scalekernel.h
    kernels/sharpenkernel.h
    kernels/splitkernel.h
    kernels/subtractkernel.h
    kernels/thresholdkernel.h
    kernels/transformgenkernel.h
    kernels/widenkernel.h
    logger.h
    logwindow.h
    mainwindow/acquisitionwidget.h
    mainwindow/advancedwidget.h
    mainwindow/averagewidget.h
    mainwindow/barrelwidget.h
    mainwindow/classifierwidget.h
    mainwindow/coloradjustmentwidget.h
    mainwindow/cropwidget.h
    mainwindow/detectionthresholdswidget.h
    mainwindow/facedetectionwidget.h
    mainwindow/ledpowerwidget.h
    mainwindow/regionofinterestwidget.h
    mainwindow/sharpeningwidget.h
    mainwindow/smoothingwidget.h
    mainwindow.h
    mainwindow/statuswidget.h
    mainwindow/transformationwidget.h
    mainwindow/whitecalibrationwidget.h
    matrix3.h
    memorybuffer.h
    memorypool.h
    namespace.h
    numberlabel.h
    objectpool.h
    openclexception.h
    openclwrapper.h
    pixelexport.h
    pixelexportdialog.h
    pixelexporter.h
    polygon.h
    polygongraphicsitem.h
    polygonselector.h
    polygontraceritem.h
    rect.h
    rectaverager.h
    regionselector.h
    selectionprocessor.h
    selectionwindow.h
    serialport.h
    settings.h
    setupdialog.h
    skincamsystem.h
    skinclassifier.h
    skinclassifier/boundingboxbackend.h
    skinclassifier/classifierbackend.h
    skinclassifier/countskinpixels.h
    skinclassifier/gaussianbackend.h
    skinclassifier/gaussianthresholdbackend.h
    skinclassifier/skinclassifierworker.h
    skinclassifier/summatepixels.h
    tcpserver.h
    types.h
    vectors.h
    vimbaloader.h
)

set(SKINCAM_FORMS
    aboutdialog.ui
    cameraselectdialog.ui
    fpswidget.ui
    logwindow.ui
    mainwindow.ui
    mainwindow/acquisitionwidget.ui
    mainwindow/advancedwidget.ui
    mainwindow/averagewidget.ui
    mainwindow/barrelwidget.ui
    mainwindow/classifierwidget.ui
    mainwindow/coloradjustmentwidget.ui
    mainwindow/cropwidget.ui
    mainwindow/detectionthresholdswidget.ui
    mainwindow/facedetectionwidget.ui
    mainwindow/ledpowerwidget.ui
    mainwindow/regionofinterestwidget.ui
    mainwindow/sharpeningwidget.ui
    mainwindow/smoothingwidget.ui
    mainwindow/statuswidget.ui
    mainwindow/transformationwidget.ui
    mainwindow/whitecalibrationwidget.ui
    pixelexportdialog.ui
    selectionwindow.ui
    setupdialog.ui
)

set(SKINCAM_RESOURCES SkinCam.qrc)

set(SKINCAM_LIBRARIES
    ${EXTRA_LIBRARIES}
    ${OpenCL_LIBRARY}
    OpenGL::GL
    Qt::Network
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::SerialPort
    Qt::Widgets
    Threads::Threads
    ${VIMBA_LIBRARY}
)

add_custom_target(ProgramHeaders
    COMMAND python3 cl2h.py ./opencl ${CMAKE_BINARY_DIR}/program_map.h program_map
    COMMENT "Generating header files for OpenCL programs ..."
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

add_executable(SkinCam WIN32
    ${SKINCAM_SOURCES}
    ${SKINCAM_HEADERS}
    ${SKINCAM_FORMS}
    ${SKINCAM_RESOURCES}
)

add_dependencies(SkinCam ProgramHeaders)
target_link_libraries(SkinCam ${SKINCAM_LIBRARIES})

if (SKINCAM_BUILD_LEGACY)
    message("Building additional target for legacy hardware.")

    add_executable(SkinCamLegacy WIN32
        ${SKINCAM_SOURCES}
        ${SKINCAM_HEADERS}
        ${SKINCAM_FORMS}
        ${SKINCAM_RESOURCES}
    )

    add_dependencies(SkinCamLegacy ProgramHeaders)
    target_compile_definitions(SkinCamLegacy PRIVATE SKINCAM_BUILD_LEGACY)
    target_link_libraries(SkinCamLegacy ${SKINCAM_LIBRARIES})
endif()
