#include "InstrumDialog.h"
#include "ui_InstrumDialog.h"

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

void InstrumentDialog::init(Scene *currentScene)
{
    instrumentScene = currentScene;
}
