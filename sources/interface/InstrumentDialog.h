#ifndef INSTRUMENTDIALOG_H
#define INSTRUMENTDIALOG_H

#include <QDialog>

namespace Ui {
class InstrumentDialog;
}

class InstrumentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstrumentDialog(QWidget *parent = 0);
    ~InstrumentDialog();

private:
    Ui::InstrumentDialog *ui;
};

#endif // INSTRUMENTDIALOG_H
