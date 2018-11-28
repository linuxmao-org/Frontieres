#include "BankDialog.h"
#include "ui_BankDialog.h"
#include <iostream>
#include "stdio.h"

BankDialog::BankDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BankDialog)
{
    ui->setupUi(this);
    QStringList labelsTree = {"Num", "Name"};
    ui->treeWidget->setHeaderLabels(labelsTree);
}

BankDialog::~BankDialog()
{
    delete ui;
}

void BankDialog::init(Scene *currentScene)
{
    bankScene = currentScene;
    resetCombiNames();
}

void BankDialog::resetCombiNames()
{
    ui->treeWidget->clear();
    ui->treeWidget->setSortingEnabled(false);
    for (int i = 0; i <= 127; i++){
        QTreeWidgetItem *itm = new QTreeWidgetItem(ui->treeWidget);
        if (bankScene->m_midiBank.existCombi(i))
            itm->setText(1, bankScene->m_midiBank.findCombi(i).getName());
        else
            itm->setText(1, "<..>");
        QString l_num = QString::number(i);
        for (unsigned j = 0; j = (3 - l_num.length()); j++){
            l_num = "0" + l_num;
        }
        itm->setText(0, l_num);
        ui->treeWidget->addTopLevelItem(itm);
    }
    ui->treeWidget->setSortingEnabled(true);
    ui->treeWidget->sortItems(0, Qt::AscendingOrder);
}

void BankDialog::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    selectedCombi = item->text(0).toInt();
}

void BankDialog::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    selectedCombi = item->text(0).toInt();
    if (selectedCombi >= 0){
        if (! bankScene->m_midiBank.existCombi(selectedCombi))
            bankScene->m_midiBank.createCombi(selectedCombi);
        combiDialog = new CombiDialog;
        combiDialog->setWindowTitle(tr("Combination"));
        for (unsigned i = 0; i < bankScene->m_clouds.size(); i++) {
            Cloud *l_cloud = bankScene->m_clouds[i]->cloud.get();
            combiDialog->addCloudToDialog(QString::number(l_cloud->getId()), l_cloud->getName());
        }
            combiDialog->initCombi(bankScene, selectedCombi);
        combiDialog->setModal(true);
        combiDialog->exec();
        resetCombiNames();
    }
}
