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
#include "regionselector.h"

#include <QGraphicsPathItem>
#include <QResizeEvent>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

class RegionSelector::GraphicsView : public QGraphicsView
{
public:
    explicit GraphicsView(QWidget* parent)
        : QGraphicsView(parent)
    {
        setMouseTracking(true);
    }

private:
    void mouseMoveEvent(QMouseEvent* event) override
    {
        QGraphicsView::mouseMoveEvent(event);
        event->ignore();
    }
};

// ---------------------------------------------------------------------------------------------- //

RegionSelector::RegionSelector(int width, int height, ImageWidget* parent)
    : ImageOverlay(parent)
{
    setVisible(false);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    m_view = new GraphicsView(this);
    layout->addWidget(m_view);

    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setStyleSheet("background: transparent");
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHint(QPainter::Antialiasing);

    m_scene = new QGraphicsScene(m_view);
    m_scene->setSceneRect(0, 0, width, height);

    m_view->setScene(m_scene);
    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);

    m_polygon = makePolygonItem(makeDefaultPolygon());
}

// ---------------------------------------------------------------------------------------------- //

void RegionSelector::setBackgroundOpacity(uchar alpha)
{
    m_backgroundOpacity = alpha;
    update();
}

// ---------------------------------------------------------------------------------------------- //

void RegionSelector::setVertices(const std::vector<QPoint>& vertices)
{
    RETURN_IF_NOT(vertices.size() == VertexCount);

    const QPolygon polygon(QVector<QPoint>(vertices.begin(), vertices.end()));
    recreatePolygonItem(polygon);
}

// ---------------------------------------------------------------------------------------------- //

auto RegionSelector::getVertices() const -> std::vector<QPoint>
{
    const QPolygonF polygon = m_polygon->polygon();

    std::vector<QPoint> vertices;

    for (const QPointF& vertex : polygon)
        vertices.push_back(vertex.toPoint());

    return vertices;
}

// ---------------------------------------------------------------------------------------------- //

void RegionSelector::setPolygon(const Polygon& polygon)
{
    std::vector<QPoint> vertices;

    for (const auto& vertex : polygon.vertices())
        vertices.emplace_back(vertex.x(), vertex.y());

    setVertices(vertices);
}

// ---------------------------------------------------------------------------------------------- //

auto RegionSelector::getPolygon() const -> Polygon
{
    const QPolygon polygon = m_polygon->polygon().toPolygon();

    std::vector<Polygon::Vertex> vertices;

    for (const QPoint& vertex : polygon)
        vertices.emplace_back(vertex.x(), vertex.y());

    return Polygon(vertices);
}

// ---------------------------------------------------------------------------------------------- //

void RegionSelector::reset()
{
    recreatePolygonItem(makeDefaultPolygon());
}

// ---------------------------------------------------------------------------------------------- //

void RegionSelector::setEditEnabled(bool enable)
{
    m_polygon->setVisible(enable);
    m_editEnabled = enable;
}

// ---------------------------------------------------------------------------------------------- //

void RegionSelector::setMirrored(bool mirror)
{
    m_view->resetTransform();

    if (mirror)
        m_view->scale(-1.0, 1.0);

    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

// ---------------------------------------------------------------------------------------------- //

void RegionSelector::showEvent(QShowEvent* event)
{
    ImageOverlay::showEvent(event);
    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);

    emit selectionChanged();
}

// ---------------------------------------------------------------------------------------------- //

void RegionSelector::hideEvent(QHideEvent* event)
{
    ImageOverlay::hideEvent(event);
    emit selectionChanged();
}

// ---------------------------------------------------------------------------------------------- //

void RegionSelector::resizeEvent(QResizeEvent* event)
{
    ImageOverlay::resizeEvent(event);
    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

// ---------------------------------------------------------------------------------------------- //

void RegionSelector::paintEvent(QPaintEvent* event)
{
    ImageOverlay::paintEvent(event);

    QPainterPath path;

    const QRectF rect(m_scene->sceneRect());
    path.addRect(-5, -5, rect.width() + 10, rect.height() + 10);

    const QPolygonF polygon = m_polygon->polygon();
    path.addPolygon(polygon);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, m_backgroundOpacity));
    painter.drawPath(m_view->mapFromScene(path));

    if (!m_editEnabled)
    {
        QPen pen(Qt::gray);
        pen.setWidth(2);

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPolygon(m_view->mapFromScene(polygon));
    }

    painter.end();

    emit paintFinished(this);
}

// ---------------------------------------------------------------------------------------------- //

auto RegionSelector::makeDefaultPolygon() const -> QPolygonF
{
    QRectF rect(m_scene->sceneRect());

    const qreal x = 0.5 * rect.width();
    const qreal y = 0.5 * rect.height();
    const qreal dx = 0.375 * rect.width();
    const qreal dy = 0.375 * rect.height();

    QPolygonF polygon;

    // Top left
    polygon << QPointF(x - dx, y - dy);

    // Top right
    polygon << QPointF(x + dx, y - dy);

    // Bottom right
    polygon << QPointF(x + dx, y + dy);

    // Bottom left
    polygon << QPointF(x - dx, y + dy);

    return polygon;
}

// ---------------------------------------------------------------------------------------------- //

auto RegionSelector::makePolygonItem(const QPolygonF& polygon) -> PolygonGraphicsItem*
{
    auto item = new PolygonGraphicsItem(polygon);
    item->setEditMode(PolygonGraphicsItem::EditMode::Positions);
    item->setVisible(m_editEnabled);

    connect(item, SIGNAL(nodePositionChanged(QPoint)), this, SIGNAL(nodePositionChanged(QPoint)));
    connect(item, SIGNAL(nodePositionChanged(QPoint)), this, SIGNAL(selectionChanged()));

    m_scene->addItem(item);
    return item;
}

// ---------------------------------------------------------------------------------------------- //

void RegionSelector::recreatePolygonItem(const QPolygonF& polygon)
{
    delete m_polygon;
    m_polygon = makePolygonItem(polygon);

    emit selectionChanged();
}

// ---------------------------------------------------------------------------------------------- //
