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

#include "imagewidget.h"
#include "polygon.h"
#include "polygongraphicsitem.h"
#include "polygontraceritem.h"

#include <QGraphicsView>

SKINCAM_BEGIN_NAMESPACE();

class PolygonSelector : public ImageOverlay
{
    Q_OBJECT

public:
    static constexpr uchar DefaultBackgroundOpacity = 100;

public:
    PolygonSelector(int width, int height, ImageWidget* parent = nullptr);

    void setBackgroundOpacity(uchar alpha);

    void addPolygon(const Polygon& polygon);
    void addPolygon(const QPolygonF& polygon);

    auto getPolygons() const -> std::vector<Polygon>;
    auto polygonCount() const -> size_t;

public slots:
    void setMirrored(bool mirror);
    void reset();

signals:
    void polygonAdded();
    void polygonRemoved();

private slots:
    void deletePolygon(PolygonGraphicsItem* item);

private:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

    auto getPositionInScene(const QPoint& pos) -> QPointF;

    void setPolygonsEditable(bool editable);

    void clearPolygons();

    void makeTracer(const QPointF& startPosition);
    void deleteTracer();

private:
    class GraphicsView;
    GraphicsView* m_view = nullptr;

    QGraphicsScene* m_scene = nullptr;

    std::unique_ptr<PolygonTracerItem> m_tracer;
    std::vector<PolygonGraphicsItem*> m_polygons;

    QPixmap m_pixmap;

    uchar m_backgroundOpacity = DefaultBackgroundOpacity;
};

SKINCAM_END_NAMESPACE();
