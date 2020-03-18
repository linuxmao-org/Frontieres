#include "ControlDialog.h"
#include "ui_ControlDialog.h"
#include <iostream>
#include <stdio.h>

ControlDialog::ControlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ControlDialog)
{
    ui->setupUi(this);

    controlGraphicScene = new QGraphicsScene(this);
    controlGraphicsView = new MyQGraphicsView(ui->frameGraphicsView, this, controlGraphicScene, &myNode);

    setMouseTracking(true);

    QPen blackPen (Qt::black);
    blackPen.setWidth(1);

    QRectF zoneControl;
    zoneControl.setX(-ampControl);
    zoneControl.setY(-ampControl);
    zoneControl.setWidth(ampControl * 2);
    zoneControl.setHeight(ampControl * 2);

    QLine vertLine;
    QLine horzLine;
    vertLine.setLine(0,-(ampControl + 10),0,(ampControl + 10));
    horzLine.setLine(-(ampControl + 10),0,ampControl + 10,0);
    QBrush *whiteBrush = new QBrush(Qt::white);

    controlGraphicsView->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));

    controlGraphicScene->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));
    controlGraphicScene->addRect(zoneControl, blackPen, *whiteBrush);
    controlGraphicScene->addLine(vertLine,blackPen);
    controlGraphicScene->addLine(horzLine,blackPen);

    setLimits(myNode, limits_control, true, - ampControl, true, ampControl, true, -ampControl, true, ampControl);
    controlGraphicScene->addItem(&myNode);
    myNode.setX(0);
    myNode.setY(0);

    scale = ui->doubleSpinBox_Scale->value();

    //std::cout << "sortie creation controlDialog" << std::endl;
}

ControlDialog::~ControlDialog()
{
    delete ui;
}

void ControlDialog::linkCloud(Cloud *cloudLinked)
{
    linking = true;
    cloudRef = cloudLinked;
    controlGraphicsView->linkCloud(cloudLinked, this);
    if (!cloudRef->getActiveState())
        myNode.setActiveState(false);
    myNode.moveToPos(0,0);
    updateControlPosition();
    //cloudRef->changesDone(false);
    linking = false;
}

void ControlDialog::updateControlPosition()
{
    //cout << "update controlposition" << endl;
    ui->doubleSpinBox_Interval->setValue(cloudRef->getCtrlInterval());
    ui->doubleSpinBox_Shade->setValue(cloudRef->getCtrlShade());
}

void ControlDialog::setLimits(Node &nodeToLimit, NodeLimits &nodeLimitsToGive,
                              bool t_minX, float valueMinX,
                              bool t_maxX, float valueMaxX,
                              bool t_minY, float valueMinY,
                              bool t_maxY, float valueMaxY)
{
        nodeLimitsToGive.minX.exist = t_minX;
        nodeLimitsToGive.maxX.exist = t_maxX;
        nodeLimitsToGive.minY.exist = t_minY;
        nodeLimitsToGive.maxY.exist = t_maxY;

        nodeLimitsToGive.minX.value = valueMinX;
        nodeLimitsToGive.maxX.value = valueMaxX;
        nodeLimitsToGive.minY.value = valueMinY;
        nodeLimitsToGive.maxY.value = valueMaxY;

        nodeToLimit.setLimits(nodeLimitsToGive);
}

void ControlDialog::setActiveState(bool l_activeState)
{
    activeState = l_activeState;
}

bool ControlDialog::getActiveState()
{
    return activeState;
}

float ControlDialog::getScale()
{
    return scale;
}

void ControlDialog::setOrientation(int l_orientation)
{
    orientation = l_orientation;
}

int ControlDialog::getOrientation()
{
    return orientation;
}

void ControlDialog::on_dial_Interval_valueChanged(int value)
{
    ui->doubleSpinBox_Interval->setValue(value);
}

void ControlDialog::on_doubleSpinBox_Interval_valueChanged(double arg1)
{
    ui->dial_Interval->setValue(int (arg1));
}

void ControlDialog::on_dial_Shade_valueChanged(int value)
{
    //ui->doubleSpinBox_Shade->setValue(value);
}

void ControlDialog::on_doubleSpinBox_Shade_valueChanged(double arg1)
{
    ui->dial_Shade->setValue(int (arg1));
    if (linking)
        return;
    int n_pos = (arg1 - 1) * (ampControl);
    if (orientation == VERTICAL) {
       //cout << "arg=" << arg1 << ", nodex=" << myNode.x() << ", np=" << n_pos << endl;
       myNode.moveToPos(n_pos, myNode.y());
    }
    else {
       myNode.moveToPos(myNode.x(), 2 - n_pos);
    }
}

MyQGraphicsView::MyQGraphicsView(QWidget *parent, ControlDialog *l_controlDialog, QGraphicsScene *l_graphicsScene, Node *l_Node)
{
    this->setParent(parent);
    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum );
    this->setMaximumSize(parent->maximumSize());
    this->setMinimumSize(parent->minimumSize());
    this->setFrameShape(Shape::NoFrame);
    this->setScene(l_graphicsScene);
    myNode = l_Node;
}

void MyQGraphicsView::linkCloud(Cloud *cloudLinked, ControlDialog *l_controldialog)
{
        cloudRef = cloudLinked;
        myControl = l_controldialog;
}

void MyQGraphicsView::mousePressEvent(QMouseEvent *eventMouse)
{
    //std::cout << "button=" << eventMouse->button() << std::endl;
    if (eventMouse->button() == 2) {
        myNode->setActiveState(false);
        cloudRef->setActiveState(false);
    }
    else {
        myNode->setActiveState(true);
        if (!cloudRef->getActiveState())
            cloudRef->setActiveState(true);
    }
    int c_x = eventMouse->x() - (ampControl + (myNode->getWidthNodes() + 3));
    int c_y = eventMouse->y() - (ampControl + (myNode->getWidthNodes() + 3));
    /*std::cout << "c_x=" << c_x << ",c_y=" << c_y << std::endl;
    std::cout << "nodex=" << myNode->x() << ",nodey=" << myNode->y() << std::endl;*/
    if ((c_x > (myNode->x() - (myNode->getWidthNodes()/2 + 2))) && (c_x < (myNode->x() + (myNode->getWidthNodes()/2 + 2))) &&
        (c_y > (myNode->y() - (myNode->getWidthNodes()/2 + 2))) && (c_y < (myNode->y() + (myNode->getWidthNodes()/2 + 2)))) {
        //std::cout << "gagné !" << std::endl;
        draging = true;
    }
    else {
        bool c_limited = false;
        if ((myNode->getLimits().minX.exist) && (c_x < myNode->getLimits().minX.value))
            c_limited = true;
        if ((myNode->getLimits().maxX.exist) && (c_x > myNode->getLimits().maxX.value))
            c_limited = true;
        if ((myNode->getLimits().minY.exist) && (c_y < myNode->getLimits().minY.value))
            c_limited = true;
        if ((myNode->getLimits().maxY.exist) && (c_y > myNode->getLimits().maxY.value))
            c_limited = true;
        if (!c_limited) {
            myNode->moveToPos(c_x, c_y);
            if (myControl->getOrientation() == VERTICAL) {
                cloudRef->setCtrlInterval(-(c_y * myControl->getScale()));
                cloudRef->setCtrlShade(float(c_x + ampControl) / ampControl);
            }
            else {
                cloudRef->setCtrlInterval(c_x * myControl->getScale());
                cloudRef->setCtrlShade(2 - float(c_y + ampControl) / ampControl);
            }
            myControl->updateControlPosition();
        }
        draging = true;
    }
}

void MyQGraphicsView::mouseReleaseEvent(QMouseEvent *eventMouse)
{
    draging = false;
}

void MyQGraphicsView::mouseMoveEvent(QMouseEvent *eventMouse)
{
    //std::cout << "mmove" << std::endl;
    if (draging) {
        //std::cout << "draging" << std::endl;
        QGraphicsSceneMouseEvent c_qgmevent;
        QPoint c_qpoint;
//        c_qpoint.setX(eventMouse->x());
  //      c_qpoint.setY(eventMouse->y());
        //_qgmevent.setPos(c_qpoint);
        int c_x = eventMouse->x() - (ampControl + (myNode->getWidthNodes() + 3));
        int c_y = eventMouse->y() - (ampControl + (myNode->getWidthNodes() + 3));
        //std::cout << "c_x=" << c_x << std::endl;;
        //std::cout << "c_y=" << c_y << std::endl;
        bool c_limited = false;
        if ((myNode->getLimits().minX.exist) && (c_x < myNode->getLimits().minX.value))
            c_limited = true;
        if ((myNode->getLimits().maxX.exist) && (c_x > myNode->getLimits().maxX.value))
            c_limited = true;
        if ((myNode->getLimits().minY.exist) && (c_y < myNode->getLimits().minY.value))
            c_limited = true;
        if ((myNode->getLimits().maxY.exist) && (c_y > myNode->getLimits().maxY.value))
            c_limited = true;
        if (!c_limited) {
            myNode->moveToPos(c_x, c_y);
            if (myControl->getOrientation() == VERTICAL) {
                cloudRef->setCtrlInterval(-(c_y * myControl->getScale()));
                cloudRef->setCtrlShade(float(c_x + ampControl) / ampControl);
            }
            else {
                cloudRef->setCtrlInterval(c_x * myControl->getScale());
                cloudRef->setCtrlShade(2 - float(c_y + ampControl) / ampControl);
            }
            myControl->updateControlPosition();
        }
    }

}

void ControlDialog::on_doubleSpinBox_Scale_valueChanged(double arg1)
{
    scale = arg1;
    ui->dial_Scale->setValue(int (arg1));

}

void ControlDialog::on_dial_Scale_valueChanged(int value)
{
    ui->doubleSpinBox_Scale->setValue(value);
}

void ControlDialog::on_radioButton_orientation_vertical_toggled(bool checked)
{
    if (checked)
        orientation =  VERTICAL;
}

void ControlDialog::on_radioButton_orientation_horizontal_toggled(bool checked)
{
    if (checked)
        orientation =  HORIZONTAL;
}


