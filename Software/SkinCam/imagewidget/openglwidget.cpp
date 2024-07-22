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

#include "openglwidget.h"
#include "../global.h"

#include <QKeyEvent>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

ImageWidget::OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(8); // Anti-aliased text
    setFormat(format);

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

// ---------------------------------------------------------------------------------------------- //

ImageWidget::OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();

    m_texture = nullptr;

    if (m_displayList > 0)
        glDeleteLists(m_displayList, 1);

    doneCurrent();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::setImage(const QImage& image)
{
    m_image = image;

    if (image.isNull())
    {
        makeCurrent();
        deleteTexture();
        doneCurrent();
    }
    else
    {
        m_needTextureUpdate = true;
        update();
    }
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::setMirrored(bool mirror)
{
    m_mirrored = mirror;
    m_needDisplayListUpdate = true;
    update();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    event->ignore();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    event->ignore();
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
    event->ignore();
}

// ---------------------------------------------------------------------------------------------- //

// Also called when changing fullscreen mode
void ImageWidget::OpenGLWidget::initializeGL()
{
    if (m_texture)
        deleteTexture();

    if (m_displayList > 0)
    {
        glDeleteLists(m_displayList, 1);
        m_displayList = 0;
        m_needDisplayListUpdate = true;
    }

    initializeOpenGLFunctions();

    if (!m_image.isNull())
        m_needTextureUpdate = true;

    m_initialized = true;
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::resizeGL(int width, int height)
{
    Q_UNUSED(width)
    Q_UNUSED(height)

    m_needDisplayListUpdate = true;
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::paintGL()
{
    Q_ASSERT(m_initialized);

    const QSize viewport = size() * devicePixelRatioF();
    glViewport(0, 0, viewport.width(), viewport.height());

    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_needDisplayListUpdate)
        updateDisplayList(viewport);

    if (m_needTextureUpdate)
        updateTexture();

    if (m_texture)
        drawTexture();

    emit paintFinished(this);
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::updateTexture()
{
    Q_ASSERT(!m_image.isNull());

    if (m_texture && m_image.format() == m_textureFormat)
        updateTexture(m_texture.get(), m_image);
    else
    {
        m_texture = createTexture(m_image);
        m_textureFormat = m_image.format();
    }

    m_needTextureUpdate = false;
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::deleteTexture()
{
    m_texture = nullptr;
    m_textureFormat = QImage::Format_Invalid;
    m_needTextureUpdate = false;
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::drawTexture()
{
    Q_ASSERT(m_texture != nullptr && m_displayList > 0);

    glEnable(GL_TEXTURE_2D);
    m_texture->bind();

    glCallList(m_displayList);
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::updateDisplayList(const QSize& viewport)
{
    const double imageRatio = static_cast<double>(ImageWidth) / ImageHeight;
    const double viewportRatio = static_cast<double>(viewport.width()) / viewport.height();

    const double scale = viewportRatio / imageRatio;

    double hextent = 1.0;
    double vextent = 1.0;

    if (scale < 1.0)
        vextent /= scale;
    else
        hextent *= scale;

    if (m_mirrored)
        hextent *= -1.0;

    if (m_displayList == 0)
        m_displayList = glGenLists(1);

    glNewList(m_displayList, GL_COMPILE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-hextent, hextent, -vextent, vextent, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBegin(GL_QUADS);
      glTexCoord2i( 0, 1);
      glVertex2i  (-1,-1);

      glTexCoord2i( 1, 1);
      glVertex2i  ( 1,-1);

      glTexCoord2i( 1, 0);
      glVertex2i  ( 1, 1);

      glTexCoord2i( 0, 0);
      glVertex2i  (-1, 1);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEndList();

    m_needDisplayListUpdate = false;
}

// ---------------------------------------------------------------------------------------------- //

void ImageWidget::OpenGLWidget::updateTexture(QOpenGLTexture* texture, const QImage& image)
{
    QImage::Format format = image.format();

    if (format == QImage::Format_RGB888)
        texture->setData(QOpenGLTexture::RGB, QOpenGLTexture::UInt8, image.bits());
    else if (format == QImage::Format_RGBA8888)
        texture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, image.bits());
    else if (format == QImage::Format_Grayscale8)
        texture->setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, image.bits());
}

// ---------------------------------------------------------------------------------------------- //

auto ImageWidget::OpenGLWidget::createTexture(const QImage& image) -> std::unique_ptr<QOpenGLTexture>
{
    auto texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    texture->setSize(image.width(), image.height());
    texture->setMipLevels(1);
    texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
    texture->setWrapMode(QOpenGLTexture::ClampToBorder);
    texture->setBorderColor(Qt::black);
    texture->allocateStorage();

    updateTexture(texture.get(), image);
    return texture;
}

// ---------------------------------------------------------------------------------------------- //
