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
#ifndef COMBIDIALOG_H
#define COMBIDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QTreeWidgetItem>
#include <QCloseEvent>
#include "model/MidiCombi.h"
#include "model/Scene.h"



namespace Ui {
class CombiDialog;
}

class CombiDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CombiDialog(QWidget *parent = 0);
    ~CombiDialog();
    void resetTreeCombi();
    void treeNotesAddChild(QTreeWidgetItem *parent, QString cldId, QString cldName, QString veloMin, QString veloMax);
    void addCloudToDialog(QString cldId, QString cldName);
    void initCombi(Scene *currentScene, unsigned numCombi);

private slots:
    void on_pushButton_AddCloud_clicked();

    void on_treeWidget_Clouds_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_RemoveCloud_clicked();

    void on_lineEdit_Name_textEdited(const QString &arg1);

private:
    void closeEvent(QCloseEvent *bar);
    Ui::CombiDialog *ui;
    Combination myCombi;
    QMap<QString, QString> cloudIdName;
    Scene *combiScene;
    int myNumCombi;

};

#endif // COMBIDIALOG_H
