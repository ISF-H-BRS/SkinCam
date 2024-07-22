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

#include "global.h"
#include "selectionwindow.h"

#include "ui_selectionwindow.h"

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

SelectionWindow::SelectionWindow(QWidget* parent)
    : QWidget(parent),
      m_ui(std::make_unique<Ui::SelectionWindow>())
{
    m_ui->setupUi(this);

    m_imageWidget = new ImageWidget(ImageWidget::Renderer::Software, m_ui->image);
    m_imageWidget->setMinimumSize(ImageWidth, ImageHeight);
    m_imageWidget->setContextMenuPolicy(Qt::NoContextMenu);
    m_ui->image->layout()->addWidget(m_imageWidget);

    m_polygonSelector = new PolygonSelector(ImageWidth, ImageHeight, m_imageWidget);

    connect(m_polygonSelector, SIGNAL(polygonAdded()), this, SLOT(updateButtons()));
    connect(m_polygonSelector, SIGNAL(polygonRemoved()), this, SLOT(updateButtons()));

    connect(m_ui->acceptButton, SIGNAL(clicked()), this, SLOT(processSelection()));
    connect(m_ui->clearButton, SIGNAL(clicked()), m_polygonSelector, SLOT(reset()));
}

// ---------------------------------------------------------------------------------------------- //

SelectionWindow::~SelectionWindow() = default;

// ---------------------------------------------------------------------------------------------- //

void SelectionWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit visiblilityChanged(true);

    updatePixmap();
}

// ---------------------------------------------------------------------------------------------- //

void SelectionWindow::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    emit visiblilityChanged(false);
}

// ---------------------------------------------------------------------------------------------- //

void SelectionWindow::setAcceptButtonText(const QString& text)
{
    m_ui->acceptButton->setText(text);
}

// ---------------------------------------------------------------------------------------------- //

void SelectionWindow::setImage(const QImage& image)
{
    m_image = image;

    if (isVisible())
        updatePixmap();
}

// ---------------------------------------------------------------------------------------------- //

void SelectionWindow::updateButtons()
{
    const size_t count = m_polygonSelector->polygonCount();

    m_ui->acceptButton->setEnabled(count > 0);
    m_ui->clearButton->setEnabled(count > 0);
}

// ---------------------------------------------------------------------------------------------- //

void SelectionWindow::processSelection()
{
    const std::vector<Polygon> polygons = m_polygonSelector->getPolygons();
    emit selectionAccepted(polygons);
}

// ---------------------------------------------------------------------------------------------- //

void SelectionWindow::updatePixmap()
{
    m_imageWidget->setImage(m_image);
}

// ---------------------------------------------------------------------------------------------- //
