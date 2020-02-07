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
