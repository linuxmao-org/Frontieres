#ifndef BANKDIALOG_H
#define BANKDIALOG_H

#include <Qt>
#include <QDialog>
#include <QMenu>
#include <QMessageBox>
#include <mutex>
#include "Frontieres.h"
#include "model/Scene.h"
#include "model/Cloud.h"
#include "CombiDialog.h"
#include "model/MidiBank.h"

enum { SELECT, EDIT };

namespace Ui {
class BankDialog;
}

class BankDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BankDialog(QWidget *parent = 0);
    ~BankDialog();
    void init(Scene *currentScene, int l_selectMode, int l_midiChannel);
    void resetCombiNames();

private slots:

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void copyCombi();

    void pasteCombi();

    void deleteCombi();

private:
    Ui::BankDialog *ui;
    CombiDialog *combiDialog = NULL;
    int selectedCombi = -1;
    int selectedCopyCombi = -1;
    int selectedMidiChannel;
    Scene *bankScene;
    int selectMode;
};

#endif // BANKDIALOG_H
