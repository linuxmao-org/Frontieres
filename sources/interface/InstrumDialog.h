#ifndef INSTRUMENTDIALOG_H
#define INSTRUMENTDIALOG_H

#include <QDialog>
#include "model/Scene.h"
#include "BankDialog.h"

namespace Ui {
class InstrumentDialog;
}

class InstrumentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstrumentDialog(QWidget *parent = 0);
    ~InstrumentDialog();
    void init(Scene *currentScene);

private:
    Ui::InstrumentDialog *ui;
    Scene *instrumentScene;
};

#endif // INSTRUMENTDIALOG_H
