#include "interface/CloudDialog.h"
#include "ui_CloudDialog.h"
#include "model/ParamCloud.h"
#include <iostream>
#include <stdio.h>

extern ValueMin g_cloudValueMin;
extern ValueMax g_cloudValueMax;
extern CloudParams g_defaultCloudParams;

CloudDialog::CloudDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CloudDialog)
{
    setModal(false);
    ui->setupUi(this);

    ui->doubleSpinBox_Volume->setMinimum(g_cloudValueMin.volumeDB);
    ui->doubleSpinBox_Volume->setMaximum(g_cloudValueMax.volumeDB);
    ui->verticalSlider_Volume->setMinimum(g_cloudValueMin.volumeDB * 1000);
    ui->verticalSlider_Volume->setMaximum(g_cloudValueMax.volumeDB * 1000);

    ui->doubleSpinBox_Grains->setMinimum(g_cloudValueMin.numGrains);
    ui->doubleSpinBox_Grains->setMaximum(g_cloudValueMax.numGrains);
    ui->dial_Grains->setMinimum(g_cloudValueMin.numGrains);
    ui->dial_Grains->setMaximum(g_cloudValueMax.numGrains);

    ui->doubleSpinBox_Duration->setMinimum(g_cloudValueMin.duration);
    ui->doubleSpinBox_Duration->setMaximum(g_cloudValueMax.duration);
    ui->dial_Duration->setMinimum(g_cloudValueMin.duration);
    ui->dial_Duration->setMaximum(g_cloudValueMax.duration);

    ui->doubleSpinBox_Overlap->setMinimum(g_cloudValueMin.overlap);
    ui->doubleSpinBox_Overlap->setMaximum(g_cloudValueMax.overlap);
    ui->dial_Overlap->setMaximum(g_cloudValueMax.overlap * 100);
    ui->dial_Overlap->setMinimum(g_cloudValueMin.overlap * 100);

    ui->doubleSpinBox_Pitch->setMinimum(g_cloudValueMin.pitch);
    ui->doubleSpinBox_Pitch->setMaximum(g_cloudValueMax.pitch);
    ui->dial_Pitch->setMaximum(g_cloudValueMax.pitch * 100);
    ui->dial_Pitch->setMinimum(g_cloudValueMin.pitch * 100);

    ui->doubleSpinBox_LFO_Freq->setMinimum(g_cloudValueMin.pitchLFOFreq);
    ui->doubleSpinBox_LFO_Freq->setMaximum(g_cloudValueMax.pitchLFOFreq);
    ui->dial_LFO_Freq->setMaximum(g_cloudValueMax.pitchLFOFreq * 100);
    ui->dial_LFO_Freq->setMinimum(g_cloudValueMin.pitchLFOFreq * 100);

    ui->doubleSpinBox_LFO_Amp->setMinimum(g_cloudValueMin.pitchLFOAmount);
    ui->doubleSpinBox_LFO_Amp->setMaximum(g_cloudValueMax.pitchLFOAmount);
    ui->dial_LFO_Amp->setMaximum(g_cloudValueMax.pitchLFOAmount * 1000);
    ui->dial_LFO_Amp->setMinimum(g_cloudValueMin.pitchLFOAmount * 1000);

    ui->doubleSpinBox_X->setMinimum(g_cloudValueMin.x);
    ui->doubleSpinBox_X->setMaximum(g_cloudValueMax.x);
    ui->dial_X->setMaximum(g_cloudValueMax.x);
    ui->dial_X->setMinimum(g_cloudValueMin.x);

    ui->doubleSpinBox_Y->setMinimum(g_cloudValueMin.y);
    ui->doubleSpinBox_Y->setMaximum(g_cloudValueMax.y);
    ui->dial_Y->setMaximum(g_cloudValueMax.y);
    ui->dial_Y->setMinimum(g_cloudValueMin.y);

    ui->doubleSpinBox_X_Extent->setMinimum(g_cloudValueMin.xRandExtent);
    ui->doubleSpinBox_X_Extent->setMaximum(g_cloudValueMax.xRandExtent);
    ui->dial_X_Extent->setMaximum(g_cloudValueMax.xRandExtent);
    ui->dial_X_Extent->setMinimum(g_cloudValueMin.xRandExtent);

    ui->doubleSpinBox_Y_Extent->setMinimum(g_cloudValueMin.yRandExtent);
    ui->doubleSpinBox_Y_Extent->setMaximum(g_cloudValueMax.yRandExtent);
    ui->dial_Y_Extent->setMaximum(g_cloudValueMax.yRandExtent);
    ui->dial_X_Extent->setMinimum(g_cloudValueMin.yRandExtent);

    ui->doubleSpinBox_Angle->setMinimum(g_cloudValueMin.angle);
    ui->doubleSpinBox_Angle->setMaximum(g_cloudValueMax.angle);
    ui->dial_Angle->setMaximum(g_cloudValueMax.angle);
    ui->dial_Angle->setMinimum(g_cloudValueMin.angle);

    ui->doubleSpinBox_Stretch->setMinimum(g_cloudValueMin.stretch);
    ui->doubleSpinBox_Stretch->setMaximum(g_cloudValueMax.stretch);
    ui->dial_Stretch->setMaximum(g_cloudValueMax.stretch * 1000);
    ui->dial_Stretch->setMinimum(g_cloudValueMin.stretch * 1000);

    ui->doubleSpinBox_Speed->setMinimum(g_cloudValueMin.speed);
    ui->doubleSpinBox_Speed->setMaximum(g_cloudValueMax.speed);
    ui->dial_Speed->setMaximum(g_cloudValueMax.speed);
    ui->dial_Speed->setMinimum(g_cloudValueMin.speed);

    ui->doubleSpinBox_Radius->setMinimum(g_cloudValueMin.radius);
    ui->doubleSpinBox_Radius->setMaximum(g_cloudValueMax.radius);
    ui->dial_Radius->setMaximum(g_cloudValueMax.radius);
    ui->dial_Radius->setMinimum(g_cloudValueMin.radius);

    ui->doubleSpinBox_RadiusInt->setMinimum(g_cloudValueMin.radiusInt);
    ui->doubleSpinBox_RadiusInt->setMaximum(g_cloudValueMax.radiusInt);
    ui->dial_RadiusInt->setMaximum(g_cloudValueMax.radiusInt);
    ui->dial_RadiusInt->setMinimum(g_cloudValueMin.radiusInt);

    ui->doubleSpinBox_Expansion->setMinimum(g_cloudValueMin.expansion);
    ui->doubleSpinBox_Expansion->setMaximum(g_cloudValueMax.expansion);
    ui->dial_Expansion->setMaximum(g_cloudValueMax.expansion);
    ui->dial_Expansion->setMinimum(g_cloudValueMin.expansion);

    ui->doubleSpinBox_Progress->setMinimum(g_cloudValueMin.progress);
    ui->doubleSpinBox_Progress->setMaximum(g_cloudValueMax.progress);
    ui->dial_Progress->setMaximum(g_cloudValueMax.progress);
    ui->dial_Progress->setMinimum(g_cloudValueMin.progress);

    ui->doubleSpinBox_Output_First->setMinimum(0);
    ui->doubleSpinBox_Output_Last->setMinimum(0);
    ui->doubleSpinBox_Output_First->setMaximum(theOutChannelCount - 1);
    ui->doubleSpinBox_Output_Last->setMaximum(theOutChannelCount - 1);

    grainsGraphicScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(grainsGraphicScene);

    setMouseTracking(true);

    QPen blackPen (Qt::black);
    blackPen.setWidth(1);

    QRectF zoneGrains;
    zoneGrains.setX(-100);
    zoneGrains.setY(-100);
    zoneGrains.setWidth(200);
    zoneGrains.setHeight(200);
    QBrush *whiteBrush = new QBrush(Qt::white);

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::white, Qt::SolidPattern));

    grainsGraphicScene->addEllipse (zoneGrains, blackPen, *whiteBrush);

    ui->graphicsView->hide();

    connect(grainsGraphicScene, SIGNAL(changed(QList<QRectF>)),
            this, SLOT(updateFromGraph()));

    QTimer *tmAutoUpdate = new QTimer(this);
    connect(tmAutoUpdate, &QTimer::timeout, this, &CloudDialog::autoUpdate);
    tmAutoUpdate->start(500);
}

CloudDialog::~CloudDialog()
{
    int lastGrain = myGrainPositions.size();
    for (int i = 0;i < lastGrain;i = i + 1) {
        delete myGrainPositions.back();
        myGrainPositions.pop_back();
    }
    delete ui;
}

void CloudDialog::autoUpdate()
{
    autoUpdating = true;
    if (cloudRef && cloudVisRef)
        linkCloud(cloudRef, cloudVisRef);
    autoUpdating = false;
}

void CloudDialog::linkCloud(Cloud *cloudLinked, CloudVis *cloudVisLinked)
{
    linking = true;
    cloudRef = cloudLinked;
    cloudVisRef = cloudVisLinked;
    if (!autoUpdating)
        if (cloudRef->getGrainsRandom())
            ui->radioButton_PositionRandom->setChecked(true);
        else
            ui->radioButton_PositionManual->setChecked(true);
    if (cloudLinked->changedDurationMs())
        ui->doubleSpinBox_Duration->setValue(cloudLinked->getDurationMs());
    if (cloudLinked->changedNumGrains())
        ui->doubleSpinBox_Grains ->setValue(cloudLinked->getNumGrains());
    if (cloudLinked->changedPitchLFOAmount())
        ui->doubleSpinBox_LFO_Amp->setValue(cloudLinked->getPitchLFOAmount());
    if (cloudLinked->changedPitchLFOFreq())
        ui->doubleSpinBox_LFO_Freq->setValue(cloudLinked->getPitchLFOFreq());
    if (cloudLinked->changedOverlap()) {
        ui->doubleSpinBox_Overlap->setValue(cloudLinked->getOverlap());
    }
    if (cloudLinked->changedVolumeDb())
        ui->doubleSpinBox_Volume->setValue(cloudLinked->getVolumeDb());
    if (cloudLinked->changedPitch())
        ui->doubleSpinBox_Pitch->setValue(cloudLinked->getPitch());
    if (cloudVisLinked->changedXRandExtent())
        ui->doubleSpinBox_X_Extent->setValue(cloudVisLinked->getXRandExtent());
    if (cloudVisLinked->changedYRandExtent())
        ui->doubleSpinBox_Y_Extent->setValue(cloudVisLinked->getYRandExtent());
    if (cloudVisLinked->changedGcX())
        ui->doubleSpinBox_X->setValue(cloudVisLinked->getOriginX());
    if (cloudVisLinked->changedGcY())
        ui->doubleSpinBox_Y->setValue(cloudVisLinked->getOriginY());
    if (cloudLinked->changedMidiNote())
        ui->doubleSpinBox_Midi_Note->setValue(cloudLinked->getMidiNote());
    ui->label_Id_Value->setText(QString::number(cloudLinked->getId()));
  //  if (cloudLinked->changedName())
    if (autoUpdating == false)
        ui->lineEdit_Name->setText(cloudLinked->getName());
    ui->checkBox_Active->setChecked(cloudLinked->getActiveState());
    ui->checkBox_Locked->setChecked(cloudLinked->getLockedState());
    ui->checkBox_Restart->setChecked(cloudLinked->getActiveRestartTrajectory());
    if (cloudLinked->changedDirection())
        switch (cloudLinked->getDirection()) {
        case FORWARD:
            ui->radioButton_Direction_Forward->setChecked(true);
            break;
        case BACKWARD:
            ui->radioButton_Direction_Backward->setChecked(true);
            break;
        case RANDOM_DIR:
            ui->radioButton_Direction_Random->setChecked(true);
            break;
        default :
            break;
        }
    if (cloudLinked->changedSpatialMode()) {
        switch (cloudLinked->getSpatialMode()) {
        case UNITY:
            ui->radioButton_Balance_Unity->setChecked(true);
            break;
        case STEREO:
            ui->radioButton_Balance_Stereo->setChecked(true);
            break;
        case AROUND:
            ui->radioButton_Balance_Around->setChecked(true);
            break;
        default :
            break;
        }
        ui->doubleSpinBox_Output_First->setValue(cloudLinked->getOutputFirst());
        ui->doubleSpinBox_Output_Last->setValue(cloudLinked->getOutputLast());
    }
    if (cloudLinked->changedWindowType())
        switch (cloudLinked->getWindowType()) {
        case HANNING:
            ui->radioButton_Window_Hanning->setChecked(true);
            break;
        case TRIANGLE:
            ui->radioButton_Window_Triangle->setChecked(true);
            break;
        case EXPDEC:
            ui->radioButton_Window_Expdec->setChecked(true);
            break;
        case REXPDEC:
            ui->radioButton_Window_Rexpdec->setChecked(true);
            break;
        case SINC:
            ui->radioButton_Window_Sinc->setChecked(true);
            break;
        case RANDOM_WIN:
            ui->radioButton_Window_Random->setChecked(true);
            break;
        default :
            break;
        }
    if (cloudLinked->changedTrajectoryType())
        switch (cloudLinked->getTrajectoryType()) {
        case STATIC:
            ui->radioButton_Trajectory_Static->setChecked(true);
            ui->doubleSpinBox_Speed->setDisabled(true);
            ui->doubleSpinBox_Radius->setDisabled(true);
            ui->doubleSpinBox_RadiusInt->setDisabled(true);
            ui->doubleSpinBox_Angle->setDisabled(true);
            ui->doubleSpinBox_Stretch->setDisabled(true);
            ui->doubleSpinBox_Expansion->setDisabled(true);
            ui->doubleSpinBox_Progress->setDisabled(true);
            ui->dial_Speed->setDisabled(true);
            ui->dial_Radius->setDisabled(true);
            ui->dial_RadiusInt->setDisabled(true);
            ui->dial_Angle->setDisabled(true);
            ui->dial_Stretch->setDisabled(true);
            ui->dial_Expansion->setDisabled(true);
            ui->dial_Progress->setDisabled(true);
            break;
        case BOUNCING:
        {
            ui->radioButton_Trajectory_Bouncing->setChecked(true);
            ui->doubleSpinBox_Speed->setDisabled(false);
            ui->doubleSpinBox_Radius->setDisabled(false);
            ui->doubleSpinBox_RadiusInt->setDisabled(true);
            ui->doubleSpinBox_Angle->setDisabled(false);
            ui->doubleSpinBox_Stretch->setDisabled(true);
            ui->doubleSpinBox_Expansion->setDisabled(true);
            ui->doubleSpinBox_Progress->setDisabled(true);
            ui->dial_Speed->setDisabled(false);
            ui->dial_Radius->setDisabled(false);
            ui->dial_RadiusInt->setDisabled(true);
            ui->dial_Angle->setDisabled(false);
            ui->dial_Stretch->setDisabled(true);
            ui->dial_Expansion->setDisabled(true);
            ui->dial_Progress->setDisabled(true);
            ui->doubleSpinBox_Speed->setValue(cloudVisLinked->getTrajectory()->getSpeed());
            ui->doubleSpinBox_Radius->setValue(cloudVisLinked->getTrajectory()->getRadius());
            ui->doubleSpinBox_Angle->setValue(cloudVisLinked->getTrajectory()->getAngle());
            have_trajectory_bouncing = true;
            break;
        }
        case CIRCULAR:
        {
            ui->radioButton_Trajectory_Circular->setChecked(true);
            ui->doubleSpinBox_Speed->setDisabled(false);
            ui->doubleSpinBox_Radius->setDisabled(false);
            ui->doubleSpinBox_RadiusInt->setDisabled(true);
            ui->doubleSpinBox_Angle->setDisabled(false);
            ui->doubleSpinBox_Stretch->setDisabled(false);
            ui->doubleSpinBox_Expansion->setDisabled(true);
            ui->doubleSpinBox_Progress->setDisabled(false);
            ui->dial_Speed->setDisabled(false);
            ui->dial_Radius->setDisabled(false);
            ui->dial_RadiusInt->setDisabled(true);
            ui->dial_Angle->setDisabled(false);
            ui->dial_Stretch->setDisabled(false);
            ui->dial_Expansion->setDisabled(true);
            ui->dial_Progress->setDisabled(false);
            ui->doubleSpinBox_Speed->setValue(cloudVisLinked->getTrajectory()->getSpeed());
            ui->doubleSpinBox_Radius->setValue(cloudVisLinked->getTrajectory()->getRadius());
            ui->doubleSpinBox_Angle->setValue(cloudVisLinked->getTrajectory()->getAngle());
            ui->doubleSpinBox_Stretch->setValue(cloudVisLinked->getTrajectory()->getStretch());
            ui->doubleSpinBox_Progress->setValue(cloudVisLinked->getTrajectory()->getProgress());
            have_trajectory_circular = true;
            break;
        }
        case HYPOTROCHOID:
        {
            ui->radioButton_Trajectory_Hypotrochoid->setChecked(true);
            ui->doubleSpinBox_Speed->setDisabled(false);
            ui->doubleSpinBox_Radius->setDisabled(false);
            ui->doubleSpinBox_RadiusInt->setDisabled(false);
            ui->doubleSpinBox_Angle->setDisabled(false);
            ui->doubleSpinBox_Stretch->setDisabled(true);
            ui->doubleSpinBox_Expansion->setDisabled(false);
            ui->doubleSpinBox_Progress->setDisabled(false);
            ui->dial_Speed->setDisabled(false);
            ui->dial_Radius->setDisabled(false);
            ui->dial_RadiusInt->setDisabled(false);
            ui->dial_Angle->setDisabled(false);
            ui->dial_Stretch->setDisabled(true);
            ui->dial_Expansion->setDisabled(false);
            ui->dial_Progress->setDisabled(false);
            ui->doubleSpinBox_Speed->setValue(cloudVisLinked->getTrajectory()->getSpeed());
            ui->doubleSpinBox_Radius->setValue(cloudVisLinked->getTrajectory()->getRadius());
            ui->doubleSpinBox_RadiusInt->setValue(cloudVisLinked->getTrajectory()->getRadiusInt());
            ui->doubleSpinBox_Angle->setValue(cloudVisLinked->getTrajectory()->getAngle());
            ui->doubleSpinBox_Expansion->setValue(cloudVisLinked->getTrajectory()->getExpansion());
            ui->doubleSpinBox_Progress->setValue(cloudVisLinked->getTrajectory()->getProgress());
            have_trajectory_hypotrochoid = true;
            break;
        }
        case RECORDED:
        {
            ui->radioButton_Trajectory_Recorded->setChecked(true);
            ui->doubleSpinBox_Speed->setDisabled(true);
            ui->doubleSpinBox_Radius->setDisabled(true);
            ui->doubleSpinBox_RadiusInt->setDisabled(true);
            ui->doubleSpinBox_Angle->setDisabled(true);
            ui->doubleSpinBox_Stretch->setDisabled(true);
            ui->doubleSpinBox_Expansion->setDisabled(true);
            ui->doubleSpinBox_Progress->setDisabled(true);
            ui->dial_Speed->setDisabled(true);
            ui->dial_Radius->setDisabled(true);
            ui->dial_RadiusInt->setDisabled(true);
            ui->dial_Angle->setDisabled(true);
            ui->dial_Stretch->setDisabled(true);
            ui->dial_Expansion->setDisabled(true);
            ui->dial_Progress->setDisabled(true);
            have_trajectory_recorded = true;
        }
        default :
            break;
        }

    cloudRef->changesDone(false);
    linking = false;
}

void CloudDialog::setDisableAllWidgets(bool disabled)
{
    ui->lineEdit_Name->setDisabled(disabled);
    ui->checkBox_Active->setDisabled(disabled);
    ui->dial_Duration->setDisabled(disabled);
    ui->dial_Grains->setDisabled(disabled);
    ui->dial_LFO_Amp->setDisabled(disabled);
    ui->dial_LFO_Freq->setDisabled(disabled);
    ui->dial_Overlap->setDisabled(disabled);
    ui->dial_Pitch->setDisabled(disabled);
    ui->dial_X->setDisabled(disabled);
    ui->dial_X_Extent->setDisabled(disabled);
    ui->dial_Y->setDisabled(disabled);
    ui->dial_Y_Extent->setDisabled(disabled);
    ui->doubleSpinBox_Duration->setDisabled(disabled);
    ui->doubleSpinBox_Grains->setDisabled(disabled);
    ui->doubleSpinBox_LFO_Amp->setDisabled(disabled);
    ui->doubleSpinBox_LFO_Freq->setDisabled(disabled);
    ui->doubleSpinBox_Midi_Note->setDisabled(disabled);
    ui->doubleSpinBox_Overlap->setDisabled(disabled);
    ui->doubleSpinBox_Pitch->setDisabled(disabled);
    ui->doubleSpinBox_Volume->setDisabled(disabled);
    ui->doubleSpinBox_X->setDisabled(disabled);
    ui->doubleSpinBox_X_Extent->setDisabled(disabled);
    ui->doubleSpinBox_Y->setDisabled(disabled);
    ui->doubleSpinBox_Y_Extent->setDisabled(disabled);
    ui->groupBox_Balance->setDisabled(disabled);
    ui->doubleSpinBox_Output_First->setDisabled(disabled);
    ui->doubleSpinBox_Output_Last->setDisabled(disabled);
    ui->groupBox_Direction->setDisabled(disabled);
    ui->groupBox_Window->setDisabled(disabled);
    ui->pushButton_Envelope->setDisabled(disabled);
    ui->verticalSlider_Volume->setDisabled(disabled);
    ui->radioButton_Trajectory_Recorded->setDisabled(disabled);
    ui->doubleSpinBox_Speed->setDisabled(disabled);
    ui->doubleSpinBox_Radius->setDisabled(disabled);
    ui->doubleSpinBox_RadiusInt->setDisabled(disabled);
    ui->doubleSpinBox_Angle->setDisabled(disabled);
    ui->doubleSpinBox_Stretch->setDisabled(disabled);
    ui->doubleSpinBox_Expansion->setDisabled(disabled);
    ui->doubleSpinBox_Progress->setDisabled(disabled);
    ui->dial_Speed->setDisabled(disabled);
    ui->dial_Radius->setDisabled(disabled);
    ui->dial_RadiusInt->setDisabled(disabled);
    ui->dial_Angle->setDisabled(disabled);
    ui->dial_Stretch->setDisabled(disabled);
    ui->dial_Expansion->setDisabled(disabled);
    ui->dial_Progress->setDisabled(disabled);
}

void CloudDialog::createGrainsPositions()
{
    ui->graphicsView->show();
    QPen whitePen (Qt::white);
    whitePen.setWidth(1);

    ui->dial_Grain_X->setDisabled(false);
    ui->doubleSpinBox_Grain_X->setDisabled(false);
    ui->dial_Grain_Y->setDisabled(false);
    ui->doubleSpinBox_Grain_Y->setDisabled(false);
    ui->dial_Grain->setDisabled(false);
    ui->doubleSpinBox_Grain->setDisabled(false);
    ui->dial_Grain->setMaximum(ui->dial_Grains->value());
    ui->doubleSpinBox_Grain->setMaximum(ui->dial_Grains->value());
    int previousX = 0;
    int previousY = 0;
    for (unsigned long i = 0; i < cloudRef->getNumGrains(); i = i + 1) {
        myGrainPositions.push_back(new GrainPosition());
        int currentX = cloudVisRef->getGrainPositionX(i);
        int currentY = - cloudVisRef->getGrainPositionY(i);

        myGrainPositions[i]->myNode.setX(currentX);
        myGrainPositions[i]->myNode.setY(currentY);
        myGrainPositions[i]->myPointNode.setX(currentX);
        myGrainPositions[i]->myPointNode.setY(currentY);

        myGrainPositions[i]->myLine.setLine(currentX,currentY,previousX,previousY);
        myGrainPositions[i]->myLine.setZValue(1);
        myGrainPositions[i]->myNode.setZValue(2);
        grainsGraphicScene->addItem(&myGrainPositions[i]->myLine);
        grainsGraphicScene->addItem(&myGrainPositions[i]->myNode);

        myGrainPositions[i]->myLimits.maxX.exist = true;
        myGrainPositions[i]->myLimits.maxY.exist = true;
        myGrainPositions[i]->myLimits.minX.exist = true;
        myGrainPositions[i]->myLimits.minY.exist = true;

        previousX = currentX;
        previousY = currentY;
        if (i == cloudRef->getNumGrains() - 1)
            myGrainPositions[0]->myLine.setLine(myGrainPositions[0]->myPointNode.x(),
                                                myGrainPositions[0]->myPointNode.y(),
                                                previousX,previousY);
    }
}

void CloudDialog::deleteGrainsPositions()
{
    ui->graphicsView->hide();
    ui->dial_Grain_X->setDisabled(true);
    ui->doubleSpinBox_Grain_X->setDisabled(true);
    ui->dial_Grain_Y->setDisabled(true);
    ui->doubleSpinBox_Grain_Y->setDisabled(true);
    ui->dial_Grain->setDisabled(true);
    ui->doubleSpinBox_Grain->setDisabled(true);
    unsigned long lastGrain = myGrainPositions.size();
    for (unsigned long i = 0;i < lastGrain;i = i + 1) {
        delete myGrainPositions.back();
        myGrainPositions.pop_back();
    }

}

void CloudDialog::updateGrainPosition(unsigned long l_numGrain, int new_x, int new_y)
{
    myGrainPositions[l_numGrain]->myPointNode.setX(new_x);
    myGrainPositions[l_numGrain]->myPointNode.setY(new_y);
    unsigned long currentGrainPos = l_numGrain;
    unsigned long lastGrainPos = l_numGrain - 1;
    unsigned long nextGrainPos = l_numGrain + 1;
    if (l_numGrain == 0) {
        lastGrainPos = cloudRef->getNumGrains() - 1;
        if (cloudRef->getNumGrains() == 1)
           nextGrainPos = l_numGrain;
    }
    else if ((l_numGrain + 1) == cloudRef->getNumGrains())
        nextGrainPos = 0;

    myGrainPositions[currentGrainPos]->myLine.setLine(new_x, new_y, myGrainPositions[lastGrainPos]->myPointNode.x(),
                                                                    myGrainPositions[lastGrainPos]->myPointNode.y());
    myGrainPositions[nextGrainPos]->myLine.setLine(myGrainPositions[nextGrainPos]->myPointNode.x(),
                                                   myGrainPositions[nextGrainPos]->myPointNode.y(), new_x, new_y);

    myGrainPositions[l_numGrain]->myLimits.maxX.value = int(sqrt(10000 - pow(new_y, 2)));
    myGrainPositions[l_numGrain]->myLimits.maxY.value = int(sqrt(10000 - pow(new_x, 2)));
    myGrainPositions[l_numGrain]->myLimits.minX.value = int(- sqrt(10000 - pow(myGrainPositions[currentGrainPos]->myPointNode.y(), 2)));
    myGrainPositions[l_numGrain]->myLimits.minY.value = int(- sqrt(10000 - pow(myGrainPositions[currentGrainPos]->myPointNode.x(), 2)));

    myGrainPositions[l_numGrain]->myNode.setLimits(myGrainPositions[l_numGrain]->myLimits);
    myGrainPositions[l_numGrain]->myNode.moveToPos(new_x,new_y);
    myGrainPositions[l_numGrain]->myNode.endMove();

    cloudVisRef->setGrainPosition(l_numGrain, new_x, - new_y);

    if (int (l_numGrain + 1) == int (ui->doubleSpinBox_Grain->value())) {
        ui->doubleSpinBox_Grain_X->setValue(new_x);
        ui->doubleSpinBox_Grain_X->setMaximum(int(sqrt(10000 - pow(new_y, 2))));
        ui->doubleSpinBox_Grain_X->setMinimum(-int(sqrt(10000 - pow(new_y, 2))));
        ui->doubleSpinBox_Grain_Y->setValue(new_y);
        ui->doubleSpinBox_Grain_Y->setMaximum(int(sqrt(10000 - pow(new_x, 2))));
        ui->doubleSpinBox_Grain_Y->setMinimum(-int(sqrt(10000 - pow(new_x, 2))));
        ui->dial_Grain_X->setValue(new_x);
        ui->dial_Grain_X->setMaximum(int(sqrt(10000 - pow(new_y, 2))));
        ui->dial_Grain_X->setMinimum(-int(sqrt(10000 - pow(new_y, 2))));
        ui->dial_Grain_Y->setValue(new_y);
        ui->dial_Grain_Y->setMaximum(int(sqrt(10000 - pow(new_x, 2))));
        ui->dial_Grain_Y->setMinimum(-int(sqrt(10000 - pow(new_x, 2))));
    }
}

void CloudDialog::updateFromGraph()
{
    if (ui->radioButton_PositionManual->isChecked()) {
        for (unsigned long i = 0; i < cloudRef->getNumGrains(); i = i + 1) {
            if (myGrainPositions[i]->myNode.moved()) {
                updateGrainPosition(i, int(myGrainPositions[i]->myNode.x()), int(myGrainPositions[i]->myNode.y()));
            }
        }
    }
}

void CloudDialog::on_dial_Overlap_valueChanged(int value)
{
    ui->doubleSpinBox_Overlap->setValue(double (value) / 100);
}

void CloudDialog::on_doubleSpinBox_Overlap_valueChanged(double arg1)
{
    ui->dial_Overlap->setValue(int (arg1 * 100));
    if (!linking)
        cloudRef->setOverlap(arg1);
}

void CloudDialog::on_doubleSpinBox_Grains_valueChanged(double arg1)
{
    ui->dial_Grains->setValue(int (arg1));
    if (!linking)
        cloudRef->setNumGrains(int (arg1));
}

void CloudDialog::on_dial_Grains_valueChanged(int value)
{
    ui->doubleSpinBox_Grains->setValue(value);
}

void CloudDialog::on_doubleSpinBox_Y_valueChanged(double arg1)
{
    ui->dial_Y->setValue(int (arg1));
    if (!linking){
        cloudVisRef->updateCloudOrigin(cloudVisRef->getOriginX(), int(arg1));
        cloudVisRef->updateCloudPosition(cloudVisRef->getOriginX(),cloudVisRef->getOriginY());
    }

}

void CloudDialog::on_dial_Y_valueChanged(int value)
{
    ui->doubleSpinBox_Y->setValue(value);
}

void CloudDialog::on_doubleSpinBox_X_valueChanged(double arg1)
{
    ui->dial_X->setValue(int(arg1));
    if (!linking) {
        cloudVisRef->updateCloudOrigin(int(arg1), cloudVisRef->getOriginY());
        cloudVisRef->updateCloudPosition(cloudVisRef->getOriginX(),cloudVisRef->getOriginY());
    }
}

void CloudDialog::on_dial_X_valueChanged(int value)
{
    ui->doubleSpinBox_X->setValue(value);
}

void CloudDialog::on_dial_X_Extent_valueChanged(int value)
{
    ui->doubleSpinBox_X_Extent->setValue(value);
}

void CloudDialog::on_doubleSpinBox_X_Extent_valueChanged(double arg1)
{
    ui->dial_X_Extent->setValue(int(arg1));
    if (!linking){
        cloudVisRef->setFixedXRandExtent(int(arg1));
    }
}

void CloudDialog::on_dial_Y_Extent_valueChanged(int value)
{
    ui->doubleSpinBox_Y_Extent->setValue(value);
}

void CloudDialog::on_doubleSpinBox_Y_Extent_valueChanged(double arg1)
{
    ui->dial_Y_Extent->setValue(int(arg1));
    if (!linking){
        cloudVisRef->setFixedYRandExtent(int(arg1));
    }
}

void CloudDialog::on_dial_LFO_Freq_valueChanged(int value)
{
    ui->doubleSpinBox_LFO_Freq->setValue((double) value / 100);
}

void CloudDialog::on_doubleSpinBox_LFO_Freq_valueChanged(double arg1)
{
    ui->dial_LFO_Freq->setValue(arg1 * 100);
    if (!linking)
        cloudRef->setPitchLFOFreq(arg1);
}

void CloudDialog::on_dial_LFO_Amp_valueChanged(int value)
{
    ui->doubleSpinBox_LFO_Amp->setValue((double) value / 1000);
}

void CloudDialog::on_doubleSpinBox_LFO_Amp_valueChanged(double arg1)
{
    ui->dial_LFO_Amp->setValue(arg1 * 1000);
    if (!linking)
        cloudRef->setPitchLFOAmount(arg1);
}

void CloudDialog::on_dial_Duration_valueChanged(int value)
{
    ui->doubleSpinBox_Duration->setValue((double) value);
}

void CloudDialog::on_doubleSpinBox_Duration_valueChanged(double arg1)
{
    ui->dial_Duration->setValue(int(arg1));
    if (!linking)
        cloudRef->setDurationMs(arg1);
}

void CloudDialog::on_dial_Pitch_valueChanged(int value)
{
    ui->doubleSpinBox_Pitch->setValue((double) value / 100);
}

void CloudDialog::on_doubleSpinBox_Pitch_valueChanged(double arg1)
{
    ui->dial_Pitch->setValue(arg1 * 100);
    if (!linking)
        cloudRef->setPitch(arg1);
}

void CloudDialog::on_verticalSlider_Volume_valueChanged(int value)
{
    ui->doubleSpinBox_Volume->setValue((double) value / 1000);
}

void CloudDialog::on_doubleSpinBox_Volume_valueChanged(double arg1)
{
    ui->verticalSlider_Volume->setValue(arg1 * 1000);
    if (!linking)
        cloudRef->setVolumeDb(arg1);
}

void CloudDialog::on_doubleSpinBox_Midi_Note_valueChanged(double arg1)
{
    if (!linking)
        cloudRef->setMidiNote(int(arg1));
}

void CloudDialog::on_checkBox_Active_toggled(bool checked)
{
    if (!linking)
        cloudRef->setActiveState(checked);
        cloudVisRef->setIsPlayed(checked);
}

void CloudDialog::on_checkBox_Locked_toggled(bool checked)
{
    cloudRef->setLockedState(checked);
    setDisableAllWidgets(checked);
}

void CloudDialog::on_radioButton_Window_Hanning_toggled(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setWindowType(HANNING);
}

void CloudDialog::on_radioButton_Window_Triangle_toggled(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setWindowType(TRIANGLE);
}

void CloudDialog::on_radioButton_Window_Expdec_toggled(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setWindowType(EXPDEC);
}

void CloudDialog::on_radioButton_Window_Rexpdec_toggled(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setWindowType(REXPDEC);
}

void CloudDialog::on_radioButton_Window_Sinc_toggled(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setWindowType(SINC);
}

void CloudDialog::on_radioButton_Window_Random_toggled(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setWindowType(RANDOM_WIN);
}

void CloudDialog::on_radioButton_Balance_Unity_toggled(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setSpatialMode(UNITY, -1);
}

void CloudDialog::on_radioButton_Balance_Stereo_toggled(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setSpatialMode(STEREO, -1);
}

void CloudDialog::on_radioButton_Balance_Around_toggled(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setSpatialMode(AROUND, -1);
}

void CloudDialog::on_radioButton_Direction_Forward_clicked(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setDirection(FORWARD);
}

void CloudDialog::on_radioButton_Direction_Backward_clicked(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setDirection(BACKWARD);
}

void CloudDialog::on_radioButton_Direction_Random_clicked(bool checked)
{
    if ((!linking) & (checked))
        cloudRef->setDirection(RANDOM_DIR);
}

void CloudDialog::on_pushButton_Envelope_clicked()
{
    ParamEnv localParamEnv;
    localParamEnv = cloudRef->getEnvelopeVolumeParam();
    cloudRef->getEnvelopeVolume().envDialogShow(localParamEnv);
    cloudRef->setEnvelopeVolumeParam(localParamEnv);
}

void CloudDialog::on_lineEdit_Name_textEdited(const QString &arg1)
{
    if (!linking)
        cloudRef->setName(arg1);
}

void CloudDialog::on_doubleSpinBox_Output_First_valueChanged(double arg1)
{
    if (!linking)
        if (ui->doubleSpinBox_Output_Last->value() >= arg1)
            cloudRef->setOutputFirst(arg1);
        else {
            ui->doubleSpinBox_Output_First->setValue(ui->doubleSpinBox_Output_Last->value());
            cloudRef->setOutputFirst(ui->doubleSpinBox_Output_Last->value());
        }
}

void CloudDialog::on_doubleSpinBox_Output_Last_valueChanged(double arg1)
{
    if (!linking)
        if (ui->doubleSpinBox_Output_First->value() <= arg1)
            cloudRef->setOutputLast(arg1);
        else {
            ui->doubleSpinBox_Output_Last->setValue(ui->doubleSpinBox_Output_First->value());
            cloudRef->setOutputLast(ui->doubleSpinBox_Output_First->value());
        }
}

void CloudDialog::on_dial_Speed_valueChanged(int value)
{
    ui->doubleSpinBox_Speed->setValue((double) value);
}

void CloudDialog::on_doubleSpinBox_Speed_valueChanged(double arg1)
{
    ui->dial_Speed->setValue(int(arg1));
    if (!linking) {
        if (cloudVisRef->getTrajectory() != nullptr)
            cloudVisRef->trajectoryChangeSpeed(arg1);
    }
}

void CloudDialog::on_dial_Radius_valueChanged(int value)
{
    ui->doubleSpinBox_Radius->setValue((double) value);
}

void CloudDialog::on_doubleSpinBox_Radius_valueChanged(double arg1)
{
    ui->dial_Radius->setValue(int(arg1));
    if (!linking) {
        if (cloudVisRef->getTrajectory() != nullptr)
            cloudVisRef->trajectoryChangeRadius(arg1);
    }
}

void CloudDialog::on_dial_Angle_valueChanged(int value)
{
    ui->doubleSpinBox_Angle->setValue(value);
}

void CloudDialog::on_doubleSpinBox_Angle_valueChanged(double arg1)
{
    ui->dial_Angle->setValue(int(arg1));
    if (!linking) {
        if (cloudVisRef->getTrajectory() != nullptr)
            cloudVisRef->trajectoryChangeAngle(arg1);
    }
}

void CloudDialog::on_dial_Stretch_valueChanged(int value)
{
    ui->doubleSpinBox_Stretch->setValue(double(value) / 1000);
}

void CloudDialog::on_doubleSpinBox_Stretch_valueChanged(double arg1)
{
    ui->dial_Stretch->setValue(int(arg1 * 1000));
    if (!linking) {
        if (cloudVisRef->getTrajectory() != nullptr)
            cloudVisRef->trajectoryChangeStretch((double) arg1);

    }
}

void CloudDialog::on_radioButton_Trajectory_Bouncing_toggled(bool checked)
{
    Trajectory *tr=nullptr;
    if ((cloudVisRef->getTrajectory() != nullptr) & (have_trajectory_bouncing)) {
        tr=new Circular(ui->doubleSpinBox_Speed->value(),ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),ui->doubleSpinBox_Radius->value(),
                        ui->doubleSpinBox_Angle->value(),0,1);

    }
    else {
        tr=new Circular(g_defaultCloudParams.speed,ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),
                        g_defaultCloudParams.radius,g_defaultCloudParams.angle,0,1);
    }
    cloudRef->setTrajectoryType(BOUNCING);
    cloudVisRef->setTrajectory(tr);
    cloudVisRef->startTrajectory();
    have_trajectory_bouncing = true;
}

void CloudDialog::on_radioButton_Trajectory_Circular_toggled(bool checked)
{
    if (linking)
        return;

    Trajectory *tr=nullptr;
    if ((cloudVisRef->getTrajectory() != nullptr) & (have_trajectory_circular))
        tr=new Circular(ui->doubleSpinBox_Speed->value(),ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),
                        ui->doubleSpinBox_Radius->value(), ui->doubleSpinBox_Angle->value(),ui->doubleSpinBox_Stretch->value(),
                        ui->doubleSpinBox_Progress->value());
    else
        tr=new Circular(g_defaultCloudParams.speed,ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),
                        g_defaultCloudParams.radius, g_defaultCloudParams.angle,g_defaultCloudParams.stretch,
                        g_defaultCloudParams.progress );
    cloudRef->setTrajectoryType(CIRCULAR);
    cloudVisRef->setTrajectory(tr);
    cloudVisRef->startTrajectory();
    have_trajectory_circular = true;
}

void CloudDialog::on_radioButton_Trajectory_Static_toggled(bool checked)
{
    if (linking)
        return;

    Trajectory *tr = nullptr;
    cloudRef->setTrajectoryType(STATIC);
    cloudVisRef->setTrajectory(tr);
    cloudVisRef->updateCloudPosition(ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value());
}

void CloudDialog::on_doubleSpinBox_RadiusInt_valueChanged(double arg1)
{
    ui->dial_RadiusInt->setValue(int(arg1));
    if (!linking) {
        if (cloudVisRef->getTrajectory() != nullptr)
            cloudVisRef->trajectoryChangeRadiusInt(int(arg1));
    }
}

void CloudDialog::on_dial_RadiusInt_valueChanged(int value)
{
    ui->doubleSpinBox_RadiusInt->setValue(value);
}

void CloudDialog::on_dial_Expansion_valueChanged(int value)
{
    ui->doubleSpinBox_Expansion->setValue(value);
}

void CloudDialog::on_doubleSpinBox_Expansion_valueChanged(double arg1)
{
    ui->dial_Expansion->setValue(int(arg1));
    if (!linking) {
        if (cloudVisRef->getTrajectory() != nullptr)
            cloudVisRef->trajectoryChangeExpansion(int(arg1));
    }
}

void CloudDialog::on_dial_Progress_valueChanged(int value)
{
    ui->doubleSpinBox_Progress->setValue(value);
}

void CloudDialog::on_doubleSpinBox_Progress_valueChanged(double arg1)
{
    ui->dial_Progress->setValue(int(arg1));
    if (!linking) {
        if (cloudVisRef->getTrajectory() != nullptr)
            cloudVisRef->trajectoryChangeProgress(int(arg1));
    }
}

void CloudDialog::on_radioButton_Trajectory_Hypotrochoid_toggled(bool checked)
{
    if (linking)
        return;

    Trajectory *tr=nullptr;
    if ((cloudVisRef->getTrajectory() != nullptr) and (have_trajectory_hypotrochoid))
        tr=new Hypotrochoid(ui->doubleSpinBox_Speed->value(), ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),ui->doubleSpinBox_Radius->value(),
                            ui->doubleSpinBox_RadiusInt->value(), ui->doubleSpinBox_Expansion->value(), ui->doubleSpinBox_Angle->value(),
                            ui->doubleSpinBox_Progress->value());
    else {
        tr=new Hypotrochoid(g_defaultCloudParams.speed, ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),g_defaultCloudParams.radius,
                            g_defaultCloudParams.radiusInt, g_defaultCloudParams.expansion, g_defaultCloudParams.angle, g_defaultCloudParams.progress );
    }
    cloudRef->setTrajectoryType(HYPOTROCHOID);
    cloudVisRef->setTrajectory(tr);
    cloudVisRef->startTrajectory();
    have_trajectory_hypotrochoid = true;
}

void CloudDialog::on_commandLinkButton_go_toggled(bool checked)
{
    if (!linking){
        if (checked)
            cloudVisRef->stopTrajectory();
        else
            cloudVisRef->startTrajectory();
     }
}

void CloudDialog::on_commandLinkButton_stop_clicked()
{
    if (!linking){
        cloudVisRef->restartTrajectory();
    }
}

void CloudDialog::on_radioButton_Trajectory_Recorded_toggled(bool checked)
{
    if (linking)
        return;

    if (!(cloudVisRef->getTrajectoryType() == RECORDED)) {
        cout << "creation trajectory" << endl;
        Trajectory *tr=nullptr;
        tr=new Recorded(0, cloudVisRef->getOriginX(),cloudVisRef->getOriginY());
        cloudRef->setTrajectoryType(RECORDED);
        cloudVisRef->updateCloudPosition(cloudVisRef->getOriginX(),cloudVisRef->getOriginY());
        cloudVisRef->setTrajectory(tr);
        cloudVisRef->setRecordTrajectoryAsked(true);
        cloudVisRef->startTrajectory();
        have_trajectory_recorded = true;
    }
}

void CloudDialog::on_checkBox_Restart_toggled(bool checked)
{
    cloudRef->setActiveRestartTrajectory(checked);
}

void CloudDialog::on_radioButton_PositionManual_toggled(bool checked)
{
    if (checked) {
        createGrainsPositions();
        cloudRef->setGrainsRandom(false);
    }
}

void CloudDialog::on_radioButton_PositionRandom_toggled(bool checked)
{
    if (checked) {
        deleteGrainsPositions();
        cloudRef->setGrainsRandom(true);
    }
}

void CloudDialog::on_doubleSpinBox_Grain_valueChanged(double arg1)
{
    ui->dial_Grain->setValue(int(arg1));
}

void CloudDialog::on_dial_Grain_valueChanged(int value)
{
    ui->doubleSpinBox_Grain->setValue(value);
}

void CloudDialog::on_doubleSpinBox_Grain_X_valueChanged(double arg1)
{
    ui->dial_Grain_X->setValue(int(arg1));
    unsigned long l_numGrain = int (ui->doubleSpinBox_Grain->value() - 1);
    updateGrainPosition(l_numGrain, int(arg1), int(myGrainPositions[l_numGrain]->myNode.y()));
}

void CloudDialog::on_dial_Grain_X_valueChanged(int value)
{
    ui->doubleSpinBox_Grain_X->setValue(value);
}

void CloudDialog::on_doubleSpinBox_Grain_Y_valueChanged(double arg1)
{
    ui->dial_Grain_Y->setValue(int(arg1));
    unsigned long l_numGrain = int (ui->doubleSpinBox_Grain->value() - 1);
    updateGrainPosition(l_numGrain, int(myGrainPositions[l_numGrain]->myNode.x()), int(arg1));
}

void CloudDialog::on_dial_Grain_Y_valueChanged(int value)
{
    ui->doubleSpinBox_Grain_Y->setValue(value);
}
