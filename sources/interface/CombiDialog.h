#ifndef COMBIDIALOG_H
#define COMBIDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QTreeWidgetItem>
#include "model/Combination.h"



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

private slots:
    void on_pushButton_AddCloud_clicked();

    void on_treeWidget_Clouds_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_RemoveCloud_clicked();

private:
    Ui::CombiDialog *ui;
    Combination myCombi;
    QMap<QString, QString> cloudIdName;

};

#endif // COMBIDIALOG_H
