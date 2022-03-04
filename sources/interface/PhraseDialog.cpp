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
#include "PhraseDialog.h"
#include "ui_PhraseDialog.h"

PhraseDialog::PhraseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PhraseDialog)
{
    setModal(false);
    ui->setupUi(this);

    QPen blackPen (Qt::black);
    QPen graypen (Qt::gray);
    blackPen.setWidth(1);

    intervalGraphicScene = new QGraphicsScene(this);
    ui->graphicsView_Interval->setScene(intervalGraphicScene);
    QLine intervalVertLine;
    QLine intervalHorzLine;
    intervalVertLine.setLine(0,-ampControl,0,ampControl);
    intervalHorzLine.setLine(0,0,30000,0);
    intervalGraphicScene->addLine(intervalVertLine,graypen);
    intervalGraphicScene->addLine(intervalHorzLine,graypen);
    ui->graphicsView_Interval->centerOn(100, 0);

    connect(intervalGraphicScene, SIGNAL(changed(QList<QRectF>)),
            this, SLOT(updateIntervalFromGraph()));

    shadeGraphicScene = new QGraphicsScene(this);
    ui->graphicsView_Shade->setScene(shadeGraphicScene);
    QLine shadeVertLine;
    QLine shadeHorzLine;
    shadeVertLine.setLine(0,-ampControl,0,ampControl);
    shadeHorzLine.setLine(0,0,30000,0);
    shadeGraphicScene->addLine(shadeVertLine,graypen);
    shadeGraphicScene->addLine(shadeHorzLine,graypen);
    ui->graphicsView_Shade->centerOn(100, 0);

    connect(shadeGraphicScene, SIGNAL(changed(QList<QRectF>)),
            this, SLOT(updateShadeFromGraph()));
}

PhraseDialog::~PhraseDialog()
{
    int lastShade = myShadePositions.size();
    for (int i = 0;i < lastShade;i = i + 1) {
        delete myShadePositions.back();
        myShadePositions.pop_back();
    }
    int lastInterval = myIntervalPositions.size();
    for (int i = 0;i < lastInterval;i = i + 1) {
        delete myIntervalPositions.back();
        myIntervalPositions.pop_back();
    }
    delete ui;
}

void PhraseDialog::linkCloud(Cloud *cloudLinked)
{
    cloudRef = cloudLinked;
    //ui->doubleSpinBox_Tempo->setValue(phraseRef->getTempo());
    deleteIntervalPositions();
    deleteShadePositions();
    createIntervalPositions();
    createShadePositions();

}

void PhraseDialog::linkPhrase(Phrase *phraseLinked)
{
    phraseRef = phraseLinked;
    //ui->doubleSpinBox_Tempo->setValue(phraseRef->getTempo());
    deleteIntervalPositions();
    deleteShadePositions();
    createIntervalPositions();
    createShadePositions();
}

void PhraseDialog::init(Scene *currentScene)
{
    sceneRef = currentScene;
    ui->doubleSpinBox_Phrase_Number->setMaximum(sceneRef->getPhrasesSize());
}

void PhraseDialog::createShadePositions()
{
    //cout << "entre createshadeposition" << endl;
    QPen whitePen (Qt::white);
    whitePen.setWidth(1);

    int previousX = 0;
    int previousY = 0;

    //unsigned long l_size = cloudRef->getPhraseShadeSize();
    unsigned long l_size = phraseRef->getMyControlShadeSize();

    for (unsigned long i = 0; i < l_size; i = i + 1) {
        myShadePositions.push_back(new PhrasePosition());
        ControlPoint currentControlPoint = phraseRef->getControlShade(i);

        int currentX = int(currentControlPoint.delay * 100 * float(ui->doubleSpinBox_Horizontal->value()));
        int currentY = - int((currentControlPoint.value - 1) * ampControl/10 * float(ui->doubleSpinBox_Vertical->value()));

        myShadePositions[i]->myNode.setX(currentX);
        myShadePositions[i]->myNode.setY(currentY);
        myShadePositions[i]->myPointNode.setX(currentX);
        myShadePositions[i]->myPointNode.setY(currentY);

        if (i == ui->doubleSpinBox_Shade_Number->value())
            myShadePositions[i]->myNode.setActiveState(false);
        else
            myShadePositions[i]->myNode.setActiveState(true);

        myShadePositions[i]->myLine.setLine(currentX,currentY,previousX,previousY);
        myShadePositions[i]->myLine.setZValue(1);
        myShadePositions[i]->myNode.setZValue(2);
        shadeGraphicScene->addItem(&myShadePositions[i]->myLine);
        //cout << "2e repere additem" << endl;
        shadeGraphicScene->addItem(&myShadePositions[i]->myNode);

        myShadePositions[i]->myLimits.maxX.exist = true;
        myShadePositions[i]->myLimits.maxY.exist = true;
        myShadePositions[i]->myLimits.minX.exist = true;
        myShadePositions[i]->myLimits.minY.exist = true;

        previousX = currentX;
        previousY = currentY;

        NodeLimits l_nodeLimits;

        if (i == 0) {
            l_nodeLimits.minX.exist = true;
            l_nodeLimits.minX.value = 0;
            l_nodeLimits.maxX.exist = true;
            l_nodeLimits.maxX.value = 0;
            l_nodeLimits.minY.exist = true;
            l_nodeLimits.minY.value = 0;
            l_nodeLimits.maxY.exist = true;
            l_nodeLimits.maxY.value = 0;

            myShadePositions[i]->myNode.setLimits(l_nodeLimits);
        }
        else if (i == l_size - 1) {
            l_nodeLimits.minX.exist = true;
            l_nodeLimits.minX.value = myShadePositions[i-1]->myNode.x() + 0.001;
            l_nodeLimits.maxX.exist = false;
            l_nodeLimits.minY.exist = true;
            l_nodeLimits.minY.value = -ampControl;
            l_nodeLimits.maxY.exist = true;
            l_nodeLimits.maxY.value = ampControl;

            myShadePositions[i]->myNode.setLimits(l_nodeLimits);
        }
        else {
            l_nodeLimits.minX.exist = true;
            l_nodeLimits.minX.value = myShadePositions[i-1]->myNode.x() + 0.001;
            l_nodeLimits.maxX.exist = true;
            l_nodeLimits.maxX.value = phraseRef->getControlShade(i + 1).delay * 100 * ui->doubleSpinBox_Horizontal->value() - 0.001;
            l_nodeLimits.minY.exist = true;
            l_nodeLimits.minY.value = -ampControl;
            l_nodeLimits.maxY.exist = true;
            l_nodeLimits.maxY.value = ampControl;

            myShadePositions[i]->myNode.setLimits(l_nodeLimits);
        }
        if (i == ui->doubleSpinBox_Shade_Number->value())
            ui->graphicsView_Shade->centerOn(&myShadePositions[i]->myNode);
    }
    if (l_size == 1)
        ui->doubleSpinBox_Shade_Number->setDisabled(true);
    else {
        ui->doubleSpinBox_Shade_Number->setDisabled(false);
        ui->doubleSpinBox_Shade_Number->setMaximum(l_size - 1);
    }
    //cout << "sortie createshadeposition" << endl;
}

void PhraseDialog::deleteShadePositions()
{
    unsigned long lastShade = myShadePositions.size();
    for (unsigned long i = 0;i < lastShade;i = i + 1) {
        delete myShadePositions.back();
        myShadePositions.pop_back();
    }
}

void PhraseDialog::updateShadePosition(unsigned long l_numShade, int new_x, int new_y)
{

}

void PhraseDialog::createIntervalPositions()
{
    //cout << "entre createintervalposition" << endl;
    QPen whitePen (Qt::white);
    whitePen.setWidth(1);

    int previousX = 0;
    int previousY = 0;

    //unsigned long l_size = cloudRef->getPhraseIntervalSize();
    unsigned long l_size = phraseRef->getMyControlIntervalSize();

    for (unsigned long i = 0; i < l_size; i = i + 1) {
        myIntervalPositions.push_back(new PhrasePosition());
        ControlPoint currentControlPoint = phraseRef->getControlInterval(i);

        int currentX = int(currentControlPoint.delay * 100 * float(ui->doubleSpinBox_Horizontal->value()));
        int currentY = - int(currentControlPoint.value * float(ui->doubleSpinBox_Vertical->value()));

        myIntervalPositions[i]->myNode.setX(currentX);
        myIntervalPositions[i]->myNode.setY(currentY);
        myIntervalPositions[i]->myPointNode.setX(currentX);
        myIntervalPositions[i]->myPointNode.setY(currentY);

        if (int(i) == int(ui->doubleSpinBox_Interval_Number->value()))
            myIntervalPositions[i]->myNode.setActiveState(false);
        else
            myIntervalPositions[i]->myNode.setActiveState(true);

        myIntervalPositions[i]->myLine.setLine(currentX,currentY,previousX,previousY);
        myIntervalPositions[i]->myLine.setZValue(1);
        myIntervalPositions[i]->myNode.setZValue(2);

        if (i != 0) {
            if (!phraseRef->getControlInterval(i - 1).silence)
                intervalGraphicScene->addItem(&myIntervalPositions[i]->myLine);
        }
        else {
            intervalGraphicScene->addItem(&myIntervalPositions[i]->myLine);
        }
        //cout << "1e repere additem" << endl;
        intervalGraphicScene->addItem(&myIntervalPositions[i]->myNode);

        myIntervalPositions[i]->myLimits.maxX.exist = true;
        myIntervalPositions[i]->myLimits.maxY.exist = true;
        myIntervalPositions[i]->myLimits.minX.exist = true;
        myIntervalPositions[i]->myLimits.minY.exist = true;

        previousX = currentX;
        previousY = currentY;

        NodeLimits l_nodeLimits;

        if (i == 0) {
            l_nodeLimits.minX.exist = true;
            l_nodeLimits.minX.value = 0;
            l_nodeLimits.maxX.exist = true;
            l_nodeLimits.maxX.value = 0;
            l_nodeLimits.minY.exist = true;
            l_nodeLimits.minY.value = 0;
            l_nodeLimits.maxY.exist = true;
            l_nodeLimits.maxY.value = 0;

            myIntervalPositions[i]->myNode.setLimits(l_nodeLimits);
        }
        else if (i == l_size - 1) {
            l_nodeLimits.minX.exist = true;
            l_nodeLimits.minX.value = float(myIntervalPositions[i-1]->myNode.x() + 0.001);
            l_nodeLimits.maxX.exist = false;
            l_nodeLimits.minY.exist = true;
            l_nodeLimits.minY.value = -ampControl;
            l_nodeLimits.maxY.exist = true;
            l_nodeLimits.maxY.value = ampControl;

            myIntervalPositions[i]->myNode.setLimits(l_nodeLimits);
        }
        else {
            l_nodeLimits.minX.exist = true;
            l_nodeLimits.minX.value = float(myIntervalPositions[i-1]->myNode.x() + 0.001);
            l_nodeLimits.maxX.exist = true;
            l_nodeLimits.maxX.value = phraseRef->getControlInterval(i + 1).delay * 100 * ui->doubleSpinBox_Horizontal->value() - 0.001;
            l_nodeLimits.minY.exist = true;
            l_nodeLimits.minY.value = -ampControl;
            l_nodeLimits.maxY.exist = true;
            l_nodeLimits.maxY.value = ampControl;

            myIntervalPositions[i]->myNode.setLimits(l_nodeLimits);
        }
        if (int(i) == int(ui->doubleSpinBox_Interval_Number->value()))
            ui->graphicsView_Interval->centerOn(&myIntervalPositions[i]->myNode);
    }
    if (l_size == 1) {
        ui->doubleSpinBox_Interval_Number->setDisabled(true);
        ui->checkBox_Silence->setDisabled(true);
    }
    else {
        ui->doubleSpinBox_Interval_Number->setDisabled(false);
        ui->checkBox_Silence->setDisabled(false);
        ui->doubleSpinBox_Interval_Number->setMaximum(l_size - 1);
    }
    //cout << "sortie createintervalposition" << endl;
}

void PhraseDialog::deleteIntervalPositions()
{
    unsigned long lastInterval = myIntervalPositions.size();
    for (unsigned long i = 0;i < lastInterval;i = i + 1) {
        delete myIntervalPositions.back();
        myIntervalPositions.pop_back();
    }
}

void PhraseDialog::updateIntervalPosition(unsigned long l_numInterval, int new_x, int new_y)
{

}

void PhraseDialog::updateIntervalFromGraph()
{
    //cout << "entree updatefromgraph" << endl;
    for (unsigned long i = 1; i < myIntervalPositions.size(); i = i + 1) {
        //cout << "update, i =" << i << endl;
        if (myIntervalPositions[i]->myNode.moved()){
            //cout << "moved" << endl;
            phraseRef->setControlInterval(i, myIntervalPositions[i]->myNode.x() / 100 / ui->doubleSpinBox_Horizontal->value(),
                                                         -myIntervalPositions[i]->myNode.y() / ui->doubleSpinBox_Vertical->value());
            if (i != 0) {
               myIntervalPositions[i]->myLine.setLine(myIntervalPositions[i-1]->myNode.x(),
                                                      myIntervalPositions[i-1]->myNode.y(),
                                                      myIntervalPositions[i]->myNode.x(),
                                                      myIntervalPositions[i]->myNode.y());
            }
            if (i != myIntervalPositions.size() - 1) {
                myIntervalPositions[i+1]->myLine.setLine(myIntervalPositions[i]->myNode.x(),
                                                         myIntervalPositions[i]->myNode.y(),
                                                         myIntervalPositions[i+1]->myNode.x(),
                                                         myIntervalPositions[i+1]->myNode.y());
            }

            NodeLimits l_nodeLimits;

            if (i == myIntervalPositions.size() - 1) {
                l_nodeLimits.minX.exist = true;
                l_nodeLimits.minX.value = myIntervalPositions[i-1]->myNode.x() + 0.001;
                l_nodeLimits.maxX.exist = false;
                l_nodeLimits.minY.exist = true;
                l_nodeLimits.minY.value = -ampControl;
                l_nodeLimits.maxY.exist = true;
                l_nodeLimits.maxY.value = ampControl;

                myIntervalPositions[i]->myNode.setLimits(l_nodeLimits);

            }
            else {
                //cout << "i=" << i <<endl;
                l_nodeLimits.minX.exist = true;
                l_nodeLimits.minX.value = myIntervalPositions[i-1]->myNode.x() + 0.001;
                l_nodeLimits.maxX.exist = true;
                l_nodeLimits.maxX.value = phraseRef->getControlInterval(i + 1).delay * 100 * ui->doubleSpinBox_Horizontal->value() - 0.001;
                l_nodeLimits.minY.exist = true;
                l_nodeLimits.minY.value = -ampControl;
                l_nodeLimits.maxY.exist = true;
                l_nodeLimits.maxY.value = ampControl;

                myIntervalPositions[i]->myNode.setLimits(l_nodeLimits);
            }

            myIntervalPositions[i]->myNode.endMove();
            ui->doubleSpinBox_Interval_Number->setValue(i);
            ui->doubleSpinBox_Interval_Delay->setValue(phraseRef->getControlInterval(i).delay);
            ui->doubleSpinBox_Interval_Value->setValue(phraseRef->getControlInterval(i).value);
            break;
        }
    }
    //phraseRef->debugListControlInterval();
   // cout << "sortie updatefromgraph" << endl;
}

void PhraseDialog::updateShadeFromGraph()
{
    //cout << "entree updatefromgraph" << endl;
    for (unsigned long i = 1; i < myShadePositions.size(); i = i + 1) {
        //cout << "update, i =" << i << endl;
        if (myShadePositions[i]->myNode.moved()){
            //cout << "moved" << endl;
            phraseRef->setControlShade(i, myShadePositions[i]->myNode.x() / 100 / ui->doubleSpinBox_Horizontal->value(),
                                                      1 - ((10 * myShadePositions[i]->myNode.y()) / (ampControl * ui->doubleSpinBox_Vertical->value())));
            if (i != 0) {
               myShadePositions[i]->myLine.setLine(myShadePositions[i-1]->myNode.x(),
                                                   myShadePositions[i-1]->myNode.y(),
                                                   myShadePositions[i]->myNode.x(),
                                                   myShadePositions[i]->myNode.y());
            }
            if (i != myShadePositions.size() - 1) {
                myShadePositions[i+1]->myLine.setLine(myShadePositions[i]->myNode.x(),
                                                      myShadePositions[i]->myNode.y(),
                                                      myShadePositions[i+1]->myNode.x(),
                                                      myShadePositions[i+1]->myNode.y());
            }

            NodeLimits l_nodeLimits;

            if (i == myShadePositions.size() - 1) {
                l_nodeLimits.minX.exist = true;
                l_nodeLimits.minX.value = myShadePositions[i-1]->myNode.x() + 0.001;
                l_nodeLimits.maxX.exist = false;
                l_nodeLimits.minY.exist = true;
                l_nodeLimits.minY.value = -ampControl;
                l_nodeLimits.maxY.exist = true;
                l_nodeLimits.maxY.value = ampControl;

                myShadePositions[i]->myNode.setLimits(l_nodeLimits);

            }
            else {
                //cout << "i=" << i <<endl;
                l_nodeLimits.minX.exist = true;
                l_nodeLimits.minX.value = myShadePositions[i-1]->myNode.x() + 0.001;
                l_nodeLimits.maxX.exist = true;
                l_nodeLimits.maxX.value = phraseRef->getControlShade(i + 1).delay * 100 * ui->doubleSpinBox_Horizontal->value() - 0.001;
                l_nodeLimits.minY.exist = true;
                l_nodeLimits.minY.value = -ampControl;
                l_nodeLimits.maxY.exist = true;
                l_nodeLimits.maxY.value = ampControl;

                myShadePositions[i]->myNode.setLimits(l_nodeLimits);
            }

            myShadePositions[i]->myNode.endMove();
            ui->doubleSpinBox_Shade_Number->setValue(i);
            ui->doubleSpinBox_Shade_Delay->setValue(phraseRef->getControlShade(i).delay);
            ui->doubleSpinBox_Shade_Value->setValue(phraseRef->getControlShade(i).value);
            break;
        }
    }
    //phraseRef->debugListControlShade();
    //cout << "sortie update " << endl;
}

void PhraseDialog::on_pushButton_Interval_Add_pressed()
{
    int l_result = phraseRef->insertControlInterval(ui->doubleSpinBox_Interval_New_Delay->value(),ui->doubleSpinBox_Interval_New_Value->value(),false);
    ui->doubleSpinBox_Interval_Number->setMaximum(ui->doubleSpinBox_Interval_Number->maximum()+1);
    ui->doubleSpinBox_Interval_Number->setValue(l_result);
    ui->checkBox_Silence->setChecked(phraseRef->getControlInterval(l_result).silence);
    ui->doubleSpinBox_Interval_Delay->setValue(ui->doubleSpinBox_Interval_New_Delay->value());
    ui->doubleSpinBox_Interval_Value->setValue(ui->doubleSpinBox_Interval_New_Value->value());
    deleteIntervalPositions();
    createIntervalPositions();
    //phraseRef->debugListControlInterval();
}

void PhraseDialog::on_pushButton_Shade_add_pressed()
{
    int l_result = phraseRef->insertControlShade(ui->doubleSpinBox_Shade_New_Delay->value(),ui->doubleSpinBox_Shade_New_Value->value());
    ui->doubleSpinBox_Shade_Number->setMaximum(ui->doubleSpinBox_Shade_Number->maximum()+1);
    ui->doubleSpinBox_Shade_Number->setValue(l_result);
    ui->doubleSpinBox_Shade_Delay->setValue(ui->doubleSpinBox_Shade_New_Delay->value());
    ui->doubleSpinBox_Shade_Value->setValue(ui->doubleSpinBox_Shade_New_Value->value());
    deleteShadePositions();
    createShadePositions();
    //phraseRef->debugListControlShade();
}

void PhraseDialog::on_doubleSpinBox_Horizontal_editingFinished()
{
    deleteIntervalPositions();
    createIntervalPositions();
}

void PhraseDialog::on_doubleSpinBox_Vertical_editingFinished()
{
    deleteIntervalPositions();
    createIntervalPositions();
}

void PhraseDialog::on_checkBox_Active_toggled(bool checked)
{
    //phraseRef->setActiveState(checked);
}

void PhraseDialog::on_doubleSpinBox_Interval_Delay_editingFinished()
{
    unsigned long l_num = int(ui->doubleSpinBox_Interval_Number->value());
    phraseRef->setControlInterval(l_num,
                                              ui->doubleSpinBox_Interval_Delay->value(),
                                              ui->doubleSpinBox_Interval_Value->value());
    deleteIntervalPositions();
    createIntervalPositions();
}

void PhraseDialog::on_doubleSpinBox_Shade_Delay_editingFinished()
{
    unsigned long l_num = int(ui->doubleSpinBox_Shade_Number->value());
    phraseRef->setControlShade(l_num,
                                           ui->doubleSpinBox_Shade_Delay->value(),
                                           ui->doubleSpinBox_Shade_Value->value());
    deleteShadePositions();
    createShadePositions();
}

void PhraseDialog::on_doubleSpinBox_Interval_Value_editingFinished()
{
    unsigned long l_num = int(ui->doubleSpinBox_Interval_Number->value());
    phraseRef->setControlInterval(l_num,
                                              ui->doubleSpinBox_Interval_Delay->value(),
                                              ui->doubleSpinBox_Interval_Value->value());
    deleteIntervalPositions();
    createIntervalPositions();
}

void PhraseDialog::on_doubleSpinBox_Shade_Value_editingFinished()
{
    unsigned long l_num = int(ui->doubleSpinBox_Shade_Number->value());
    phraseRef->setControlShade(l_num,
                                           ui->doubleSpinBox_Shade_Delay->value(),
                                           ui->doubleSpinBox_Shade_Value->value());
    deleteShadePositions();
    createShadePositions();
}

void PhraseDialog::on_doubleSpinBox_Interval_Number_editingFinished()
{
    int l_num = int(ui->doubleSpinBox_Interval_Number->value());
    int l_size = int(phraseRef->getMyControlIntervalSize());
    if (l_num == l_size - 1) {
        ui->doubleSpinBox_Interval_Delay->setMinimum(phraseRef->getControlInterval(l_num - 1).delay + 0.001);
        ui->doubleSpinBox_Interval_Delay->setMaximum(1000000);
    }
    else {
        ui->doubleSpinBox_Interval_Delay->setMinimum(phraseRef->getControlInterval(l_num - 1).delay + 0.001);
        ui->doubleSpinBox_Interval_Delay->setMaximum(phraseRef->getControlInterval(l_num + 1).delay - 0.001);
    }
    ui->doubleSpinBox_Interval_Delay->setValue(phraseRef->getControlInterval(ui->doubleSpinBox_Interval_Number->value()).delay);
    ui->doubleSpinBox_Interval_Value->setValue(phraseRef->getControlInterval(ui->doubleSpinBox_Interval_Number->value()).value);
    ui->checkBox_Silence->setChecked(phraseRef->getControlInterval(l_num).silence);
    deleteIntervalPositions();
    createIntervalPositions();
}

void PhraseDialog::on_doubleSpinBox_Shade_Number_editingFinished()
{
    int l_num = int(ui->doubleSpinBox_Shade_Number->value());
    int l_size = phraseRef->getMyControlShadeSize();
    if (l_num == l_size - 1) {
        ui->doubleSpinBox_Shade_Delay->setMinimum(phraseRef->getControlShade(l_num - 1).delay + 0.001);
        ui->doubleSpinBox_Shade_Delay->setMaximum(1000000);
    }
    else {
        ui->doubleSpinBox_Shade_Delay->setMinimum(phraseRef->getControlShade(l_num - 1).delay + 0.001);
        ui->doubleSpinBox_Shade_Delay->setMaximum(phraseRef->getControlShade(l_num + 1).delay - 0.001);
    }
    ui->doubleSpinBox_Shade_Delay->setValue(phraseRef->getControlShade(ui->doubleSpinBox_Shade_Number->value()).delay);
    ui->doubleSpinBox_Shade_Value->setValue(phraseRef->getControlShade(ui->doubleSpinBox_Shade_Number->value()).value);
    deleteShadePositions();
    createShadePositions();
}

void PhraseDialog::on_doubleSpinBox_Tempo_editingFinished()
{
    //phraseRef->setTempo(int(ui->doubleSpinBox_Tempo->value()));
}

void PhraseDialog::on_pushButton_Interval_Delete_pressed()
{
    unsigned long l_num = int(ui->doubleSpinBox_Interval_Number->value());
    if ((l_num != 0) & (l_num < phraseRef->getMyControlIntervalSize())) {
        phraseRef->deleteControlInterval(l_num);
        deleteIntervalPositions();
        createIntervalPositions();
    }
}

void PhraseDialog::on_pushButton_Shade_Delete_pressed()
{
    unsigned long l_num = int(ui->doubleSpinBox_Shade_Number->value());
    if ((l_num != 0) & (l_num < phraseRef->getMyControlShadeSize())) {
        phraseRef->deleteControlShade(l_num);
        deleteShadePositions();
        createShadePositions();
    }
}

void PhraseDialog::on_checkBox_Silence_toggled(bool checked)
{
    int l_num =ui->doubleSpinBox_Interval_Number->value();
    phraseRef->setSilence(l_num, checked);
    deleteIntervalPositions();
    createIntervalPositions();
}

void PhraseDialog::on_pushButton_Save_pressed()
{
    std::string namePhraseFile = phraseRef->askNamePhrase(FileDirection::Save);
    if (namePhraseFile.empty())
        return;

    QFile phraseFile(QString::fromStdString(namePhraseFile));
    phraseRef->save(phraseFile);

}

void PhraseDialog::on_pushButton_Load_pressed()
{
    std::string namePhraseFile = phraseRef->askNamePhrase(FileDirection::Load);
    if (namePhraseFile.empty())
        return;

    QFile phraseFile(QString::fromStdString(namePhraseFile));
    phraseRef->load(phraseFile);

    deleteIntervalPositions();
    createIntervalPositions();
    deleteShadePositions();
    createShadePositions();
}

void PhraseDialog::on_pushButton_pressed()
{
        phraseRef->reset();

        deleteIntervalPositions();
        createIntervalPositions();
        deleteShadePositions();
        createShadePositions();
}


void PhraseDialog::on_pushButton_Envelope_clicked()
{
    ParamEnv localParamEnv;
    localParamEnv = cloudRef->getEnvelopeVolumeParam();
    cloudRef->getEnvelopeVolume().envDialogShow(localParamEnv);
    cloudRef->setEnvelopeVolumeParam(localParamEnv);
}


void PhraseDialog::on_doubleSpinBox_Interval_Shift_editingFinished()
{
    unsigned long l_num = int(ui->doubleSpinBox_Interval_Number->value());
    phraseRef->shiftControlInterval(l_num, float(ui->doubleSpinBox_Interval_Shift->value()));
    ui->doubleSpinBox_Interval_Shift->setValue(0);
    on_doubleSpinBox_Interval_Number_editingFinished();
}

void PhraseDialog::on_doubleSpinBox_Shade_Shift_editingFinished()
{
    unsigned long l_num = int(ui->doubleSpinBox_Shade_Number->value());
    phraseRef->shiftControlShade(l_num, float(ui->doubleSpinBox_Shade_Shift->value()));
    ui->doubleSpinBox_Shade_Shift->setValue(0);
    on_doubleSpinBox_Shade_Number_editingFinished();
}

void PhraseDialog::on_pushButton_Phrase_Delete_pressed()
{
    //sceneRef->delete...
}

void PhraseDialog::on_pushButton_Phrase_Add_pressed()
{
    sceneRef->addNewPhrase();
    ui->doubleSpinBox_Phrase_Number->setMaximum(ui->doubleSpinBox_Phrase_Number->maximum()+1);
}

void PhraseDialog::on_doubleSpinBox_Phrase_Number_editingFinished()
{
    int l_num = int(ui->doubleSpinBox_Phrase_Number->value());

    //cout << "phrase num =" << l_num << endl;

    linkPhrase(sceneRef->findPhraseById(int(ui->doubleSpinBox_Phrase_Number->value()))->phrase.get());
    ui->lineEdit_Name->setText(phraseRef->getName());

    deleteIntervalPositions();
    deleteShadePositions();
    createIntervalPositions();
    createShadePositions();
}

void PhraseDialog::on_lineEdit_Name_editingFinished()
{
    phraseRef->setName(ui->lineEdit_Name->text());
}
