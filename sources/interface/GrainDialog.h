#ifndef GRAINDIALOG_H
#define GRAINDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include "interface/Node.h"
#include "model/Cloud.h"
#include "visual/CloudVis.h"
#include "model/Grain.h"

typedef std::vector<std::unique_ptr<Grain>> VecGrain;

namespace Ui {
class GrainDialog;
}

class GrainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GrainDialog(QWidget *parent = nullptr);
    ~GrainDialog();
    void linkCloud (Cloud *cloudLinked, CloudVis *cloudVisLinked);

private slots:
    void autoUpdate();

    void on_spinBoxGrain_valueChanged(int arg1);

private:
    Ui::GrainDialog *ui;
    vector<Node *> myNodes;
    Cloud *cloudRef;
    CloudVis *cloudVisRef;
};

#endif // GRAINDIALOG_H
