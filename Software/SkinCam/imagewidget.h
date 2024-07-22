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

#include "imageoverlay.h"

#include <QTimer>
#include <QWidget>

SKINCAM_BEGIN_NAMESPACE();

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    enum class Renderer
    {
        OpenGL,
        Software
    };

public:
    ImageWidget(Renderer renderer, QWidget* parent = nullptr);

    void setImage(const QImage& image);
    void setText(const QString& text);

    auto overlay() const -> ImageOverlay*;

    auto isMirrored() const -> bool;

public slots:
    void setMirrored(bool mirror);
    void setCursorAutoHide(bool hide);
    void clear();

signals:
    void fullscreenChanged(bool fullscreen);

private:
    friend class ImageOverlay;
    void setOverlay(ImageOverlay* overlay);

private slots:
    void drawText(QWidget* target);

    void updateCursor();
    void hideCursor();

private:
    void resizeEvent(QResizeEvent* event) override;

    void contextMenuEvent(QContextMenuEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void setFullscreen(bool fullscreen);

private:
    class Backend;
    class OpenGLWidget;
    class SoftwareWidget;

    Backend* m_backend = nullptr;

    QImage m_image;
    QString m_text;

    ImageOverlay* m_overlay = nullptr;

    bool m_mirrored = false;

    QTimer m_cursorTimer;
    bool m_cursorAutoHide = true;
    bool m_cursorHidden = false;
};

SKINCAM_END_NAMESPACE();
