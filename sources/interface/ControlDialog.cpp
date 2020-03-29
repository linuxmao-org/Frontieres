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
    QPen graypen (Qt::gray);
    blackPen.setWidth(1);

    QRectF zoneControl;
    zoneControl.setX(-ampControl);
    zoneControl.setY(-ampControl);
    zoneControl.setWidth(ampControl * 2);
    zoneControl.setHeight(ampControl * 2);

    QLine vertLine;
    QLine horzLine;

    QBrush *whiteBrush = new QBrush(Qt::white);

    controlGraphicsView->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));

    controlGraphicScene->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));
    controlGraphicScene->addRect(zoneControl, blackPen, *whiteBrush);

    vertLine.setLine(0,-(ampControl + 10),0,(ampControl + 10));
    horzLine.setLine(-(ampControl + 10),0,ampControl + 10,0);
    controlGraphicScene->addLine(vertLine,blackPen);
    controlGraphicScene->addLine(horzLine,blackPen);

    vertLine.setLine(-(ampControl/2),-(ampControl + 10),-(ampControl/2),(ampControl + 10));
    horzLine.setLine(-(ampControl + 10),-(ampControl/2),ampControl + 10,-(ampControl/2));
    controlGraphicScene->addLine(vertLine,graypen);
    controlGraphicScene->addLine(horzLine,graypen);

    vertLine.setLine((ampControl/2),-(ampControl + 10),(ampControl/2),(ampControl + 10));
    horzLine.setLine(-(ampControl + 10),(ampControl/2),ampControl + 10,(ampControl/2));
    controlGraphicScene->addLine(vertLine,graypen);
    controlGraphicScene->addLine(horzLine,graypen);

    setLimits(myNode, limits_control, true, - ampControl, true, ampControl, true, -ampControl, true, ampControl);
    controlGraphicScene->addItem(&myNode);
    myNode.setX(0);
    myNode.setY(0);

    scale = ui->doubleSpinBox_Scale->value();
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
    updateFromControlPosition(0, 0);
    linking = false;
}

void ControlDialog::updateFromControlPosition(float l_x, float l_y)
{
    bool c_limited = false;
    if ((myNode.getLimits().minX.exist) && (l_x < myNode.getLimits().minX.value))
        c_limited = true;
    if ((myNode.getLimits().maxX.exist) && (l_x > myNode.getLimits().maxX.value))
        c_limited = true;
    if ((myNode.getLimits().minY.exist) && (l_y < myNode.getLimits().minY.value))
        c_limited = true;
    if ((myNode.getLimits().maxY.exist) && (l_y > myNode.getLimits().maxY.value))
        c_limited = true;
    if (!c_limited) {
        myNode.moveToPos(l_x, l_y);
        if (orientation == VERTICAL) {
            cloudRef->setCtrlInterval(-(l_y * scale));
            cloudRef->setCtrlShade(float(l_x + ampControl) / ampControl);
        }
        else {
            cloudRef->setCtrlInterval(l_x * scale);
            cloudRef->setCtrlShade(2 - float(l_y + ampControl) / ampControl);
        }
        ui->doubleSpinBox_Interval->setValue(cloudRef->getCtrlInterval());
        ui->doubleSpinBox_Shade->setValue(cloudRef->getCtrlShade());
        updateMinMax();
    }
}

void ControlDialog::updateControlPosition()
{
    if (linking)
        return;

    int n_shade = ((ui->doubleSpinBox_Shade->value() - 1) * ampControl);
    int n_interval = - (ui->doubleSpinBox_Interval->value() / ui->doubleSpinBox_Scale->value());

    if (orientation == VERTICAL) {
       myNode.moveToPos(n_shade, n_interval);
    }
    else {
       myNode.moveToPos(-n_interval, n_shade);
    }
}

void ControlDialog::updateMinMax()
{
    float n_interval = ui->doubleSpinBox_Interval->value();
    float n_scale = ui->doubleSpinBox_Scale->value();

    float n_scaleMin = (n_interval / 240);
    ui->doubleSpinBox_Scale->setMinimum(abs(n_scaleMin));
    float n_intervalMax = (n_scale * 240);
    ui->doubleSpinBox_Interval->setMaximum(abs(n_intervalMax));
    ui->doubleSpinBox_Interval->setMinimum(-abs(n_intervalMax));
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

   if ((c_x > (myNode->x() - (myNode->getWidthNodes()/2 + 2))) && (c_x < (myNode->x() + (myNode->getWidthNodes()/2 + 2))) &&
       (c_y > (myNode->y() - (myNode->getWidthNodes()/2 + 2))) && (c_y < (myNode->y() + (myNode->getWidthNodes()/2 + 2)))) {
       draging = true;
   }
   else {
       myControl->updateFromControlPosition(c_x, c_y);
       draging = true;
   }
}

void MyQGraphicsView::mouseReleaseEvent(QMouseEvent *eventMouse)
{
    draging = false;
}

void MyQGraphicsView::mouseMoveEvent(QMouseEvent *eventMouse)
{
    if (draging) {
        int c_x = eventMouse->x() - (ampControl + (myNode->getWidthNodes() + 3));
        int c_y = eventMouse->y() - (ampControl + (myNode->getWidthNodes() + 3));
        myControl->updateFromControlPosition(c_x, c_y);
    }

}

void ControlDialog::on_doubleSpinBox_Scale_valueChanged(double arg1)
{
    scale = arg1;
}

void ControlDialog::on_radioButton_orientation_vertical_toggled(bool checked)
{
    if (checked) {
        orientation =  VERTICAL;
        updateControlPosition();
    }
}

void ControlDialog::on_radioButton_orientation_horizontal_toggled(bool checked)
{
    if (checked) {
        orientation =  HORIZONTAL;
        updateControlPosition();
    }
}

void ControlDialog::on_doubleSpinBox_Shade_editingFinished()
{
    updateControlPosition();
}

void ControlDialog::on_doubleSpinBox_Interval_editingFinished()
{
    updateControlPosition();
    updateMinMax();
}

void ControlDialog::on_doubleSpinBox_Scale_editingFinished()
{
    scale = ui->doubleSpinBox_Scale->value();
    updateControlPosition();
    updateMinMax();
}
