#include "interface/AdsrDialog.h"
#include "ui_AdsrDialog.h"
#include "model/ParamAdsr.h"
#include <iostream>
#include <stdio.h>

ParamEnv paramEnvDialog;

AdsrDialog::AdsrDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdsrDialog)
{
    setModal(false);
    ui->setupUi(this);

    envelopeGraphicScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(envelopeGraphicScene);

    setMouseTracking(true);

    QPen blackPen (Qt::black);
    blackPen.setWidth(1);

    setCoord(pointNode_begin, 0, 0);
    setCoord(pointNode_l1, 0, 0);
    setCoord(pointNode_l2, 0, 0);
    setCoord(pointNode_l3, 0, 0);
    setCoord(pointNode_l4, 0, 0);
    setCoord(pointNode_end, 0, 0);

    line_r1 = envelopeGraphicScene->addLine(0, 0, 0, 0, blackPen);
    line_r2 = envelopeGraphicScene->addLine(0, 0, 0, 0, blackPen);
    line_r3 = envelopeGraphicScene->addLine(0, 0, 0, 0, blackPen);
    line_r4 = envelopeGraphicScene->addLine(0, 0, 0, 0, blackPen);

    envelopeGraphicScene->addItem(&graphicNode_begin);
    envelopeGraphicScene->addItem(&graphicNode_l1);
    envelopeGraphicScene->addItem(&graphicNode_l2);
    envelopeGraphicScene->addItem(&graphicNode_l3);
    envelopeGraphicScene->addItem(&graphicNode_l4);
    envelopeGraphicScene->addItem(&graphicNode_end);

    connect(envelopeGraphicScene, SIGNAL(changed(QList<QRectF>)),
            this, SLOT(updateFromGraph()));
}

AdsrDialog::~AdsrDialog()
{
    delete ui;
}

void AdsrDialog::modifEnvelope(ParamEnv &paramEnvForDialog)
{
    paramEnvDialog = paramEnvForDialog;
    paramEnvDialog.getTimeBasedParameters(tr1, tr2, tr3, tr4, samp_rate);

    ui->doubleSpinBox_l_1->setValue(paramEnvDialog.l1);
    ui->doubleSpinBox_r_1->setValue(tr1);
    ui->doubleSpinBox_l_2->setValue(paramEnvDialog.l2);
    ui->doubleSpinBox_r_2->setValue(tr2);
    ui->doubleSpinBox_l_3->setValue(paramEnvDialog.l3);
    ui->doubleSpinBox_r_3->setValue(tr3);
    ui->doubleSpinBox_r_4->setValue(tr4);

    setLimits(graphicNode_l3, limits_l3, true, - xSustain, true, - xSustain, true, - heightMaxAff / 2, true, heightMaxAff / 2);
    setLimits(graphicNode_l4, limits_l4, true, xSustain, true, xSustain, true, - heightMaxAff / 2, true, heightMaxAff / 2);
    setLimits(graphicNode_end, limits_end, true, xSustain + 1, false, 0, true, heightMaxAff / 2, true, heightMaxAff / 2);
    setLimits(graphicNode_l2, limits_l2, false, 0, true, - xSustain - 1, true, - heightMaxAff / 2, true, heightMaxAff / 2);
    setLimits(graphicNode_l1, limits_l1, false, 0, true, graphicNode_l2.x() - 1, true, - heightMaxAff / 2, true, heightMaxAff / 2);
    setLimits(graphicNode_begin, limits_begin, false, 0, true, graphicNode_l1.x() - 1, true, heightMaxAff / 2, true, heightMaxAff / 2);

    setModal(true);
    this->exec();
    double l1 = paramEnvDialog.l1;
    double l2 = paramEnvDialog.l2;
    double l3 = paramEnvDialog.l3;
    paramEnvDialog.setTimeBasedParameters(l1, l2, l3, tr1, tr2, tr3, tr4, samp_rate);
    paramEnvForDialog = paramEnvDialog;
}

void AdsrDialog::setCoord(QPointF &pointNode, float newX, float newY)
{
    pointNode.setX(newX);
    pointNode.setY(newY);
}

void AdsrDialog::setLimits(Node &nodeToLimit, NodeLimits &nodeLimitsToGive,
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

void AdsrDialog::updateFromGraph()
{
    if (graphicNode_end.moved()){
        ui->doubleSpinBox_r_4->setValue((graphicNode_end.x() - graphicNode_l4.x()) / coeffAffMS);
        graphicNode_end.endMove();
    }
    if (graphicNode_begin.moved()){
        ui->doubleSpinBox_r_1->setValue((graphicNode_l1.x() - graphicNode_begin.x()) / coeffAffMS);
        graphicNode_begin.endMove();
    }
    if (graphicNode_l4.moved()){
        ui->doubleSpinBox_l_4->setValue((graphicNode_l4.y() - (heightMaxAff / 2)) / (- heightMaxAff));
        graphicNode_l4.endMove();
    }
    if (graphicNode_l3.moved()){
        ui->doubleSpinBox_l_3->setValue((graphicNode_l3.y() - (heightMaxAff / 2)) / (- heightMaxAff));
        graphicNode_l3.endMove();
    }
    if (graphicNode_l1.moved()){
        ui->doubleSpinBox_l_1->setValue((graphicNode_l1.y() - (heightMaxAff / 2)) / (- heightMaxAff));
        ui->doubleSpinBox_r_2->setValue((graphicNode_l2.x() - graphicNode_l1.x()) / coeffAffMS);
        graphicNode_l1.endMove();
    }
    if (graphicNode_l2.moved()){
        ui->doubleSpinBox_l_2->setValue((graphicNode_l2.y() - (heightMaxAff / 2)) / (- heightMaxAff));
        ui->doubleSpinBox_r_3->setValue((graphicNode_l3.x() - graphicNode_l2.x()) / coeffAffMS);
        graphicNode_l2.endMove();
    }
}

void AdsrDialog::on_doubleSpinBox_r_4_valueChanged(double arg1)
{
    tr4 = arg1;
    setCoord(pointNode_end,
             xSustain + ui->doubleSpinBox_r_4->value() * coeffAffMS,
             heightMaxAff / 2);
    graphicNode_end.moveToPos(pointNode_end.x(), pointNode_end.y());
    line_r4->setLine(graphicNode_l4.x(),
                     graphicNode_l4.y(),
                     graphicNode_end.x(),
                     graphicNode_end.y());
}

void AdsrDialog::on_doubleSpinBox_r_3_valueChanged(double arg1)
{
    tr3 = arg1;
    setCoord(pointNode_l2,
             - xSustain - ui->doubleSpinBox_r_3->value() * coeffAffMS,
             ui->doubleSpinBox_l_2->value() * (- heightMaxAff) + (heightMaxAff / 2));
    graphicNode_l2.moveToPos(pointNode_l2.x(), pointNode_l2.y());
    line_r3->setLine(graphicNode_l2.x(),
                     graphicNode_l2.y(),
                     graphicNode_l3.x(),
                     graphicNode_l3.y());
    setCoord(pointNode_l1,
             pointNode_l2.x() - ui->doubleSpinBox_r_2->value() * coeffAffMS,
             ui->doubleSpinBox_l_1->value() * (- heightMaxAff) + (heightMaxAff / 2));
    graphicNode_l1.moveToPos(pointNode_l1.x(), pointNode_l1.y());
    line_r2->setLine(graphicNode_l1.x(),
                     graphicNode_l1.y(),
                     graphicNode_l2.x(),
                     graphicNode_l2.y());
    setCoord(pointNode_begin,
             pointNode_l1.x() - ui->doubleSpinBox_r_1->value() * coeffAffMS,
             heightMaxAff / 2);
    graphicNode_begin.moveToPos(pointNode_begin.x(), pointNode_begin.y());
    line_r1->setLine(graphicNode_begin.x(),
                     graphicNode_begin.y(),
                     graphicNode_l1.x(),
                     graphicNode_l1.y());
 }

void AdsrDialog::on_doubleSpinBox_l_1_valueChanged(double arg1)
{
     paramEnvDialog.l1 = arg1;
     setCoord(pointNode_l1,
              pointNode_l2.x() - ui->doubleSpinBox_r_2->value() * coeffAffMS,
              ui->doubleSpinBox_l_1->value() * (- heightMaxAff) + (heightMaxAff / 2));
     graphicNode_l1.moveToPos(pointNode_l1.x(), pointNode_l1.y());
     line_r2->setLine(graphicNode_l1.x(),
                      graphicNode_l1.y(),
                      graphicNode_l2.x(),
                      graphicNode_l2.y());
     line_r1->setLine(graphicNode_begin.x(),
                      graphicNode_begin.y(),
                      graphicNode_l1.x(),
                      graphicNode_l1.y());
     setLimits(graphicNode_begin, limits_begin, false, 0, true, graphicNode_l1.x() - 1, true, heightMaxAff / 2, true, heightMaxAff / 2);
}

void AdsrDialog::on_doubleSpinBox_r_1_valueChanged(double arg1)
{
     tr1 = arg1;
     setCoord(pointNode_begin,
              pointNode_l1.x() - ui->doubleSpinBox_r_1->value() * coeffAffMS,
              heightMaxAff / 2);
     graphicNode_begin.moveToPos(pointNode_begin.x(), pointNode_begin.y());
     line_r1->setLine(graphicNode_begin.x(),
                      graphicNode_begin.y(),
                      graphicNode_l1.x(),
                      graphicNode_l1.y());
}

void AdsrDialog::on_doubleSpinBox_l_2_valueChanged(double arg1)
{
     paramEnvDialog.l2 = arg1;
     setCoord(pointNode_l2,
              - xSustain - ui->doubleSpinBox_r_3->value() * coeffAffMS,
              ui->doubleSpinBox_l_2->value() * (- heightMaxAff) + (heightMaxAff / 2));
     graphicNode_l2.moveToPos(pointNode_l2.x(), pointNode_l2.y());
     line_r2->setLine(graphicNode_l1.x(),
                      graphicNode_l1.y(),
                      graphicNode_l2.x(),
                      graphicNode_l2.y());
     line_r3->setLine(graphicNode_l2.x(),
                      graphicNode_l2.y(),
                      graphicNode_l3.x(),
                      graphicNode_l3.y());
     setLimits(graphicNode_begin, limits_begin, false, 0, true, graphicNode_l1.x() - 1, true, heightMaxAff / 2, true, heightMaxAff / 2);
     setLimits(graphicNode_l1, limits_l1, false, 0, true, graphicNode_l2.x() - 1, true, - heightMaxAff / 2, true, heightMaxAff / 2);
}

void AdsrDialog::on_doubleSpinBox_r_2_valueChanged(double arg1)
{
    tr2 = arg1;
    setCoord(pointNode_l2,
             - xSustain - ui->doubleSpinBox_r_3->value() * coeffAffMS,
             ui->doubleSpinBox_l_2->value() * (- heightMaxAff) + (heightMaxAff / 2));
    graphicNode_l2.moveToPos(pointNode_l2.x(), pointNode_l2.y());
    line_r3->setLine(graphicNode_l2.x(),
                     graphicNode_l2.y(),
                     graphicNode_l3.x(),
                     graphicNode_l3.y());
    setCoord(pointNode_l1,
             pointNode_l2.x() - ui->doubleSpinBox_r_2->value() * coeffAffMS,
             ui->doubleSpinBox_l_1->value() * (- heightMaxAff) + (heightMaxAff / 2));
    graphicNode_l1.moveToPos(pointNode_l1.x(), pointNode_l1.y());
    line_r2->setLine(graphicNode_l1.x(),
                     graphicNode_l1.y(),
                     graphicNode_l2.x(),
                     graphicNode_l2.y());
    setCoord(pointNode_begin,
             pointNode_l1.x() - ui->doubleSpinBox_r_1->value() * coeffAffMS,
             heightMaxAff / 2);
    graphicNode_begin.moveToPos(pointNode_begin.x(), pointNode_begin.y());
    line_r1->setLine(graphicNode_begin.x(),
                     graphicNode_begin.y(),
                     graphicNode_l1.x(),
                     graphicNode_l1.y());
    setLimits(graphicNode_begin, limits_begin, false, 0, true, graphicNode_l1.x() - 1, true, heightMaxAff / 2, true, heightMaxAff / 2);
    setLimits(graphicNode_l1, limits_l1, false, 0, true, graphicNode_l2.x() - 1, true, - heightMaxAff / 2, true, heightMaxAff / 2);
}

void AdsrDialog::on_doubleSpinBox_l_3_valueChanged(double arg1)
{
    ui->doubleSpinBox_l_4->setValue(arg1);
    paramEnvDialog.l3 = arg1;
    setCoord(pointNode_l3,
             - xSustain,
             ui->doubleSpinBox_l_3->value() * (- heightMaxAff) + (heightMaxAff / 2));
    graphicNode_l3.moveToPos(pointNode_l3.x(), pointNode_l3.y());
    line_r3->setLine(graphicNode_l2.x(),
                     graphicNode_l2.y(),
                     graphicNode_l3.x(),
                     graphicNode_l3.y());
}

void AdsrDialog::on_doubleSpinBox_l_4_valueChanged(double arg1)
{
    ui->doubleSpinBox_l_3->setValue(arg1);
    paramEnvDialog.l3 = arg1;
    setCoord(pointNode_l4,
             xSustain,
             ui->doubleSpinBox_l_4->value() * (- heightMaxAff) + (heightMaxAff / 2));
    graphicNode_l4.moveToPos(pointNode_l4.x(), pointNode_l4.y());
    line_r4->setLine(graphicNode_l4.x(),
                     graphicNode_l4.y(),
                     graphicNode_end.x(),
                     graphicNode_end.y());
}
