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
    float coordNodeX = 0;
    float coordNodeY = 0;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *) Q_DECL_OVERRIDE;

private:

    QColor color;
    NodeLimits limits;
    int widthNodes = 15;
    int heightMaxAff = 180;
    int coeffAffMS = 50;
    int xSustain = 40;
    bool flagMoved = false;

    float limitPlus_x, limitPlus_y;
};


#endif // NODE_H

