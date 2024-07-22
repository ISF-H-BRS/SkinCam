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

#include <QGraphicsView>

SKINCAM_BEGIN_NAMESPACE();

class RegionSelector : public ImageOverlay
{
    Q_OBJECT

public:
    static constexpr size_t VertexCount = 4;

public:
    RegionSelector(int width, int height, ImageWidget* parent = nullptr);

    void setBackgroundOpacity(uchar alpha);

    void setVertices(const std::vector<QPoint>& vertices);
    auto getVertices() const -> std::vector<QPoint>;

    void setPolygon(const Polygon& polygon);
    auto getPolygon() const -> Polygon;

public slots:
    void reset();

    void setEditEnabled(bool enable);
    void setMirrored(bool mirror);

signals:
    void nodePositionChanged(const QPoint& position);
    void selectionChanged();

private:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    auto makeDefaultPolygon() const -> QPolygonF;
    auto makePolygonItem(const QPolygonF& polygon) -> PolygonGraphicsItem*;

    void recreatePolygonItem(const QPolygonF& polygon);

private:
    class GraphicsView;
    GraphicsView* m_view = nullptr;

    QGraphicsScene* m_scene = nullptr;
    PolygonGraphicsItem* m_polygon = nullptr;

    bool m_editEnabled = false;
    uchar m_backgroundOpacity = 150;
};

SKINCAM_END_NAMESPACE();
