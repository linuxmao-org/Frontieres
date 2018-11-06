#include "interface/AdsrDialog.h"
#include "ui_AdsrDialog.h"
#include <iostream>

ParamEnv paramEnvDialog;

AdsrDialog::AdsrDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdsrDialog)
{
    setModal(false);
    ui->setupUi(this);
}

AdsrDialog::~AdsrDialog()
{
    delete ui;
}

void AdsrDialog::modifEnvelope(ParamEnv &paramEnvForDialog)
{
    paramEnvDialog = paramEnvForDialog;
    ui->doubleSpinBox_l_1->setValue(paramEnvDialog.l1);
    ui->doubleSpinBox_r_1->setValue(paramEnvDialog.r1);
    ui->doubleSpinBox_l_2->setValue(paramEnvDialog.l2);
    ui->doubleSpinBox_r_2->setValue(paramEnvDialog.r2);
    ui->doubleSpinBox_l_3->setValue(paramEnvDialog.l3);
    ui->doubleSpinBox_r_3->setValue(paramEnvDialog.r3);
    ui->doubleSpinBox_r_4->setValue(paramEnvDialog.r4);
    setModal(true);
    this->exec();
    /*paramEnvForDialog.l1 = paramEnvDialog.l1;
    paramEnvForDialog.r1 = paramEnvDialog.r1;
    paramEnvForDialog.t1 = paramEnvDialog.t1;
    paramEnvForDialog.l2 = paramEnvDialog.l2;
    paramEnvForDialog.r2 = paramEnvDialog.t2;
    paramEnvForDialog.t2 = paramEnvDialog.t2;
    paramEnvForDialog.l3 = paramEnvDialog.l3;
    paramEnvForDialog.r3 = paramEnvDialog.r3;
    paramEnvForDialog.r4 = paramEnvDialog.r4;
    paramEnvForDialog.t4 = paramEnvDialog.t4;*/
    paramEnvForDialog = paramEnvDialog;
}

void AdsrDialog::on_doubleSpinBox_r_4_valueChanged(double arg1)
{
    paramEnvDialog.r4 = arg1;
}

void AdsrDialog::on_doubleSpinBox_r_3_valueChanged(double arg1)
{
    paramEnvDialog.r3 = arg1;
}

void AdsrDialog::on_doubleSpinBox_l_1_valueChanged(double arg1)
{
     paramEnvDialog.l1 = arg1;
}

void AdsrDialog::on_doubleSpinBox_r_1_valueChanged(double arg1)
{
     paramEnvDialog.r1 = arg1;
}

void AdsrDialog::on_doubleSpinBox_l_2_valueChanged(double arg1)
{
     paramEnvDialog.l2 = arg1;
}

void AdsrDialog::on_doubleSpinBox_r_2_valueChanged(double arg1)
{
     paramEnvDialog.r2 = arg1;
}

void AdsrDialog::on_doubleSpinBox_l_3_valueChanged(double arg1)
{
    ui->doubleSpinBox_l_4->setValue(arg1);
    paramEnvDialog.l3 = arg1;
}

void AdsrDialog::on_doubleSpinBox_l_4_valueChanged(double arg1)
{
    ui->doubleSpinBox_l_3->setValue(arg1);
    paramEnvDialog.l3 = arg1;
}
