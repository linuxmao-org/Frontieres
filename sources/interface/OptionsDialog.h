#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include <stdio.h>
#include <iostream>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

private slots:
    void on_lineEdit_OSCPort_textChanged(const QString &arg1);

    void on_pushButton_OSCServerLauncher_clicked();

    void on_pushButton_clicked();

    void on_spinBox_valueChanged(int arg1);

private:
    Ui::OptionsDialog *ui;
};

#endif // OPTIONSDIALOG_H
