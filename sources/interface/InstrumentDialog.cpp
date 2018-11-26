#include "InstrumentDialog.h"
#include "ui_InstrumentDialog.h"

InstrumentDialog::InstrumentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstrumentDialog)
{
    ui->setupUi(this);
}

InstrumentDialog::~InstrumentDialog()
{
    delete ui;
}
