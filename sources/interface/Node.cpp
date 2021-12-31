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
#include "Node.h"

Node::Node()
{
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton);
}

QRectF Node::boundingRect() const
{
    return QRectF(- (widthNodes + 0.5), - (widthNodes + 0.5), widthNodes + 10, widthNodes + 10);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (activeState) {
        currentColor = colorActive;
        currentDarkColor = colorDarkActive;
    }
    else {
        currentColor = colorInActive;
        currentDarkColor = colorDarkInActive;
    }

    QRadialGradient gradient(- widthNodes / 3, - widthNodes / 3, widthNodes);

    gradient.setColorAt(0, currentColor);
    gradient.setColorAt(1, currentDarkColor);
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::black);
    painter->setOpacity(0.5f);
    painter->drawEllipse(- widthNodes / 2 + 3, - widthNodes / 2 + 2, widthNodes, widthNodes);
    painter->setBrush(gradient);
    painter->setPen(QPen(Qt::black, 0));
    painter->setOpacity(1.0f);
    painter->drawEllipse(- widthNodes / 2, - widthNodes / 2, widthNodes, widthNodes);
}

NodeLimits Node::getLimits()
{
    return limits;
}

void Node::setLimits(NodeLimits limitsToSet)
{
    limits = limitsToSet;
}

void Node::moveToPos(float newX, float newY)
{
    coordNodeX = newX;
    coordNodeY = newY;

    setPos(coordNodeX, coordNodeY);
}

bool Node::moved()
{
    return flagMoved;
}

void Node::endMove()
{
    flagMoved = false;
}

void Node::setActiveState(bool l_activeSate)
{
    activeState = l_activeSate;
    if (activeState) {
        currentColor = colorActive;
        currentDarkColor = colorDarkActive;
    }
    else {
        currentColor = colorInActive;
        currentDarkColor = colorDarkInActive;
    }
    this->hide();
    this->show();
}

bool Node::getActiveSate()
{
    return activeState;
}

void Node::setWidthNodes(int l_widthNodes)
{
    widthNodes = l_widthNodes;
}

int Node::getWidthNodes()
{
    return widthNodes;
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    setCursor(Qt::ClosedHandCursor);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    coordNodeX = (coordNodeX + event->pos().x());
    if ((limits.maxX.exist && (coordNodeX >= limits.maxX.value)))
        coordNodeX = limits.maxX.value;
    if ((limits.minX.exist && (coordNodeX <= limits.minX.value)))
        coordNodeX = limits.minX.value;
    coordNodeY = coordNodeY + event->pos().y();
    if ((limits.maxY.exist && (coordNodeY >= limits.maxY.value)))
        coordNodeY = limits.maxY.value;
    if ((limits.minY.exist && (coordNodeY <= limits.minY.value)))
        coordNodeY = limits.minY.value;
    setPos(coordNodeX, coordNodeY);
    flagMoved = true;
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    setCursor(Qt::OpenHandCursor);
}
