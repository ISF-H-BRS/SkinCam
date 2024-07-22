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
#include "polygongraphicsitem.h"

#include <QCursor>
#include <QMenu>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>

// ---------------------------------------------------------------------------------------------- //

using namespace SkinCam;

// ---------------------------------------------------------------------------------------------- //

namespace {
    constexpr QPointF NodeExtent(6, 6);

    constexpr size_t MinimumVertexCount = 3;

    constexpr const char* DuplicateNodeText = "Duplicate node";
    constexpr const char* DeleteNodeText = "Delete node";
    constexpr const char* DeletePolygonText = "Delete polygon";
}

// ---------------------------------------------------------------------------------------------- //

PolygonGraphicsNode::PolygonGraphicsNode(QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsEllipseItem(QRectF(-NodeExtent, NodeExtent), parent)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setCacheMode(DeviceCoordinateCache);
    setAcceptHoverEvents(true);
    setZValue(1);
    setPen(QColor(Qt::darkGreen));
    setBrush(QColor(Qt::green));
    setCursor(Qt::ArrowCursor);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonGraphicsNode::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsEllipseItem::hoverEnterEvent(event);

    setBrush(QColor(Qt::darkGreen));
    update();
}

// ---------------------------------------------------------------------------------------------- //

void PolygonGraphicsNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsEllipseItem::hoverLeaveEvent(event);

    setBrush(QColor(Qt::green));
    update();
}

// ---------------------------------------------------------------------------------------------- //

void PolygonGraphicsNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsEllipseItem::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
        setCursor(Qt::ClosedHandCursor);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonGraphicsNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsEllipseItem::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton)
        setCursor(Qt::ArrowCursor);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonGraphicsNode::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QGraphicsEllipseItem::contextMenuEvent(event);
    event->accept();

    emit contextMenuRequested(this, event->screenPos());
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonGraphicsNode::itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant
{
    if (change == ItemPositionChange && scene())
    {
        QPointF newPos = value.toPointF();
        QRectF rect = scene()->sceneRect();

        if (!rect.contains(newPos))
        {
            newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
        }

        return newPos;
    }

    if (change == ItemPositionHasChanged)
        emit positionChanged(this, pos());

    return QGraphicsItem::itemChange(change, value);
}

// ---------------------------------------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------- //

PolygonGraphicsItem::PolygonGraphicsItem(const QPolygonF& polygon, QGraphicsItem* parent)
    : QGraphicsPolygonItem(polygon, parent)
{
    setPen(QColor(Qt::darkGreen));

    for (int i = 0; i < polygon.size(); ++i)
    {
        auto node = makeNode(polygon[i]);
        m_nodes.push_back(node);
    }
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonGraphicsItem::nodes() const -> const std::vector<PolygonGraphicsNode*>&
{
    return m_nodes;
}

// ---------------------------------------------------------------------------------------------- //

void PolygonGraphicsItem::setEditMode(EditMode mode)
{
    m_editMode = mode;

    for (auto node : m_nodes)
        node->setVisible(mode != EditMode::None);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonGraphicsItem::onContextMenuRequested(PolygonGraphicsNode* node, const QPoint& screenPos)
{
    if (m_editMode != EditMode::Full)
        return;

#if 0
    // This works fine on Linux but not on Windows using msys2,
    // we'll have to resort to an ugly workaround for now

    const auto duplicateNodeSlot = [&]{ duplicateNode(node); };
    const auto deleteNodeSlot = [&]{ deleteNode(node); };
    const auto deletePolygonSlot = [&]{ emit deleteRequested(this); };

    auto duplicateNode = new QAction("Duplicate node");
    connect(duplicateNode, &QAction::triggered, duplicateNodeSlot);

    if (m_nodes.size() > MinimumVertexCount)
    {
        auto deleteNode = new QAction("Delete node");
        deleteNode->setEnabled(m_nodes.size() > MinimumNodeCount);
        connect(deleteNode, &QAction::triggered, deleteNodeSlot);
    }

    auto deletePolygon = new QAction("Delete polygon");
    connect(deletePolygon, &QAction::triggered, deletePolygonSlot);
#endif

    QMenu menu;

    auto duplicateNode = new QAction(DuplicateNodeText);
    duplicateNode->setData(QVariant::fromValue(node));
    menu.addAction(duplicateNode);

    if (m_nodes.size() > MinimumVertexCount)
    {
        auto deleteNode = new QAction(DeleteNodeText);
        deleteNode->setData(QVariant::fromValue(node));
        menu.addAction(deleteNode);
    }

    auto deletePolygon = new QAction(DeletePolygonText);
    menu.addAction(deletePolygon);

    connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(handleContextMenuAction(QAction*)));
    menu.exec(screenPos);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonGraphicsItem::onPositionChanged(PolygonGraphicsNode* node, const QPointF& position)
{
    const int index = getIndex(node);

    QPolygonF polygon = this->polygon();
    ASSERT(index < polygon.size());

    polygon[index] = position;
    setPolygon(polygon);

    emit nodePositionChanged(position.toPoint());
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonGraphicsItem::makeNode(const QPointF& position) -> PolygonGraphicsNode*
{
    auto node = new PolygonGraphicsNode(this);
    node->setPos(position);

    connect(node, SIGNAL(positionChanged(PolygonGraphicsNode*,QPointF)),
            this, SLOT(onPositionChanged(PolygonGraphicsNode*,QPointF)));

    connect(node, SIGNAL(contextMenuRequested(PolygonGraphicsNode*,QPoint)),
            this, SLOT(onContextMenuRequested(PolygonGraphicsNode*,QPoint)));

    return node;
}

// ---------------------------------------------------------------------------------------------- //

auto PolygonGraphicsItem::getIndex(PolygonGraphicsNode* node) const -> int
{
    const auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
    ASSERT(it != m_nodes.end());

    const auto index = static_cast<int>(std::distance(m_nodes.begin(), it));
    ASSERT(index < static_cast<int>(m_nodes.size()));

    return index;
}

// ---------------------------------------------------------------------------------------------- //

void PolygonGraphicsItem::duplicateNode(PolygonGraphicsNode* node)
{
    const int index = getIndex(node) + 1; // Insert after

    QPolygonF polygon = this->polygon();
    ASSERT(index <= polygon.size());

    polygon.insert(index, polygon[index - 1]);
    polygon[index] += NodeExtent;

    setPolygon(polygon);

    auto duplicate = makeNode(polygon[index]);
    m_nodes.insert(m_nodes.begin() + index, duplicate);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonGraphicsItem::deleteNode(PolygonGraphicsNode* node)
{
    ASSERT(m_nodes.size() > MinimumVertexCount);

    const int index = getIndex(node);

    QPolygonF polygon = this->polygon();
    ASSERT(index < polygon.size());

    polygon.removeAt(index);
    setPolygon(polygon);

    delete node;
    m_nodes.erase(m_nodes.begin() + index);
}

// ---------------------------------------------------------------------------------------------- //

void PolygonGraphicsItem::handleContextMenuAction(QAction* action)
{
    if (action->text() == DuplicateNodeText)
        duplicateNode(action->data().value<PolygonGraphicsNode*>());
    else if (action->text() == DeleteNodeText)
        deleteNode(action->data().value<PolygonGraphicsNode*>());
    else if (action->text() == DeletePolygonText)
        emit deleteRequested(this);
}

// ---------------------------------------------------------------------------------------------- //
