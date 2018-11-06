#ifndef ADSRDIALOG_H
#define ADSRDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include "model/Adsr.h"

namespace Ui {
class AdsrDialog;
}

class AdsrDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdsrDialog(QWidget *parent = 0);
    ~AdsrDialog();
    void modifEnvelope(ParamEnv &paramEnvForDialog);

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
    QGraphicsScene *scene;

};

#endif // ADSRDIALOG_H
