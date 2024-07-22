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

#include "backend.h"

#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLWidget>

SKINCAM_BEGIN_NAMESPACE();

class ImageWidget::OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions, public Backend
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget* parent);
    ~OpenGLWidget() override;

    void setImage(const QImage& image) override;
    void setMirrored(bool mirror) override;

signals:
    void paintFinished(QWidget* widget);

private:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

    void updateTexture();
    void deleteTexture();
    void drawTexture();

    void updateDisplayList(const QSize& viewport);

    static void updateTexture(QOpenGLTexture* texture, const QImage& image);
    static auto createTexture(const QImage& image) -> std::unique_ptr<QOpenGLTexture>;

private:
    QImage m_image;

    std::unique_ptr<QOpenGLTexture> m_texture;
    QImage::Format m_textureFormat = QImage::Format_Invalid;
    bool m_needTextureUpdate = false;

    GLuint m_displayList = 0;
    bool m_needDisplayListUpdate = false;

    bool m_mirrored = false;
    bool m_initialized = false;
};

SKINCAM_END_NAMESPACE();
