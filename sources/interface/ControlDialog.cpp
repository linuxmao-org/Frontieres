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

    initScene();

    myNode.setX(0);
    myNode.setY(0);

    zoom = ui->doubleSpinBox_Zoom->value();

    QTimer *tmAutoUpdate = new QTimer(this);
    connect(tmAutoUpdate, &QTimer::timeout, this, &ControlDialog::autoUpdate);
    tmAutoUpdate->start(500);
}

ControlDialog::~ControlDialog()
{
    delete ui;
}

void ControlDialog::autoUpdate()
{
    autoUpdating = true;
    if (cloudRef) {
        if (cloudRef->getCtrlAutoUpdate()) {
            //cout << "control autoupdate" << endl;
            updateFromControlPosition(cloudRef->getCtrlX(), cloudRef->getCtrlY());
            if (scaleAttraction()) {
                double l_interval = getInterval();
                double l_nearestPosition = nearestScalePosition(l_interval, getMinInterval(), getMaxInterval());
                setInterval(l_nearestPosition);
            }
            cloudRef->setCtrlAutoUpdate(false);
        }
        if (cloudRef->getActualiseByPhrase()) {
            //out << "actualise by phrase" << endl;
            updateValues(cloudRef->getCtrlInterval(),cloudRef->getCtrlShade());
            if (scaleAttraction()) {
                double l_interval = getInterval();
                double l_nearestPosition = nearestScalePosition(l_interval, getMinInterval(), getMaxInterval());
                setInterval(l_nearestPosition);
            }
            cloudRef->setActualiseByPhrase(false);
        }

    }
    autoUpdating = false;
}

void ControlDialog::initScene()
{
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

    setLimits(myNode, limits_control, true, - ampControl, true, ampControl, true, -ampControl, true, ampControl);
    controlGraphicScene->addItem(&myNode);
}

void ControlDialog::linkCloud(Cloud *cloudLinked)
{
    linking = true;
    cloudRef = cloudLinked;
    scaleRef = cloudLinked->getScale();
    controlGraphicsView->linkCloud(cloudLinked, this);
    if (!cloudRef->getActiveState())
        myNode.setActiveState(false);
    updateFromControlPosition(0, 0);
    drawScale();
    linking = false;
}

void ControlDialog::updateFromControlPosition(float l_x, float l_y)
{
    //cout << "updadefromcontrolpos, l_x = " << l_x <<", l_y = " << l_y << endl;
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
            cloudRef->setCtrlInterval(-(l_y * zoom), false);
            cloudRef->setCtrlShade(float(l_x + ampControl) / ampControl, false);
        }
        else {
            cloudRef->setCtrlInterval(l_x * zoom, false);
            cloudRef->setCtrlShade(2 - float(l_y + ampControl) / ampControl, false);
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
    int n_interval;
    if (scaleAttraction()) {
        double l_interval = ui->doubleSpinBox_Interval->value();
        double l_nearestPosition = nearestScalePosition(l_interval, getMinInterval(), getMaxInterval());
        n_interval = - l_nearestPosition / ui->doubleSpinBox_Zoom->value();
    }
    else
        n_interval = - (ui->doubleSpinBox_Interval->value() / ui->doubleSpinBox_Zoom->value());

    if (orientation == VERTICAL) {
        updateFromControlPosition(n_shade, n_interval);
    }
    else {
        updateFromControlPosition(-n_interval, -n_shade);
    }
}

void ControlDialog::updateMinMax()
{
    float n_interval = ui->doubleSpinBox_Interval->value();
    float n_zoom = ui->doubleSpinBox_Zoom->value();

    float n_zoomMin = (n_interval / ampControl);
    ui->doubleSpinBox_Zoom->setMinimum(abs(n_zoomMin));
    float n_intervalMax = (n_zoom * ampControl);
    ui->doubleSpinBox_Interval->setMaximum(abs(n_intervalMax));
    ui->doubleSpinBox_Interval->setMinimum(-abs(n_intervalMax));
}

void ControlDialog::updateValues(float l_interval, float l_shade)
{
    ui->doubleSpinBox_Interval->setValue(l_interval);
    ui->doubleSpinBox_Shade->setValue(l_shade);
    updateControlPosition();
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

float ControlDialog::getZoom()
{
    return zoom;
}

void ControlDialog::setOrientation(int l_orientation)
{
    orientation = l_orientation;
}

int ControlDialog::getOrientation()
{
    return orientation;
}

double ControlDialog::getInterval()
{
    //cout << "getinterval, value = " << ui->doubleSpinBox_Interval->value() << endl;
    return ui->doubleSpinBox_Interval->value();
}

void ControlDialog::setInterval(double n_interval)
{
    ui->doubleSpinBox_Interval->setValue(n_interval);
    updateControlPosition();
    updateMinMax();
}

double ControlDialog::getMinInterval()
{
    return ui->doubleSpinBox_Interval->minimum();
}

double ControlDialog::getMaxInterval()
{
    return ui->doubleSpinBox_Interval->maximum();
}

void ControlDialog::drawScale()
{

    controlGraphicScene->removeItem(&myNode);
    controlGraphicScene->clear();
    initScene();

    QPen graypen (Qt::gray);

    for (int i = 0; i < scaleRef->getSize(); i = i + 1) {
        if (orientation == VERTICAL) {
            int l_y = - (scaleRef->getScalePosition(i).pitchInterval / ui->doubleSpinBox_Zoom->value());
            if ((l_y >= - ampControl) && (l_y <= ampControl)) {
                QGraphicsItem *l_line;
                l_line = controlGraphicScene->addLine(-(ampControl + 10),
                                                      l_y,
                                                      ampControl + 10,
                                                      l_y,
                                                      graypen);
            }
        }
        else {
            int l_y = (scaleRef->getScalePosition(i).pitchInterval / ui->doubleSpinBox_Zoom->value());
            if ((l_y >= - ampControl) && (l_y <= ampControl)) {
                QGraphicsItem *l_line;
                l_line = controlGraphicScene->addLine(l_y,
                                                      -(ampControl + 10),
                                                      l_y,
                                                      ampControl + 10,
                                                      graypen);
            }
        }
    }
}

double ControlDialog::nearestScalePosition(double c_interval, double c_minInterval, double c_maxInterval)
{
    return scaleRef->nearest(c_interval, c_minInterval, c_maxInterval);
}

double ControlDialog::getIntervalFromScale(int l_i)
{
    return scaleRef->getScalePosition(l_i - 1).pitchInterval;
}

bool ControlDialog::scaleAttraction()
{
    return cloudRef->scaleAttraction();
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
    //cout << "mousepressevent" << endl;
    if (eventMouse->button() == 2) {
       myNode->setActiveState(false);
       cloudRef->getPhrase()->setRestart(false);
       cloudRef->setActiveState(false);
   }
   else {
       myNode->setActiveState(true);
       cloudRef->getPhrase()->setRestart(false);
       //if (!cloudRef->getActiveState())

       cloudRef->setActiveState(false);
       cloudRef->setActiveState(true);
   }
   int c_x = eventMouse->x() - (ampControl + (myNode->getWidthNodes() + 3));
   int c_y = eventMouse->y() - (ampControl + (myNode->getWidthNodes() + 3));

   myControl->updateFromControlPosition(c_x, c_y);
   draging = true;

/*   if ((c_x > (myNode->x() - (myNode->getWidthNodes()/2 + 2))) && (c_x < (myNode->x() + (myNode->getWidthNodes()/2 + 2))) &&
       (c_y > (myNode->y() - (myNode->getWidthNodes()/2 + 2))) && (c_y < (myNode->y() + (myNode->getWidthNodes()/2 + 2)))) {
       draging = true;
   }
   else {
       myControl->updateFromControlPosition(c_x, c_y);
       draging = true;
   }*/
}

void MyQGraphicsView::mouseReleaseEvent(QMouseEvent *eventMouse)
{
    //cout << "mouserelease" << endl;
    draging = false;
    if (myControl->scaleAttraction()) {
        double l_interval = myControl->getInterval();
        double l_nearestPosition = myControl->nearestScalePosition(l_interval, myControl->getMinInterval(), myControl->getMaxInterval());
        myControl->setInterval(l_nearestPosition);
    }
}

void MyQGraphicsView::mouseMoveEvent(QMouseEvent *eventMouse)
{
    if (draging) {
        int c_x = eventMouse->x() - (ampControl + (myNode->getWidthNodes() + 3));
        int c_y = eventMouse->y() - (ampControl + (myNode->getWidthNodes() + 3));
        if ((c_x != old_c_x) || (c_y != old_c_y)) {
            myControl->updateFromControlPosition(c_x, c_y);
            old_c_x = c_x;
            old_c_y = c_y;
        }
    }

}

void ControlDialog::on_doubleSpinBox_Zoom_valueChanged(double arg1)
{
    zoom = arg1;
}

void ControlDialog::on_radioButton_orientation_vertical_toggled(bool checked)
{
    if (checked) {
        orientation =  VERTICAL;
        updateControlPosition();
        drawScale();
    }
}

void ControlDialog::on_radioButton_orientation_horizontal_toggled(bool checked)
{
    if (checked) {
        orientation =  HORIZONTAL;
        updateControlPosition();
        drawScale();
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

void ControlDialog::on_doubleSpinBox_Zoom_editingFinished()
{
    zoom = ui->doubleSpinBox_Zoom->value();
    updateControlPosition();
    updateMinMax();
    drawScale();
}

void ControlDialog::on_pushButton_add_pressed()
{
    ScalePosition l_scalePosition;
    l_scalePosition.pitchInterval = ui->doubleSpinBox_Interval->value();
    scaleRef->insertScalePosition(l_scalePosition);

    drawScale();
}

void ControlDialog::on_pushButton_reset_pressed()
{
    controlGraphicScene->removeItem(&myNode);
    controlGraphicScene->clear();
    scaleRef->reset();
    initScene();
}

void ControlDialog::on_pushButton_attraction_toggled(bool checked)
{
    cloudRef->setScaleAttraction(checked);
}

void ControlDialog::on_pushButton_save_pressed()
{
    std::string nameScaleFile = scaleRef->askNameScale(FileDirection::Save);
    if (nameScaleFile.empty())
        return;

    QFile scaleFile(QString::fromStdString(nameScaleFile));
    scaleRef->save(scaleFile);
}

void ControlDialog::on_pushButton_load_pressed()
{
    std::string nameScaleFile = scaleRef->askNameScale(FileDirection::Load);
    if (nameScaleFile.empty())
        return;
    
    controlGraphicScene->removeItem(&myNode);
    controlGraphicScene->clear();
    scaleRef->reset();
    
    QFile scaleFile(QString::fromStdString(nameScaleFile));
    scaleRef->load(scaleFile);

    initScene();
    drawScale();
}
