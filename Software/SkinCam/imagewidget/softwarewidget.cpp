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

#include "softwarewidget.h"

#include <QKeyEvent>
#include <QPainter>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

ImageWidget::SoftwareWidget::SoftwareWidget(QWidget* parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::SoftwareWidget::setImage(const QImage& image)
{
    m_image = image;
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::SoftwareWidget::setMirrored(bool mirror)
{
    m_mirrored = mirror;
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::SoftwareWidget::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    const QRect& widgetRect = rect();

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    painter.drawRect(widgetRect);

    if (!m_image.isNull())
    {
        QPixmap pixmap = QPixmap::fromImage(m_image);

        QSize pixmapSize = pixmap.size();
        pixmapSize.scale(widgetRect.size(), Qt::KeepAspectRatio);

        QRect pixmapRect(QPoint(), pixmapSize);
        pixmapRect.moveCenter(widgetRect.center());

        if (m_mirrored)
        {
            painter.translate(widgetRect.width(), 0.0);
            painter.scale(-1.0, 1.0);
        }

        painter.drawPixmap(pixmapRect, pixmap);
    }

    painter.end();

    emit paintFinished(this);
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::SoftwareWidget::mouseMoveEvent(QMouseEvent* event)
{
    event->ignore();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::SoftwareWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    event->ignore();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::SoftwareWidget::keyPressEvent(QKeyEvent* event)
{
    event->ignore();
}

// ---------------------------------------------------------------------------------------------- //

