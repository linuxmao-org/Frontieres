#include "CombiDialog.h"
#include "ui_CombiDialog.h"
#include <iostream>
#include "stdio.h"

CombiDialog::CombiDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CombiDialog)
{
    ui->setupUi(this);
    //resetTreeCombi();
    QStringList labelsTreeCombi = {"Num", "Name", "Velocity min", "velocity max"};
    QStringList labelsTreeCloud = {"Id", "Name"};
    ui->treeWidget_Combi->setHeaderLabels(labelsTreeCombi);
    ui->treeWidget_Combi->setColumnWidth(0, 70) ;
    ui->treeWidget_Combi->setColumnWidth(1, 70) ;
    ui->treeWidget_Clouds->setHeaderLabels(labelsTreeCloud);
    ui->treeWidget_Clouds->setColumnWidth(0, 50) ;
}

CombiDialog::~CombiDialog()
{
    delete ui;
}

void CombiDialog::resetTreeCombi()
{
    ui->treeWidget_Combi->clear();
    QString baseNameNote[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    for (int i=0; i <= 127; i++) {
        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->treeWidget_Combi);
        itm->setText(0, QString::number(i));
        itm->setText(1, (QString) (baseNameNote[i % 12] + QString::number(abs(i/12) - 1)));
        ui->treeWidget_Combi->addTopLevelItem(itm);
        Note l_note = myCombi.getNote(i);
        for (int k=0; k < l_note.cloudLayer.size(); k++){
            treeNotesAddChild(itm, QString::number(l_note.cloudLayer[k].cloudId),
                                   cloudIdName[QString::number(l_note.cloudLayer[k].cloudId)],
                                   QString::number(l_note.cloudLayer[k].velocity.min),
                                   QString::number(l_note.cloudLayer[k].velocity.max));
        }
    }
}

void CombiDialog::treeNotesAddChild(QTreeWidgetItem *parent, QString cldId, QString cldName, QString veloMin, QString veloMax)
{
    QTreeWidgetItem *itm = new QTreeWidgetItem();
    itm->setText(0, cldId);
    itm->setText(1, cldName);
    itm->setText(2, veloMin);
    itm->setText(3, veloMax);
    parent->addChild(itm);
}

void CombiDialog::addCloudToDialog(QString cldId, QString cldName)
{
    QTreeWidgetItem *itm = new QTreeWidgetItem(ui->treeWidget_Clouds);
    itm->setText(0, cldId);
    itm->setText(1, cldName);
    cloudIdName[cldId].append(cldName);
    ui->treeWidget_Clouds->addTopLevelItem(itm);
}

void CombiDialog::initCombi(Scene *currentScene, unsigned numCombi)
{
    combiScene = currentScene;
    myNumCombi = numCombi;
    myCombi = combiScene->m_midiBank.findCombi(numCombi);
    ui->lineEdit_Name->setText(myCombi.getName());
    resetTreeCombi();
}

void CombiDialog::on_pushButton_AddCloud_clicked()
{
    if (ui->label_cloudId->text().toInt() == 0)
        return;
    for (int i = (int) ui->spinBox_RangeBegin->value(); i <= ui->spinBox_RangeEnd->value(); i++){
        myCombi.addCloud(i, ui->label_cloudId->text().toInt(), ui->spinBox_VeloMin->value(), ui->spinBox_VeloMax->value());
    }
    resetTreeCombi();
}

void CombiDialog::on_treeWidget_Clouds_itemClicked(QTreeWidgetItem *item, int column)
{
    ui->label_cloudId->setText(item->text(0));
}

void CombiDialog::on_pushButton_RemoveCloud_clicked()
{
    for (int i = (int) ui->spinBox_RangeBegin->value(); i <= ui->spinBox_RangeEnd->value(); i++){
        myCombi.removeCloud(i, ui->label_cloudId->text().toInt());
    }
    resetTreeCombi();
}

void CombiDialog::closeEvent(QCloseEvent *bar)
{
    combiScene->m_midiBank.updateCombi(&myCombi, myNumCombi);
    bar->accept();
}

void CombiDialog::on_lineEdit_Name_textEdited(const QString &arg1)
{
    myCombi.setName(ui->lineEdit_Name->text());
}
