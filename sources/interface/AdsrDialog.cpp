#include "interface/AdsrDialog.h"
#include "ui_AdsrDialog.h"
#include "model/ParamAdsr.h"
#include <iostream>

ParamEnv paramEnvDialog;

AdsrDialog::AdsrDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdsrDialog)
{
    setModal(false);
    ui->setupUi(this);

    envelopeGraphicScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(envelopeGraphicScene);

    QBrush redBrush (Qt::red);
    QBrush blueBrush (Qt::blue);
    QBrush yellowBrush (Qt::yellow);
    QBrush blackBrush (Qt::black);
    QBrush whiteBrush (Qt::white);
    QPen blackPen (Qt::black);
    blackPen.setWidth(1);
    node_begin.rect.setWidth(widthNodes);
    node_l1.rect.setWidth(widthNodes);
    node_l2.rect.setWidth(widthNodes);
    node_l3.rect.setWidth(widthNodes);
    node_l4.rect.setWidth(widthNodes);
    node_end.rect.setWidth(widthNodes);
    setCoord(node_begin, -140, heightMaxAff / 2);
    setCoord(node_l1, -100, - heightMaxAff / 2);
    setCoord(node_l2, -60, -50);
    setCoord(node_l3, -20, -20);
    setCoord(node_l4, 20, -20);
    setCoord(node_end, 60, heightMaxAff / 2);
    line_r1 = envelopeGraphicScene->addLine(node_begin.center.x() + widthNodes,
                                            node_begin.center.y() + widthNodes,
                                            node_l1.center.x() + widthNodes,
                                            node_l1.center.y() + widthNodes, blackPen);
    line_r2 = envelopeGraphicScene->addLine(node_l1.center.x() + widthNodes,
                                            node_l1.center.y() + widthNodes,
                                            node_l2.center.x() + widthNodes,
                                            node_l2.center.y() + widthNodes, blackPen);
    line_r3 = envelopeGraphicScene->addLine(node_l2.center.x() + widthNodes,
                                            node_l2.center.y() + widthNodes,
                                            node_l3.center.x() + widthNodes,
                                            node_l3.center.y() + widthNodes, blackPen);
    line_r4 = envelopeGraphicScene->addLine(node_l4.center.x() + widthNodes,
                                            node_l4.center.y() + widthNodes,
                                            node_end.center.x() + widthNodes,
                                            node_end.center.y() + widthNodes, blackPen);
    ellipse_begin = envelopeGraphicScene->addEllipse(node_begin.center.x(), node_begin.center.y(), widthNodes * 2, widthNodes * 2, blackPen, whiteBrush);
    ellipse_l1 = envelopeGraphicScene->addEllipse(node_l1.center.x(), node_l1.center.y(), widthNodes * 2, widthNodes * 2, blackPen, redBrush);
    ellipse_l2 = envelopeGraphicScene->addEllipse(node_l2.center.x(), node_l2.center.y(), widthNodes * 2, widthNodes * 2, blackPen, blueBrush);
    ellipse_l3 = envelopeGraphicScene->addEllipse(node_l3.center.x(), node_l3.center.y(), widthNodes * 2, widthNodes * 2, blackPen, yellowBrush);
    ellipse_l4 = envelopeGraphicScene->addEllipse(node_l4.center.x(), node_l4.center.y(), widthNodes * 2, widthNodes * 2, blackPen, yellowBrush);
    ellipse_end = envelopeGraphicScene->addEllipse(node_end.center.x(), node_end.center.y(), widthNodes * 2, widthNodes * 2, blackPen, whiteBrush);

    /*ellipse_l1->setFlags(QGraphicsItem::ItemIsMovable);
    ellipse_l2->setFlags(QGraphicsItem::ItemIsMovable);
    ellipse_l3->setFlags(QGraphicsItem::ItemIsMovable);
    ellipse_l4->setFlags(QGraphicsItem::ItemIsMovable);
    ellipse_l1->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    ellipse_l2->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    ellipse_l3->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    ellipse_l4->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);*/
}

AdsrDialog::~AdsrDialog()
{
    delete ui;
}

void AdsrDialog::modifEnvelope(ParamEnv &paramEnvForDialog)
{
    paramEnvDialog = paramEnvForDialog;
    std::cout << "entree ,verif,  release : " << paramEnvDialog.r4 << std::endl;
    paramEnvDialog.getTimeBasedParameters(tr1, tr2, tr3, tr4, samp_rate);
    std::cout << "entree, release : " << tr4 << std::endl;
    ui->doubleSpinBox_l_1->setValue(paramEnvDialog.l1);
    ui->doubleSpinBox_r_1->setValue(tr1);
    ui->doubleSpinBox_l_2->setValue(paramEnvDialog.l2);
    ui->doubleSpinBox_r_2->setValue(tr2);
    ui->doubleSpinBox_l_3->setValue(paramEnvDialog.l3);
    ui->doubleSpinBox_r_3->setValue(tr3);
    ui->doubleSpinBox_r_4->setValue(tr4);
    setModal(true);
    this->exec();
    double l1 = paramEnvDialog.l1;
    double l2 = paramEnvDialog.l2;
    double l3 = paramEnvDialog.l3;
    std::cout << "sortie, release : " << tr4 << std::endl;
    paramEnvDialog.setTimeBasedParameters(l1, l2, l3, tr1, tr2, tr3, tr4, samp_rate);
    std::cout << "sortie,verif,  release : " << paramEnvDialog.r4 << std::endl;
    paramEnvForDialog = paramEnvDialog;
}

void AdsrDialog::setCoord(EnvelopeNode &node, float newX, float newY)
{
    node.center.setX(newX);
    node.center.setY(newY);
    node.rect.setLeft(newX);
    node.rect.setRight(newX + widthNodes * 2);
    node.rect.setTop(newY);
    node.rect.setBottom(newY + widthNodes * 2);
}

void AdsrDialog::updateAff()
{
    setCoord(node_l3,
             - xSustain,
             ui->doubleSpinBox_l_3->value() * (- heightMaxAff) + (heightMaxAff / 2));
    setCoord(node_l4,
             xSustain,
             ui->doubleSpinBox_l_4->value() * (- heightMaxAff) + (heightMaxAff / 2));
    setCoord(node_end,
             xSustain + ui->doubleSpinBox_r_4->value() * coeffAffMS,
             heightMaxAff / 2);
    setCoord(node_l2,
             - xSustain - ui->doubleSpinBox_r_3->value() * coeffAffMS,
             ui->doubleSpinBox_l_2->value() * (- heightMaxAff) + (heightMaxAff / 2));
    setCoord(node_l1,
             node_l2.center.x() - ui->doubleSpinBox_r_2->value() * coeffAffMS,
             ui->doubleSpinBox_l_1->value() * (- heightMaxAff) + (heightMaxAff / 2));
    setCoord(node_begin,
             node_l1.center.x() - ui->doubleSpinBox_r_1->value() * coeffAffMS,
             heightMaxAff / 2);
    ellipse_begin->setRect(node_begin.rect);
    ellipse_l1->setRect(node_l1.rect);
    ellipse_l2->setRect(node_l2.rect);
    ellipse_l3->setRect(node_l3.rect);
    ellipse_l4->setRect(node_l4.rect);
    ellipse_end->setRect(node_end.rect);
    line_r1->setLine(node_begin.center.x() + widthNodes,
                     node_begin.center.y() + widthNodes,
                     node_l1.center.x() + widthNodes,
                     node_l1.center.y() + widthNodes);
    line_r2->setLine(node_l1.center.x() + widthNodes,
                     node_l1.center.y() + widthNodes,
                     node_l2.center.x() + widthNodes,
                     node_l2.center.y() + widthNodes);
    line_r3->setLine(node_l2.center.x() + widthNodes,
                     node_l2.center.y() + widthNodes,
                     node_l3.center.x() + widthNodes,
                     node_l3.center.y() + widthNodes);
    line_r4->setLine(node_l4.center.x() + widthNodes,
                     node_l4.center.y() + widthNodes,
                     node_end.center.x() + widthNodes,
                     node_end.center.y() + widthNodes);
}

void AdsrDialog::on_doubleSpinBox_r_4_valueChanged(double arg1)
{
    tr4 = arg1;
    updateAff();
}

void AdsrDialog::on_doubleSpinBox_r_3_valueChanged(double arg1)
{
    tr3 = arg1;
    updateAff();
}

void AdsrDialog::on_doubleSpinBox_l_1_valueChanged(double arg1)
{
     paramEnvDialog.l1 = arg1;
     updateAff();
}

void AdsrDialog::on_doubleSpinBox_r_1_valueChanged(double arg1)
{
     tr1 = arg1;
     updateAff();
}

void AdsrDialog::on_doubleSpinBox_l_2_valueChanged(double arg1)
{
     paramEnvDialog.l2 = arg1;
     updateAff();
}

void AdsrDialog::on_doubleSpinBox_r_2_valueChanged(double arg1)
{
     tr2 = arg1;
     updateAff();
}

void AdsrDialog::on_doubleSpinBox_l_3_valueChanged(double arg1)
{
    ui->doubleSpinBox_l_4->setValue(arg1);
    paramEnvDialog.l3 = arg1;
    updateAff();
}

void AdsrDialog::on_doubleSpinBox_l_4_valueChanged(double arg1)
{
    ui->doubleSpinBox_l_3->setValue(arg1);
    paramEnvDialog.l3 = arg1;
    updateAff();
}

