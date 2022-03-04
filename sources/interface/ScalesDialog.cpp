//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     https://github.com/linuxmao-org/Frontieres
//
// Copyright (C) 2022  Olivier Flatres
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
#include "ScalesDialog.h"
#include "ui_ScalesDialog.h"
#include <iostream>
#include <stdio.h>

ScalesDialog::ScalesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScalesDialog)
{
    ui->setupUi(this);

    controlGraphicScene = new QGraphicsScene(this);
    controlGraphicsView = new MyQGraphicsView(ui->frameGraphicsView, this, controlGraphicScene, &myNode);

    setMouseTracking(true);

    initScene();

    myNode.setX(0);
    myNode.setY(0);

    zoom = float(ui->doubleSpinBox_Zoom->value());
}

ScalesDialog::~ScalesDialog()
{
    delete ui;
}

void ScalesDialog::linkScale(Scale *scaleLinked)
{
    scaleRef = scaleLinked;
    drawScale();
}

void ScalesDialog::init(Scene *currentScene)
{
    sceneRef = currentScene;
    ui->doubleSpinBox_Scale_Number->setMaximum(sceneRef->getScalesSize());
    linkScale(sceneRef->findScaleById(int(ui->doubleSpinBox_Scale_Number->value()))->scale.get());
}

void ScalesDialog::initScene()
{
    QPen blackPen (Qt::black);
    QPen graypen (Qt::gray);
    blackPen.setWidth(1);

    QRectF zoneControl;
    zoneControl.setX(-ampHorzControl);
    zoneControl.setY(-ampControl);
    zoneControl.setWidth(ampHorzControl * 2);
    zoneControl.setHeight(ampControl * 2);

    QLine vertLine;
    QLine horzLine;

    QBrush *whiteBrush = new QBrush(Qt::white);

    controlGraphicsView->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));

    controlGraphicScene->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));
    controlGraphicScene->addRect(zoneControl, blackPen, *whiteBrush);

    vertLine.setLine(0,-(ampControl + 10),0,(ampControl + 10));
    horzLine.setLine(-(ampHorzControl + 10),0,ampHorzControl + 10,0);
    controlGraphicScene->addLine(vertLine,blackPen);
    controlGraphicScene->addLine(horzLine,blackPen);

    setLimits(myNode, limits_control, true, - ampHorzControl, true, ampHorzControl, true, -ampControl, true, ampControl);
    controlGraphicScene->addItem(&myNode);
}

void ScalesDialog::updateFromControlPosition(float l_x, float l_y)
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
    cout << "inter 1 updadefromcontrolpos" << endl;
    if (!c_limited) {
        myNode.moveToPos(l_x, l_y);
        //ui->doubleSpinBox_Interval->setValue(double(-(l_y * zoom)));

        updateMinMax();
    }
    else {
        //cout << "limited" << endl;
    }
    //cout << "sortie updadefromcontrolpos" << endl;
}

void ScalesDialog::updateControlPosition()
{
    if (linking)
        return;

    int n_interval;
    n_interval = int(- (ui->doubleSpinBox_Interval->value() / ui->doubleSpinBox_Zoom->value()));

    updateFromControlPosition(0, n_interval);
}

void ScalesDialog::updateMinMax()
{
    float n_interval = float(ui->doubleSpinBox_Interval->value());
    float n_zoom = float(ui->doubleSpinBox_Zoom->value());

    float n_zoomMin = (n_interval / ampControl);
    ui->doubleSpinBox_Zoom->setMinimum(double(abs(n_zoomMin)));
    float n_intervalMax = (n_zoom * ampControl);
    ui->doubleSpinBox_Interval->setMaximum(double(abs(n_intervalMax)));
    ui->doubleSpinBox_Interval->setMinimum(double(-abs(n_intervalMax)));
}

void ScalesDialog::setLimits(Node &nodeToLimit, NodeLimits &nodeLimitsToGive,
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

void ScalesDialog::setActiveState(bool l_activeState)
{
    activeState = l_activeState;
}

bool ScalesDialog::getActiveState()
{
    return activeState;
}

float ScalesDialog::getZoom()
{
    return zoom;
}

double ScalesDialog::getInterval()
{
    //cout << "getinterval, value = " << ui->doubleSpinBox_Interval->value() << endl;
    return ui->doubleSpinBox_Interval->value();
}

void ScalesDialog::setInterval(double n_interval)
{
    ui->doubleSpinBox_Interval->setValue(n_interval);
    updateControlPosition();
    updateMinMax();
}

double ScalesDialog::getMinInterval()
{
    return ui->doubleSpinBox_Interval->minimum();
}

double ScalesDialog::getMaxInterval()
{
    return ui->doubleSpinBox_Interval->maximum();
}

void ScalesDialog::drawScale()
{
    cout << "entree drawscale" << endl;

    controlGraphicScene->removeItem(&myNode);
    controlGraphicScene->clear();
    initScene();

    QPen graypen (Qt::gray);

    for (unsigned long i = 0; i < scaleRef->getSize(); i = i + 1) {
        int l_y = int(- (scaleRef->getScalePosition(i).pitchInterval / ui->doubleSpinBox_Zoom->value()));
        if ((l_y >= - ampControl) && (l_y <= ampControl)) {
            QGraphicsItem *l_line;
            l_line = controlGraphicScene->addLine(-(ampHorzControl + 10),
                                                  l_y,
                                                  ampHorzControl + 10,
                                                  l_y,
                                                  graypen);
        }
    }

    resetTreeScales();
    cout << "sortie drawscale" << endl;
}

double ScalesDialog::nearestScalePosition(double c_interval, double c_minInterval, double c_maxInterval)
{
    return scaleRef->nearest(c_interval, c_minInterval, c_maxInterval);
}

double ScalesDialog::getIntervalFromScale(int l_i)
{
    return scaleRef->getScalePosition(unsigned(l_i - 1)).pitchInterval;
}

void ScalesDialog::resetTreeScales()
{
    //ui->treeWidget_Scale->clear();
    ui->tableWidget_Scale->clear();
    //QTreeWidgetItem *itm = new QTreeWidgetItem(ui->treeWidget_Scale);
    ui->tableWidget_Scale->setRowCount(scaleRef->getSize());
    ui->tableWidget_Scale->setColumnCount(1);

    //itm->setText(0, tr("Interval"));
    //ui->treeWidget_Scale->addTopLevelItem(itm);
    for (unsigned long i=0; i < scaleRef->getSize() ; i++) {
       // QTreeWidgetItem *itmChild = new QTreeWidgetItem();
       // itmChild->setText(0, QString::number(scaleRef->getScalePosition(scaleRef->getSize()-(i+1)).pitchInterval));
       // itm->addChild(itmChild);

        ui->tableWidget_Scale->setItem(0, i, new QTableWidgetItem(QString::number(scaleRef->getScalePosition(scaleRef->getSize()-(i+1)).pitchInterval)));



    }
}


MyQGraphicsView::MyQGraphicsView(QWidget *parent, ScalesDialog *l_ScalesDialog, QGraphicsScene *l_graphicsScene, Node *l_Node)
{
    this->setParent(parent);
    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum );
    this->setMaximumSize(parent->maximumSize());
    this->setMinimumSize(parent->minimumSize());
    this->setFrameShape(Shape::NoFrame);
    this->setScene(l_graphicsScene);
    myNode = l_Node;
}

void MyQGraphicsView::mousePressEvent(QMouseEvent *eventMouse)
{
    return;
    //cout << "mousepressevent" << endl;

    if (eventMouse->button() == 2) {
       myNode->setActiveState(false);
   }
   else {
       myNode->setActiveState(true);
   }
   int c_x = eventMouse->x() - (ampControl + (myNode->getWidthNodes() + 3));
   int c_y = eventMouse->y() - (ampControl + (myNode->getWidthNodes() + 3));

       //cout << "mousepressevent avant update" << endl;
   myControl->updateFromControlPosition(c_x, c_y);
   draging = true;


}

void MyQGraphicsView::mouseReleaseEvent(QMouseEvent *eventMouse)
{
    return;
    cout << "mouserelease" << endl;
    draging = false;
}

void MyQGraphicsView::mouseMoveEvent(QMouseEvent *eventMouse)
{
    return;
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

void ScalesDialog::on_doubleSpinBox_Zoom_valueChanged(double arg1)
{
    zoom = float(arg1);
    updateControlPosition();
}

void ScalesDialog::on_doubleSpinBox_Interval_editingFinished()
{
    updateControlPosition();
    updateMinMax();
}

void ScalesDialog::on_doubleSpinBox_Zoom_editingFinished()
{
    zoom = float(ui->doubleSpinBox_Zoom->value());
    updateControlPosition();
    updateMinMax();
    drawScale();
}

void ScalesDialog::on_pushButton_add_pressed()
{
    ScalePosition l_scalePosition;
    l_scalePosition.pitchInterval = ui->doubleSpinBox_Interval->value();
    scaleRef->insertScalePosition(l_scalePosition);

    drawScale();
}

void ScalesDialog::on_pushButton_reset_pressed()
{
    controlGraphicScene->removeItem(&myNode);
    controlGraphicScene->clear();
    scaleRef->reset();
    initScene();
}

void ScalesDialog::on_pushButton_save_pressed()
{
    std::string nameScaleFile = scaleRef->askNameScale(FileDirection::Save);
    if (nameScaleFile.empty())
        return;

    QFile scaleFile(QString::fromStdString(nameScaleFile));
    scaleRef->save(scaleFile);
}

void ScalesDialog::on_pushButton_load_pressed()
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


void ScalesDialog::on_doubleSpinBox_Scale_Number_editingFinished()
{
    //int l_num = int(ui->doubleSpinBox_Scale_Number->value());

    //cout << "scale num =" << l_num << endl;

    linkScale(sceneRef->findScaleById(unsigned(ui->doubleSpinBox_Scale_Number->value()))->scale.get());
    ui->lineEdit_Name->setText(scaleRef->getName());

    drawScale();
}

void ScalesDialog::on_pushButton_Scale_Add_pressed()
{
    sceneRef->addNewScale();
    ui->doubleSpinBox_Scale_Number->setMaximum(ui->doubleSpinBox_Scale_Number->maximum()+1);
}

void ScalesDialog::on_lineEdit_Name_editingFinished()
{
    scaleRef->setName(ui->lineEdit_Name->text());
}
