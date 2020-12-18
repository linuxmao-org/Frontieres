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
