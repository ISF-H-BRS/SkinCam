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
#include "polygontraceritem.h"

#include <QPen>

#include <cmath> // for sqrt()

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr qreal SnapRadius = PolygonTracerItem::SnapRadius;

    constexpr QPointF SnapExtent(SnapRadius, SnapRadius);
    constexpr QRectF SnapRect(-SnapExtent, SnapExtent);

    constexpr Qt::GlobalColor LineColor = Qt::green;
}

// ---------------------------------------------------------------------------------------------- //

PolygonTracerItem::PolygonTracerItem(const QPointF& startPosition, QGraphicsItem* parent)
    : QGraphicsItemGroup(parent),
      m_closingLine(new QGraphicsLineItem(this)),
      m_closingVertex(new QGraphicsEllipseItem(SnapRect, this)),
      m_startPosition(startPosition),
      m_cursorPosition(startPosition)
{
    QColor lineColor(LineColor);
    lineColor.setAlpha(127);

    QPen linePen(lineColor);
    linePen.setStyle(Qt::DotLine);

    m_closingLine->setPen(linePen);
    m_closingLine->setVisible(false);

    m_closingVertex->setPen(QColor(LineColor));
    m_closingVertex->setVisible(false);
    m_closingVertex->setPos(m_startPosition);

    addVertex(m_startPosition);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonTracerItem::setCursorPosition(const QPointF& position)
{
    if (m_polygonComplete)
        return;

    m_cursorPosition = position;
    m_closingVertex->setVisible(polygonCompletable() && positionWithinSnapRadius(position));

    updateLines();
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonTracerItem::positionWithinSnapRadius(const QPointF& position) const -> bool
{
    const qreal distance = getDistance(position, m_startPosition);
    return distance <= SnapRadius;
}

// ---------------------------------------------------------------------------------------------- //

void PolygonTracerItem::addVertex(const QPointF& position)
{
    if (m_polygonComplete)
        return;

    if (polygonCompletable() && positionWithinSnapRadius(position))
    {
        m_closingVertex->setVisible(false);
        m_polygonComplete = true;

        return;
    }

    const QLineF line(position, position);

    auto item = new QGraphicsLineItem(line, this);
    item->setPen(QColor(LineColor));

    m_lines.push_back(item);

    updateLines();
}

// ---------------------------------------------------------------------------------------------- //

void PolygonTracerItem::removeLastVertex()
{
    if (m_polygonComplete || m_lines.size() <= 1)
        return;

    delete m_lines.back();
    m_lines.pop_back();

    updateLines();

    if (!polygonCompletable())
        m_closingVertex->setVisible(false);
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonTracerItem::vertexCount() const -> size_t
{
    return m_lines.size();
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonTracerItem::polygonCompletable() const -> bool
{
    static constexpr size_t MinimumVertexCount = 3;
    return !m_polygonComplete && vertexCount() >= MinimumVertexCount;
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonTracerItem::polygonComplete() const -> bool
{
    return m_polygonComplete;
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonTracerItem::polygonValid() const -> bool
{
    static constexpr size_t MinimumVertexCount = 2; // Cursor position implicitly included
    return vertexCount() >= MinimumVertexCount;
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonTracerItem::getPolygon() const -> QPolygonF
{
    QPolygonF polygon;

    for (QGraphicsLineItem* item : m_lines)
        polygon << item->line().p1();

    if (!m_polygonComplete)
        polygon << m_cursorPosition;

    return polygon;
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonTracerItem::getDistance(const QPointF& p1, const QPointF& p2) const -> qreal
{
    const QPointF diff = p2 - p1;
    return std::sqrt(diff.x()*diff.x() + diff.y()*diff.y());
}

// ---------------------------------------------------------------------------------------------- //

void PolygonTracerItem::updateLines()
{
    ASSERT(!m_polygonComplete);

    m_closingLine->setVisible(!m_polygonComplete && m_lines.size() > 1);

    if (!m_lines.empty())
    {
        QLineF line = m_lines.back()->line();
        m_lines.back()->setLine(QLineF(line.p1(), m_cursorPosition));

        m_closingLine->setLine(QLineF(m_cursorPosition, m_startPosition));
    }
}

// ---------------------------------------------------------------------------------------------- //
