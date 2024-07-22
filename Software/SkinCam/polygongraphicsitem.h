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

#include <QAction>
#include <QGraphicsPolygonItem>

// ---------------------------------------------------------------------------------------------- //

SKINCAM_BEGIN_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //

class PolygonGraphicsNode : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:
    PolygonGraphicsNode(QGraphicsItem* parent = nullptr);

signals:
    void positionChanged(PolygonGraphicsNode* node, const QPointF& position);
    void contextMenuRequested(PolygonGraphicsNode* node, const QPoint& screenPos);

private:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

    auto itemChange(GraphicsItemChange change, const QVariant& value) -> QVariant override;
};

// ---------------------------------------------------------------------------------------------- //

class PolygonGraphicsItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT

public:
    enum class EditMode
    {
        None,
        Positions,
        Full
    };

public:
    PolygonGraphicsItem(const QPolygonF& polygon, QGraphicsItem *parent = nullptr);

    auto nodes() const -> const std::vector<PolygonGraphicsNode*>&;

    void setEditMode(EditMode mode);

signals:
    void nodePositionChanged(const QPoint& position);
    void deleteRequested(PolygonGraphicsItem* item);

private slots:
    void onPositionChanged(PolygonGraphicsNode* node, const QPointF& position);
    void onContextMenuRequested(PolygonGraphicsNode* node, const QPoint& screenPos);

    void handleContextMenuAction(QAction* action);

private:
    auto makeNode(const QPointF& position) -> PolygonGraphicsNode*;

    auto getIndex(PolygonGraphicsNode* node) const -> int;

    void duplicateNode(PolygonGraphicsNode* node);
    void deleteNode(PolygonGraphicsNode* node);

private:
    std::vector<PolygonGraphicsNode*> m_nodes;
    EditMode m_editMode = EditMode::Full;
};

// ---------------------------------------------------------------------------------------------- //

SKINCAM_END_NAMESPACE();

// ---------------------------------------------------------------------------------------------- //
