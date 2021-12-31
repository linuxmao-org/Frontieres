//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     https://github.com/linuxmao-org/Frontieres
//
// Copyright (C) 2020  Olivier Flatres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "Frontieres.h"
#include "OptionsDialog.h"
#include "ui_OptionsDialog.h"
#include "MyRtOsc.h"

extern int timeInputs;

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    MyRtOsc &osc = MyRtOsc::instance();
    std::string oscUrl = osc.getUrl();
    std::cout << "OSC address: " << oscUrl << "\n";
    ui->label_OSCServerLauncher->setText(QString::fromStdString(oscUrl));
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::on_lineEdit_OSCPort_textChanged(const QString &arg1)
{

}

void OptionsDialog::on_pushButton_OSCServerLauncher_clicked()
{
    MyRtOsc &osc = MyRtOsc::instance();

    // TODO make a configuration for this command
    QString program = "open-stage-control";
    QStringList arguments;
    arguments << "-s" << ("0.0.0.0:" + QString::number(osc.getPort()));

    if (!QProcess::startDetached(program, arguments))
        QMessageBox::warning(
            this, tr("Error"),
            tr("Cannot lanch the program: %1").arg(program) + "\n" +
            tr("Please ensure the software is installed and the path is correct."));
}

void OptionsDialog::on_pushButton_clicked()
{

    MyRtOsc &osc = MyRtOsc::instance();
    osc.close();
//    osc.setMessageHandler(&oscCallback, nullptr);
    if (!osc.open(ui->lineEdit_OSCPort->text().toStdString().c_str()) || !osc.start()) {
        std::cerr << "Error: failed to start OSC!\n";
        return;
    }

    std::string oscUrl = osc.getUrl();
    std::cout << "OSC address: " << oscUrl << "\n";
    ui->label_OSCServerLauncher->setText(QString::fromStdString(oscUrl));
    //GLwindow->setupOscUrl(QString::fromStdString(oscUrl));
}

void OptionsDialog::on_spinBox_valueChanged(int arg1)
{
    ::timeInputs = arg1;
}
