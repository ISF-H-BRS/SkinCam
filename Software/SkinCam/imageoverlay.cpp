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
#include "imagewidget.h"
#include "imageoverlay.h"

#include <QMouseEvent>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

ImageOverlay::ImageOverlay(ImageWidget* imageWidget)
    : QWidget(imageWidget),
      m_imageWidget(imageWidget)
{
    ASSERT_NOT_NULL(imageWidget);

    m_imageWidget->setOverlay(this);
    setMouseTracking(true);
}

// ---------------------------------------------------------------------------------------------- //

auto ImageOverlay::imageWidget() const -> ImageWidget*
{
    return m_imageWidget;
}

// ---------------------------------------------------------------------------------------------- //

void ImageOverlay::mouseMoveEvent(QMouseEvent* event)
{
    event->ignore();
}

// ---------------------------------------------------------------------------------------------- //
