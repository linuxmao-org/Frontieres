#ifndef CONTROLDIALOG_H
#define CONTROLDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QSizePolicy>
#include <QString>
#include "model/Cloud.h"
#include "model/Scale.h"
#include "interface/Node.h"

enum {VERTICAL, HORIZONTAL};

struct ControlPosition {
public:
    Node myNode;
    QPointF myPointNode;
    NodeLimits myLimits;
};

class MyQGraphicsView;

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

    void on_pushButton_add_pressed();

    void on_pushButton_reset_pressed();

    void on_pushButton_attraction_toggled(bool checked);

    void on_pushButton_save_pressed();

    void on_pushButton_load_pressed();

private:
    const int ampControl = 240;
    Ui::ControlDialog *ui;
    bool linking = false;
    Cloud *cloudRef;
    Node myNode;
    NodeLimits limits_control;
    QGraphicsScene *controlGraphicScene;
    MyQGraphicsView *controlGraphicsView;
    QRectF rectControl;
    ControlPosition myControlPosition;
    bool activeState = true;
    float zoom = 0.05f;
    bool orientation = VERTICAL;
    bool myScaleAttraction = false;
    Scale *scaleRef;
//    vector<ScalePosition *> myScalePositions;
};

class MyQGraphicsView : public QGraphicsView
{
       Q_OBJECT
public:
    explicit MyQGraphicsView(QWidget *parent, ControlDialog *l_controlDialog, QGraphicsScene *l_graphicsScene,Node *l_Node);
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
};

#endif // CONTROLDIALOG_H
