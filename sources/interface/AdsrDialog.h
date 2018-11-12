#ifndef ADSRDIALOG_H
#define ADSRDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include "model/Adsr.h"
#include "interface/Node.h"

extern unsigned int samp_rate;

namespace Ui {
class AdsrDialog;
}

class AdsrDialog : public QDialog
{
    Q_OBJECT
struct EnvelopeNode {
    QPointF center;
    QRect rect;
};
public slots:
    void updateFromGraph();
public:
    explicit AdsrDialog(QWidget *parent = 0);
    ~AdsrDialog();
    void modifEnvelope(ParamEnv &paramEnvForDialog);
    void setCoord (QPointF &pointNode, float newX, float newY);
    void setLimits(Node &nodeToLimit, NodeLimits &nodeLimitsToGive,
                   bool t_minX, float valueMinX,
                   bool t_maxX, float valueMaxX,
                   bool t_minY, float valueMinY,
                   bool t_maxY, float valueMaxY);
private slots:
    void on_doubleSpinBox_r_4_valueChanged(double arg1);

    void on_doubleSpinBox_r_3_valueChanged(double arg1);

    void on_doubleSpinBox_l_1_valueChanged(double arg1);

    void on_doubleSpinBox_r_1_valueChanged(double arg1);

    void on_doubleSpinBox_l_2_valueChanged(double arg1);

    void on_doubleSpinBox_r_2_valueChanged(double arg1);

    void on_doubleSpinBox_l_3_valueChanged(double arg1);

    void on_doubleSpinBox_l_4_valueChanged(double arg1);

private:
    Ui::AdsrDialog *ui;
    QGraphicsScene *envelopeGraphicScene;
    //QGraphicsEllipseItem *ellipse_begin, *ellipse_l1, *ellipse_l2, *ellipse_l3, *ellipse_l4, *ellipse_end;
    Node graphicNode_begin, graphicNode_l1, graphicNode_l2, graphicNode_l3, graphicNode_l4, graphicNode_end;
    QGraphicsLineItem *line_r1, *line_r2, *line_r3, *line_r4;
    QRectF rectEnvelope;
    float tr1, tr2, tr3, tr4;
    QPointF pointNode_begin, pointNode_l1, pointNode_l2, pointNode_l3, pointNode_l4, pointNode_end;
    NodeLimits limits_begin, limits_l1, limits_l2, limits_l3, limits_l4, limits_end;
    int widthNodes = 10;
    int heightMaxAff = 180;
    int coeffAffMS = 50;
    int xSustain = 40;
    bool affUpdated = false;
    bool editing = false;
    float valueEditing;
    float currentXMove = 0;
    bool mouseIsPressed = false;

};

#endif // ADSRDIALOG_H
