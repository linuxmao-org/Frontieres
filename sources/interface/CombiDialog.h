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
