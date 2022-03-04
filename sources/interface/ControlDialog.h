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
#ifndef CONTROLDIALOG_H
#define CONTROLDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QSizePolicy>
#include <QString>
#include "theglobals.h"
#include "model/Cloud.h"
#include "model/Scale.h"
#include "interface/Node.h"

struct ControlPosition {
public:
    Node myNode;
    QPointF myPointNode;
    NodeLimits myLimits;
};

class MyControlQGraphicsView;

namespace Ui {
class ControlDialog;
}

class ControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ControlDialog(QWidget *parent = nullptr);
    ~ControlDialog();
    void initScene();
    void linkCloud (Cloud *cloudLinked);

    void updateFromControlPosition(float l_x, float l_y);
    void updateControlPosition();
    void updateMinMax();
    void updateValues(float l_interval, float l_shade);
    void setLimits(Node &nodeToLimit, NodeLimits &nodeLimitsToGive,
                   bool t_minX, float valueMinX,
                   bool t_maxX, float valueMaxX,
                   bool t_minY, float valueMinY,
                   bool t_maxY, float valueMaxY);
    void setActiveState(bool l_activeState);
    bool getActiveState();
    float getZoom();
    void setZoom (float l_zoom);
    void setOrientation(int l_orientation);
    int getOrientation();
    double getInterval();
    void setInterval(double n_interval);
    double getMinInterval();
    double getMaxInterval();
    void drawScale();
    double nearestScalePosition(double c_interval, double c_minInterval, double c_maxInterval);
    double getIntervalFromScale(int l_i);
    bool scaleAttraction();
    std::string askNameScale(FileDirection direction);

private slots:

    void on_doubleSpinBox_Zoom_valueChanged(double arg1);

    void on_radioButton_orientation_vertical_toggled(bool checked);

    void on_radioButton_orientation_horizontal_toggled(bool checked);

    void on_doubleSpinBox_Shade_editingFinished();

    void on_doubleSpinBox_Interval_editingFinished();

    void on_doubleSpinBox_Zoom_editingFinished();

    void autoUpdate();

    void on_checkBox_scale_attraction_clicked(bool checked);

private:
    const int ampControl = 240;
    Ui::ControlDialog *ui;
    bool linking = false;
    Cloud *cloudRef;
    bool autoUpdating = false;
    Node myNode;
    NodeLimits limits_control;
    QGraphicsScene *controlGraphicScene;
    MyControlQGraphicsView *controlGraphicsView;
    QRectF rectControl;
    ControlPosition myControlPosition;
    bool activeState = true;
    float zoom = 0.05f;
    bool orientation = VERTICAL;
    bool myScaleAttraction = false;
    Scale *scaleRef = nullptr;
//    vector<ScalePosition *> myScalePositions;
};

class MyControlQGraphicsView : public QGraphicsView
{
       Q_OBJECT
public:
    explicit MyControlQGraphicsView(QWidget *parent, ControlDialog *l_controlDialog, QGraphicsScene *l_graphicsScene,Node *l_Node);
    void linkCloud (Cloud *cloudLinked, ControlDialog *l_controldialog);
signals:

public slots:
    void mousePressEvent(QMouseEvent * eventMouse);
    void mouseReleaseEvent(QMouseEvent * eventMouse);
    // void mouseDoubleClickEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * eventMouse);
private:
    const int ampControl = 240;
    Cloud *cloudRef;
    Node *myNode;
    QGraphicsScene *myScene;
    ControlDialog *myControl;
    bool draging = false;
    int old_c_x, old_c_y;
};

#endif // CONTROLDIALOG_H
