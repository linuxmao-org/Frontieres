#ifndef ADSRDIALOG_H
#define ADSRDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include "model/Adsr.h"

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

public:
    explicit AdsrDialog(QWidget *parent = 0);
    ~AdsrDialog();
    void modifEnvelope(ParamEnv &paramEnvForDialog);
    void setCoord (EnvelopeNode &Node, float newX, float newY);
    void updateAff();
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
    QGraphicsEllipseItem *ellipse_begin, *ellipse_l1, *ellipse_l2, *ellipse_l3, *ellipse_l4, *ellipse_end;
    QGraphicsLineItem *line_r1, *line_r2, *line_r3, *line_r4;
    float tr1, tr2, tr3, tr4;
    EnvelopeNode node_begin, node_l1, node_l2, node_l3, node_l4, node_end;
    int widthNodes = 5;
    int heightMaxAff = 200;
    int coeffAffMS = 50;
    int xSustain = 40;
};

#endif // ADSRDIALOG_H
