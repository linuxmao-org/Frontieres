#include "Node.h"

Node::Node()
{
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton);
}

QRectF Node::boundingRect() const
{
    return QRectF(-15.5, -15.5, 34, 34);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRadialGradient gradient(- widthNodes / 3, - widthNodes / 3, widthNodes);

    gradient.setColorAt(0, Qt::yellow);
    gradient.setColorAt(1, Qt::darkYellow);
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::gray);
    painter->drawEllipse(- widthNodes / 2 + 3, - widthNodes / 2 + 2, widthNodes, widthNodes);
    painter->setBrush(gradient);
    painter->setPen(QPen(Qt::black, 0));
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
