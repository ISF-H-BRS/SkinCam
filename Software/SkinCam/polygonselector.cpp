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
#include "polygonselector.h"

#include <QGraphicsPathItem>
#include <QResizeEvent>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr qreal SnapRadius = 8.0F;
    constexpr QPointF SnapExtent(SnapRadius, SnapRadius);
}

// ---------------------------------------------------------------------------------------------- //

class PolygonSelector::GraphicsView : public QGraphicsView
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

PolygonSelector::PolygonSelector(int width, int height, ImageWidget* parent)
    : ImageOverlay(parent),
      m_pixmap(width, height)
{
    setCursor(Qt::CrossCursor);

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
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::setBackgroundOpacity(uchar alpha)
{
    m_backgroundOpacity = alpha;
    update();
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::addPolygon(const Polygon& polygon)
{
    QPolygonF qpolygon;

    for (const Vec2i& vertex : polygon.vertices())
        qpolygon << QPointF(vertex.x(), vertex.y());

    addPolygon(qpolygon);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::addPolygon(const QPolygonF& polygon)
{
    auto item = new PolygonGraphicsItem(polygon);

    connect(item, SIGNAL(deleteRequested(PolygonGraphicsItem*)),
            this, SLOT(deletePolygon(PolygonGraphicsItem*)));

    m_scene->addItem(item);
    m_polygons.push_back(item);

    update();
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonSelector::getPolygons() const -> std::vector<Polygon>
{
    std::vector<Polygon> polygons;
    std::vector<Polygon::Vertex> vertices;

    for (QGraphicsPolygonItem* item : m_polygons)
    {
        const QPolygonF polygon = item->polygon();

        for (const QPointF& vertex : polygon)
            vertices.emplace_back(vertex.x(), vertex.y());

        polygons.emplace_back(vertices);
        vertices.clear();
    }

    return polygons;
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonSelector::polygonCount() const -> size_t
{
    return m_polygons.size();
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::setMirrored(bool mirror)
{
    m_view->resetTransform();

    if (mirror)
        m_view->scale(-1.0, 1.0);

    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::reset()
{
    deleteTracer();
    clearPolygons();

    emit polygonRemoved();
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::deletePolygon(PolygonGraphicsItem* item)
{
    const auto it = std::find(m_polygons.begin(), m_polygons.end(), item);
    ASSERT(it != m_polygons.end());

    delete item;
    m_polygons.erase(it);

    emit polygonRemoved();
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::showEvent(QShowEvent* event)
{
    ImageOverlay::showEvent(event);
    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::resizeEvent(QResizeEvent* event)
{
    ImageOverlay::resizeEvent(event);
    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::paintEvent(QPaintEvent* event)
{
    ImageOverlay::paintEvent(event);

    m_pixmap.fill(QColor(0, 0, 0, m_backgroundOpacity));

    QPainter pixmapPainter(&m_pixmap);
    pixmapPainter.setRenderHint(QPainter::Antialiasing);
    pixmapPainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    pixmapPainter.setBrush(QColor(0, 0, 0, 0));

    for (PolygonGraphicsItem* item : m_polygons)
        pixmapPainter.drawPolygon(item->polygon());

    if (m_tracer && m_tracer->polygonValid())
        pixmapPainter.drawPolygon(m_tracer->getPolygon());

    pixmapPainter.end();

    QSize pixmapSize = m_pixmap.size();
    pixmapSize.scale(rect().size(), Qt::KeepAspectRatio);

    QRect pixmapRect(QPoint(), pixmapSize);
    pixmapRect.moveCenter(rect().center());

    QPainter painter(this);
    painter.drawPixmap(pixmapRect, m_pixmap);

    emit paintFinished(this);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::mouseMoveEvent(QMouseEvent* event)
{
    ImageOverlay::mouseMoveEvent(event);

    if (m_tracer)
        m_tracer->setCursorPosition(getPositionInScene(event->pos()));
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::mousePressEvent(QMouseEvent* event)
{
    ImageOverlay::mousePressEvent(event);

    if (event->button() != Qt::LeftButton)
    {
        if (m_tracer)
        {
            if (m_tracer->vertexCount() > 1)
                m_tracer->removeLastVertex();
            else
                deleteTracer();
        }

        return;
    }

    const QPointF position = getPositionInScene(event->pos());

    if (m_tracer)
    {
        m_tracer->addVertex(position);

        if (m_tracer->polygonComplete())
        {
            addPolygon(m_tracer->getPolygon());
            deleteTracer();

            emit polygonAdded();
        }
    }
    else
        makeTracer(position);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::mouseDoubleClickEvent(QMouseEvent* event)
{
    ImageOverlay::mouseDoubleClickEvent(event);
    event->accept(); // Prevent toggling fullscreen
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::keyPressEvent(QKeyEvent* event)
{
    ImageOverlay::keyPressEvent(event);

    if (event->key() == Qt::Key_Escape)
        deleteTracer();
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonSelector::getPositionInScene(const QPoint& pos) -> QPointF
{
    const QRectF rect = m_view->sceneRect();
    QPointF position = m_view->mapToScene(pos);

    position.setX(std::clamp(position.x(), rect.left(), rect.right()));
    position.setY(std::clamp(position.y(), rect.top(), rect.bottom()));

    return position;
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::setPolygonsEditable(bool editable)
{
    const auto mode = editable ? PolygonGraphicsItem::EditMode::Full
                               : PolygonGraphicsItem::EditMode::None;

    for (PolygonGraphicsItem* polygon : m_polygons)
        polygon->setEditMode(mode);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::clearPolygons()
{
    for (PolygonGraphicsItem* polygon : m_polygons)
        delete polygon;

    m_polygons.clear();
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::makeTracer(const QPointF& startPosition)
{
    m_tracer = std::make_unique<PolygonTracerItem>(startPosition);
    m_scene->addItem(m_tracer.get());

    setPolygonsEditable(false);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonSelector::deleteTracer()
{
    m_tracer = nullptr;
    setPolygonsEditable(true);
}

// ---------------------------------------------------------------------------------------------- //
