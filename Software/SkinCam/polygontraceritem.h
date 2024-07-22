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

#include "namespace.h"

#include <QGraphicsEllipseItem>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>

#include <vector>

SKINCAM_BEGIN_NAMESPACE();

class PolygonTracerItem : public QGraphicsItemGroup
{
public:
    static constexpr qreal SnapRadius = 8.0F;

public:
    PolygonTracerItem(const QPointF& startPosition, QGraphicsItem* parent = nullptr);

    void setCursorPosition(const QPointF& position);

    auto positionWithinSnapRadius(const QPointF& position) const -> bool;

    void addVertex(const QPointF& position);
    void removeLastVertex();

    auto vertexCount() const -> size_t;

    auto polygonCompletable() const -> bool;
    auto polygonComplete() const -> bool;
    auto polygonValid() const -> bool;

    auto getPolygon() const -> QPolygonF;

private:
    auto getDistance(const QPointF& p1, const QPointF& p2) const -> qreal;
    void updateLines();

private:
    std::vector<QGraphicsLineItem*> m_lines;

    QGraphicsLineItem* m_closingLine;
    QGraphicsEllipseItem* m_closingVertex;

    QPointF m_startPosition;
    QPointF m_cursorPosition;

    bool m_polygonComplete = false;
};

SKINCAM_END_NAMESPACE();
