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
#ifndef SCALESDIALOG_H
#define SCALESDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QSizePolicy>
#include <QString>
#include "theglobals.h"
#include "model/Scene.h"
#include "model/Scale.h"
#include "interface/Node.h"

struct ScaleControlPosition {
public:
    Node myNode;
    QPointF myPointNode;
    NodeLimits myLimits;
};

class MyQGraphicsView;

namespace Ui {
class ScalesDialog;
}

class ScalesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScalesDialog(QWidget *parent = nullptr);
    ~ScalesDialog();

    void linkScale (Scale *scaleLinked);
    void on_doubleSpinBox_Shade_editingFinished();
    void init(Scene *currentScene);
    void initScene();
    void updateFromControlPosition(float l_x, float l_y);
    void updateControlPosition();
    void updateMinMax();
    void setLimits(Node &nodeToLimit, NodeLimits &nodeLimitsToGive,
                   bool t_minX, float valueMinX,
                   bool t_maxX, float valueMaxX,
                   bool t_minY, float valueMinY,
                   bool t_maxY, float valueMaxY);
    void setActiveState(bool l_activeState);
    bool getActiveState();
    float getZoom();
    void setZoom (float l_zoom);
    double getInterval();
    void setInterval(double n_interval);
    double getMinInterval();
    double getMaxInterval();
    void drawScale();
    double nearestScalePosition(double c_interval, double c_minInterval, double c_maxInterval);
    double getIntervalFromScale(int l_i);
    std::string askNameScale(FileDirection direction);
    void resetTreeScales();

private slots:

    void on_doubleSpinBox_Zoom_valueChanged(double arg1);

    void on_doubleSpinBox_Interval_editingFinished();

    void on_doubleSpinBox_Zoom_editingFinished();

    void on_pushButton_add_pressed();

    void on_pushButton_reset_pressed();

    void on_pushButton_save_pressed();

    void on_pushButton_load_pressed();

    void on_doubleSpinBox_Scale_Number_editingFinished();

    void on_pushButton_Scale_Add_pressed();

    void on_lineEdit_Name_editingFinished();

private:
    const int ampControl = 240;
    const int ampHorzControl = 5;
    Ui::ScalesDialog *ui;
    bool linking = false;
    //Cloud *cloudRef;
    Scene *sceneRef;
    Scale *scaleRef;
    bool autoUpdating = false;
    Node myNode;
    NodeLimits limits_control;
    QGraphicsScene *controlGraphicScene;
    MyQGraphicsView *controlGraphicsView;
    QRectF rectControl;
    ScalePosition myControlPosition;
    bool activeState = true;
    float zoom = 0.05f;
//    vector<ScalePosition *> myScalePositions;
};

class MyQGraphicsView : public QGraphicsView
{
       Q_OBJECT
public:
    explicit MyQGraphicsView(QWidget *parent, ScalesDialog *l_ScalesDialog, QGraphicsScene *l_graphicsScene,Node *l_Node);

signals:

public slots:
    void mousePressEvent(QMouseEvent * eventMouse);
    void mouseReleaseEvent(QMouseEvent * eventMouse);
    // void mouseDoubleClickEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * eventMouse);
private:
    const int ampControl = 240;
    const int ampHorzControl = 5;
    //    Cloud *cloudRef;

    Node *myNode;
    QGraphicsScene *myScene;
    ScalesDialog *myControl;
    bool draging = false;
    int old_c_x, old_c_y;
};

#endif // SCALESDIALOG_H
