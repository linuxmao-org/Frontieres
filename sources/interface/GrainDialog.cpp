#include "GrainDialog.h"
#include "ui_GrainDialog.h"

GrainDialog::GrainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GrainDialog)
{
    ui->setupUi(this);

    QTimer *tmAutoUpdate = new QTimer(this);
    connect(tmAutoUpdate, &QTimer::timeout, this, &GrainDialog::autoUpdate);
    tmAutoUpdate->start(500);

}

GrainDialog::~GrainDialog()
{
    delete ui;
}

void GrainDialog::linkGrains(VecGrain *grainSet)
{

}

void GrainDialog::autoUpdate()
{
    autoUpdating = true;
    if (cloudRef && cloudVisRef)
        linkCloud(cloudRef, cloudVisRef);
    autoUpdating = false;
}

void GrainDialog::on_spinBoxGrain_valueChanged(int arg1)
{

}
