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
#include "InstrumDialog.h"
#include "ui_InstrumDialog.h"

InstrumentDialog::InstrumentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstrumentDialog)
{
    ui->setupUi(this);
}

InstrumentDialog::~InstrumentDialog()
{
    delete ui;
}

void InstrumentDialog::init(Scene *currentScene)
{
    instrumScene = currentScene;
    resetCombiNamesMidiChannels();
}

void InstrumentDialog::resetCombiNamesMidiChannels()
{
    ui->treeWidget->clear();
    for (int i = 1; i <= 16; i++){
        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->treeWidget);
        int l_numCombiChannel = instrumScene->m_midiInstrument.getMidiCombi(i);
        if (l_numCombiChannel != -1) {
            itm->setText(1, QString::number(l_numCombiChannel));
            itm->setText(2, instrumScene->m_midiBank.findCombi(l_numCombiChannel).getName());
        }
        else {
            itm->setText(1, "<..>");
            itm->setText(2, "<..>");
        }
        QString l_num = QString::number(i);
        for (int j = 0; j = (2 - l_num.length()); j++){
            l_num = "0" + l_num;
        }
        itm->setText(0, l_num);
        ui->treeWidget->addTopLevelItem(itm);
    }
    ui->treeWidget->sortItems(0, Qt::AscendingOrder);
}

void InstrumentDialog::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    bankDialog = new BankDialog;
    bankDialog->setWindowTitle(tr("Combinations"));
    bankDialog->init(instrumScene, SELECT, item->text(0).toInt());
    bankDialog->setModal(true);
    bankDialog->exec();
    resetCombiNamesMidiChannels();
}
