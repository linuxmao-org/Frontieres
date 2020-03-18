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

