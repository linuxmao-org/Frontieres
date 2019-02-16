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
    void resetCombiNamesMidiChannels();

private slots:
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::InstrumentDialog *ui;
    BankDialog *bankDialog;
    Scene *instrumScene;
};

#endif // INSTRUMENTDIALOG_H
