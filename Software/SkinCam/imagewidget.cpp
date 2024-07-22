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

#include "imageoverlay.h"
#include "imagewidget.h"

#include "imagewidget/openglwidget.h"
#include "imagewidget/softwarewidget.h"

#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QScreen>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

ImageWidget::ImageWidget(Renderer renderer, QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    const auto setupBackend = [&](auto backend)
    {
        connect(backend, SIGNAL(paintFinished(QWidget*)), this, SLOT(drawText(QWidget*)));
        layout->addWidget(backend);
        m_backend = backend;
    };

    if (renderer == Renderer::OpenGL)
        setupBackend(new OpenGLWidget(this));
    else
        setupBackend(new SoftwareWidget(this));

    m_cursorTimer.setInterval(1000);
    m_cursorTimer.setSingleShot(true);
    connect(&m_cursorTimer, SIGNAL(timeout()), this, SLOT(hideCursor()));
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::setImage(const QImage& image)
{
    m_image = image;
    m_backend->setImage(image);

    update();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::setText(const QString& text)
{
    m_text = text;
    update();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::setOverlay(ImageOverlay* overlay)
{
    if (m_overlay)
        delete m_overlay;

    m_overlay = overlay;

    if (m_overlay)
        connect(m_overlay, SIGNAL(paintFinished(QWidget*)), this, SLOT(drawText(QWidget*)));
}

// ---------------------------------------------------------------------------------------------- //

auto ImageWidget::overlay() const -> ImageOverlay*
{
    return m_overlay;
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::setMirrored(bool mirror)
{
    m_mirrored = mirror;
    m_backend->setMirrored(mirror);

    update();
}

// ---------------------------------------------------------------------------------------------- //

auto ImageWidget::isMirrored() const -> bool
{
    return m_mirrored;
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::setCursorAutoHide(bool hide)
{
    m_cursorAutoHide = hide;
    updateCursor();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::clear()
{
    setFullscreen(false);
    setImage(QImage());
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::drawText(QWidget* target)
{
    if (m_text.isEmpty())
        return;

    if (m_overlay && m_overlay->isVisible() && target != m_overlay)
        return;

    QFont font = QApplication::font();
    font.setWeight(QFont::Black);

    if (isFullScreen())
        font.setPointSize(font.pointSize() + 2);
    else
        font.setPointSize(font.pointSize() + 1);

    QPainterPath path;
    path.addText(10, height() - 10, font, m_text);

    QPainter painter(target);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::white);
    painter.drawPath(path);
    painter.end();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::updateCursor()
{
    if (m_cursorHidden)
    {
        setCursor(Qt::ArrowCursor);
        m_cursorHidden = false;
    }

    if (m_cursorAutoHide && isFullScreen())
        m_cursorTimer.start();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::hideCursor()
{
    setCursor(Qt::BlankCursor);
    m_cursorHidden = true;
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (m_overlay)
        m_overlay->resize(size());
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if (isFullScreen() || m_image.isNull())
        return;

    const auto saveToFile = [this]
    {
        auto name = "SkinCam-" + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss") + ".png";
        name = QFileDialog::getSaveFileName(this, "Save Image", name, "Image Files (*.png)");

        if (!name.isEmpty())
            m_image.save(name);
    };

    QMenu menu(this);

    auto saveImage = new QAction(QIcon(":/res/save.svg"), "Save image");
    connect(saveImage, &QAction::triggered, saveToFile);

    menu.addAction(saveImage);
    menu.exec(event->globalPos());
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
    updateCursor();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        setFullscreen(!isFullScreen());
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::keyPressEvent(QKeyEvent* event)
{
    const int key = event->key();
    const bool leaveFullscreen = (key == Qt::Key_F || key == Qt::Key_Escape);

    if (isFullScreen() && leaveFullscreen)
        setFullscreen(false);
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::setFullscreen(bool fullscreen)
{
    if (fullscreen)
    {
        // Show fullscreen window on the screen that contains most of the widget
        move(screen()->geometry().topLeft());

        setWindowFlags(Qt::Window);
        showFullScreen();

        if (m_cursorAutoHide)
        {
            setMouseTracking(true);
            m_cursorTimer.start();
        }
    }
    else
    {
        setWindowFlags(Qt::Widget);
        showNormal();

        setMouseTracking(false);
        setCursor(Qt::ArrowCursor);
        m_cursorTimer.stop();
        m_cursorHidden = false;
    }

    emit fullscreenChanged(fullscreen);
}

// ---------------------------------------------------------------------------------------------- //
