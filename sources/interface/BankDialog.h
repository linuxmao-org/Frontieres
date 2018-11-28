#ifndef BANKDIALOG_H
#define BANKDIALOG_H

#include <Qt>
#include <QDialog>
#include "model/Scene.h"
#include "model/Cloud.h"
#include "CombiDialog.h"
#include "model/MidiBank.h"

namespace Ui {
class BankDialog;
}

class BankDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BankDialog(QWidget *parent = 0);
    ~BankDialog();
    void init(Scene *currentScene);
    void resetCombiNames();

private slots:

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::BankDialog *ui;
    CombiDialog *combiDialog = NULL;
    int selectedCombi = -1;
    Scene *bankScene;
};

#endif // BANKDIALOG_H
