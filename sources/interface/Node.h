//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     https://github.com/linuxmao-org/Frontieres
//
// Copyright (C) 2020  Olivier Flatres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QtWidgets>

#include <stdio.h>
#include <iostream>

struct pointLimit {
    bool exist = false;
    float value;
};

struct NodeLimits {
    pointLimit minX;
    pointLimit minY;
    pointLimit maxX;
    pointLimit maxY;
};

class Node : public QGraphicsItem
{
public:
    Node();

    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    NodeLimits getLimits ();
    void setLimits(NodeLimits limitsToSet);
    void moveToPos (float newX, float newY);
    bool moved();
    void endMove();
    void setActiveState(bool l_activeSate);
    bool getActiveSate();
    void setWidthNodes(int l_widthNodes);
    int getWidthNodes();
    float coordNodeX = 0;
    float coordNodeY = 0;

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

private:

    QColor colorActive = Qt::yellow;
    QColor colorDarkActive = Qt::darkYellow;
    QColor colorInActive = Qt::red;
    QColor colorDarkInActive = Qt::darkRed;
    QColor currentColor = Qt::yellow;
    QColor currentDarkColor = Qt::darkYellow;

    NodeLimits limits;
    int widthNodes = 15;
    bool flagMoved = false;
    bool activeState = true;

    float limitPlus_x, limitPlus_y;
};


#endif // NODE_H

