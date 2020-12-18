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
