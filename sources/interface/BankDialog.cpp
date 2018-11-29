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

void BankDialog::init(Scene *currentScene, int l_selectMode, int l_midiChannel)
{
    bankScene = currentScene;
    resetCombiNames();
    selectMode = l_selectMode;
    selectedMidiChannel = l_midiChannel;
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
    switch (selectMode){
    case EDIT: {
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
        break;
    }
    case SELECT:{
        selectedCombi = item->text(0).toInt();
        bankScene->m_midiInstrument.setMidiCombi(selectedMidiChannel, selectedCombi);
        this->close();
    }
    default:
        break;
    }

}

void BankDialog::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    if (selectMode == EDIT){
        QMenu contextMenu(this);
        QIcon icon;
        QAction * pAction_copy = contextMenu.addAction(icon, "Copy");
        QAction * pAction_paste = contextMenu.addAction(icon, "Paste");
        QAction * pAction_delete = contextMenu.addAction(icon, "Delete");
        connect(pAction_copy, SIGNAL(triggered()), this, SLOT(copyCombi()));
        connect(pAction_paste, SIGNAL(triggered()), this, SLOT(pasteCombi()));
        connect(pAction_delete, SIGNAL(triggered()), this, SLOT(deleteCombi()));
        contextMenu.exec(QCursor::pos());
    }
}

void BankDialog::copyCombi()
{
    selectedCopyCombi = selectedCombi;
}

void BankDialog::pasteCombi()
{
    if ((selectedCopyCombi == -1) || (! bankScene->m_midiBank.existCombi(selectedCopyCombi)))
        return;
    bool l_pasteUpdate = false;
    if (selectedCombi >= 0){
        if (bankScene->m_midiBank.existCombi(selectedCombi)){
            int l_accept = QMessageBox::question(this, "Paste",
                                                 "This will override combination "
                                                 + bankScene->m_midiBank.findCombi(selectedCombi).getName()
                                                 + " , are you sure ?",
                                                 QMessageBox ::Yes | QMessageBox::No);
            if (l_accept == QMessageBox::Yes)
                l_pasteUpdate = true;
        }
        else{
            bankScene->m_midiBank.createCombi(selectedCombi);
            l_pasteUpdate = true;
        }
        if (l_pasteUpdate)
            bankScene->m_midiBank.updateCombi(bankScene->m_midiBank.findCombi(selectedCopyCombi), selectedCombi);
        resetCombiNames();
    }
}

void BankDialog::deleteCombi()
{
    if ((! bankScene->m_midiBank.existCombi(selectedCombi)) || (selectedCombi == -1))
        return;
    int l_accept = QMessageBox::question(this, "Delete",
                                         "This will delete combination "
                                         + bankScene->m_midiBank.findCombi(selectedCombi).getName()
                                         + " , are you sure ?",
                                         QMessageBox ::Yes | QMessageBox::No);
    if (l_accept == QMessageBox::Yes)
       bankScene->m_midiBank.deleteCombi(selectedCombi);
    resetCombiNames();
}
