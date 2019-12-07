#include "TriggerDialog.h"
#include "ui_TriggerDialog.h"
#include "model/ParamCloud.h"
#include <iostream>
#include <stdio.h>

extern ValueMin g_cloudValueMin;
extern ValueMax g_cloudValueMax;
extern CloudParams g_defaultCloudParams;

TriggerDialog::TriggerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TriggerDialog)
{
    setModal(false);
    ui->setupUi(this);

    ui->doubleSpinBox_X->setMinimum(g_cloudValueMin.x);
    ui->doubleSpinBox_X->setMaximum(g_cloudValueMax.x);
    ui->dial_X->setMaximum(g_cloudValueMax.x);
    ui->dial_X->setMinimum(g_cloudValueMin.x);

    ui->doubleSpinBox_Y->setMinimum(g_cloudValueMin.y);
    ui->doubleSpinBox_Y->setMaximum(g_cloudValueMax.y);
    ui->dial_Y->setMaximum(g_cloudValueMax.y);
    ui->dial_Y->setMinimum(g_cloudValueMin.y);

    ui->doubleSpinBox_Extent->setMinimum(g_cloudValueMin.xRandExtent);
    ui->doubleSpinBox_Extent->setMaximum(g_cloudValueMax.xRandExtent);
    ui->dial_Extent->setMaximum(g_cloudValueMax.xRandExtent);
    ui->dial_Extent->setMinimum(g_cloudValueMin.xRandExtent);

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

    QTimer *tmAutoUpdate = new QTimer(this);
    connect(tmAutoUpdate, &QTimer::timeout, this, &TriggerDialog::autoUpdate);
    tmAutoUpdate->start(500);
}

TriggerDialog::~TriggerDialog()
{
    delete ui;
}

void TriggerDialog::linkTrigger(Trigger *triggerLinked, TriggerVis *triggerVisLinked)
{
    if (editing == true)
        return;
    linking = true;
    triggerRef = triggerLinked;
    triggerVisRef = triggerVisLinked;
    if (triggerVisLinked->changedZoneExtent())
        ui->doubleSpinBox_Extent->setValue(triggerVisLinked->getZoneExtent());
    if (triggerVisLinked->changedGcX())
        ui->doubleSpinBox_X->setValue(triggerVisLinked->getOriginX());
    if (triggerVisLinked->changedGcY())
        ui->doubleSpinBox_Y->setValue(triggerVisLinked->getOriginY());
    ui->label_Id_Value->setText(QString::number(triggerLinked->getId()));
    if (autoUpdating == false)
        ui->lineEdit_Name->setText(triggerLinked->getName());
    ui->checkBox_Active->setChecked(triggerLinked->getActiveState());
    ui->checkBox_Locked->setChecked(triggerLinked->getLockedState());
    ui->checkBox_Restart->setChecked(triggerLinked->getActiveRestartTrajectory());
    if (triggerLinked->changedIn())
        switch (triggerLinked->getIn()) {
        case NOTHING:
            ui->radioButton_In_Nothing->setChecked(true);
            break;
        case ON:
            ui->radioButton_In_On->setChecked(true);
            break;
        case OFF:
            ui->radioButton_In_Off->setChecked(true);
            break;
        case COMMUTE:
            ui->radioButton_In_Commute->setChecked(true);
            break;
        default :
            break;
        }
    if (triggerLinked->changedOut())
        switch (triggerLinked->getOut()) {
        case NOTHING:
            ui->radioButton_Out_Nothing->setChecked(true);
            break;
        case ON:
            ui->radioButton_Out_On->setChecked(true);
            break;
        case OFF:
            ui->radioButton_Out_Off->setChecked(true);
            break;
        case COMMUTE:
            ui->radioButton_Out_Commute->setChecked(true);
            break;
        default :
            break;
        }
    //cout << "forme trigger, link" << endl;
    if (triggerLinked->changedTrajectoryType())
        cout << "forme trigger, traj changee" << endl;
        switch (triggerLinked->getTrajectoryType()) {
        case STATIC:
            cout << "static" << endl;
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
            cout << "bouncing" << endl;
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
            ui->doubleSpinBox_Speed->setValue(triggerVisLinked->getTrajectory()->getSpeed());
            ui->doubleSpinBox_Radius->setValue(triggerVisLinked->getTrajectory()->getRadius());
            ui->doubleSpinBox_Angle->setValue(triggerVisLinked->getTrajectory()->getAngle());
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
            ui->doubleSpinBox_Speed->setValue(triggerVisLinked->getTrajectory()->getSpeed());
            ui->doubleSpinBox_Radius->setValue(triggerVisLinked->getTrajectory()->getRadius());
            ui->doubleSpinBox_Angle->setValue(triggerVisLinked->getTrajectory()->getAngle());
            ui->doubleSpinBox_Stretch->setValue(triggerVisLinked->getTrajectory()->getStretch());
            ui->doubleSpinBox_Progress->setValue(triggerVisLinked->getTrajectory()->getProgress());
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
            ui->doubleSpinBox_Speed->setValue(triggerVisLinked->getTrajectory()->getSpeed());
            ui->doubleSpinBox_Radius->setValue(triggerVisLinked->getTrajectory()->getRadius());
            ui->doubleSpinBox_RadiusInt->setValue(triggerVisLinked->getTrajectory()->getRadiusInt());
            ui->doubleSpinBox_Angle->setValue(triggerVisLinked->getTrajectory()->getAngle());
            ui->doubleSpinBox_Expansion->setValue(triggerVisLinked->getTrajectory()->getExpansion());
            ui->doubleSpinBox_Progress->setValue(triggerVisLinked->getTrajectory()->getProgress());
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

    triggerRef->changesDone(false);
    linking = false;
}

void TriggerDialog::setDisableAllWidgets(bool disabled)
{
    ui->lineEdit_Name->setDisabled(disabled);
    ui->checkBox_Active->setDisabled(disabled);
    ui->dial_X->setDisabled(disabled);
    ui->dial_Extent->setDisabled(disabled);
    ui->dial_Y->setDisabled(disabled);
    ui->doubleSpinBox_X->setDisabled(disabled);
    ui->doubleSpinBox_Extent->setDisabled(disabled);
    ui->doubleSpinBox_Y->setDisabled(disabled);
    ui->groupBox_In->setDisabled(disabled);
    ui->groupBox_Out->setDisabled(disabled);
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

void TriggerDialog::autoUpdate()
{
    autoUpdating = true;
    if (triggerRef && triggerVisRef)
        linkTrigger(triggerRef, triggerVisRef);
    autoUpdating = false;
}

void TriggerDialog::on_doubleSpinBox_Y_valueChanged(double arg1)
{
    ui->dial_Y->setValue((int) arg1);
}

void TriggerDialog::on_dial_Y_valueChanged(int value)
{
    ui->doubleSpinBox_Y->setValue(value);
    if (!linking) {
        triggerVisRef->updateTriggerOrigin(triggerVisRef->getOriginX(),(int) value);
        triggerVisRef->updateTriggerPosition(triggerVisRef->getOriginX(),triggerVisRef->getOriginY());
    }
}

void TriggerDialog::on_doubleSpinBox_X_valueChanged(double arg1)
{
    ui->dial_X->setValue((int) arg1);
}

void TriggerDialog::on_dial_X_valueChanged(int value)
{
    ui->doubleSpinBox_X->setValue(value);
    if (!linking) {
        triggerVisRef->updateTriggerOrigin((int) value, triggerVisRef->getOriginY());
        triggerVisRef->updateTriggerPosition(triggerVisRef->getOriginX(),triggerVisRef->getOriginY());
    }
}

void TriggerDialog::on_dial_Extent_valueChanged(int value)
{
    ui->doubleSpinBox_Extent->setValue(value);
    if (!linking) {
        triggerVisRef->setFixedZoneExtent((int) value, (int) value);
    }
}

void TriggerDialog::on_doubleSpinBox_Extent_valueChanged(double arg1)
{
    ui->dial_Extent->setValue((int) arg1);
}

void TriggerDialog::on_checkBox_Active_toggled(bool checked)
{
    if (!linking)
        triggerRef->setActiveState(checked);
    triggerVisRef->setIsPlayed(checked);
}

void TriggerDialog::on_checkBox_Locked_toggled(bool checked)
{
    triggerRef->setLockedState(checked);
    setDisableAllWidgets(checked);
}

void TriggerDialog::on_lineEdit_Name_textEdited(const QString &arg1)
{
    if (!linking)
        triggerRef->setName(arg1);
}

void TriggerDialog::on_dial_Speed_valueChanged(int value)
{
    ui->doubleSpinBox_Speed->setValue((double) value);
}

void TriggerDialog::on_doubleSpinBox_Speed_valueChanged(double arg1)
{
    ui->dial_Speed->setValue((int) arg1);
    if (!linking) {
        if (triggerVisRef->getTrajectory() != nullptr)
            triggerVisRef->trajectoryChangeSpeed(arg1);
    }
}

void TriggerDialog::on_dial_Radius_valueChanged(int value)
{
    ui->doubleSpinBox_Radius->setValue((double) value);
}

void TriggerDialog::on_doubleSpinBox_Radius_valueChanged(double arg1)
{
    ui->dial_Radius->setValue((int) arg1);
    if (!linking) {
        if (triggerVisRef->getTrajectory() != nullptr)
            triggerVisRef->trajectoryChangeRadius(arg1);
    }
}

void TriggerDialog::on_dial_Angle_valueChanged(int value)
{
    ui->doubleSpinBox_Angle->setValue(value);
}

void TriggerDialog::on_doubleSpinBox_Angle_valueChanged(double arg1)
{
    ui->dial_Angle->setValue((int) arg1);
    if (!linking) {
        if (triggerVisRef->getTrajectory() != nullptr)
            triggerVisRef->trajectoryChangeAngle(arg1);
    }
}

void TriggerDialog::on_dial_Stretch_valueChanged(int value)
{
    ui->doubleSpinBox_Stretch->setValue(double(value) / 1000);
}

void TriggerDialog::on_doubleSpinBox_Stretch_valueChanged(double arg1)
{
    ui->dial_Stretch->setValue(int(arg1 * 1000));
    if (!linking) {
        if (triggerVisRef->getTrajectory() != nullptr)
            triggerVisRef->trajectoryChangeStretch((double) arg1);

    }
}

void TriggerDialog::on_radioButton_Trajectory_Bouncing_toggled(bool checked)
{
    Trajectory *tr=nullptr;
    if ((triggerVisRef->getTrajectory() != nullptr) & (have_trajectory_bouncing)) {
        tr=new Circular(ui->doubleSpinBox_Speed->value(),ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),ui->doubleSpinBox_Radius->value(),
                        ui->doubleSpinBox_Angle->value(),0,1);

    }
    else {
        tr=new Circular(g_defaultCloudParams.speed,ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),
                        g_defaultCloudParams.radius,g_defaultCloudParams.angle,0,1);
    }
    triggerRef->setTrajectoryType(BOUNCING);
    triggerVisRef->setTrajectory(tr);
    triggerVisRef->startTrajectory();
    have_trajectory_bouncing = true;
}

void TriggerDialog::on_radioButton_Trajectory_Circular_toggled(bool checked)
{
    Trajectory *tr=nullptr;
    if ((triggerVisRef->getTrajectory() != nullptr) & (have_trajectory_circular))
        tr=new Circular(ui->doubleSpinBox_Speed->value(),ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),
                        ui->doubleSpinBox_Radius->value(), ui->doubleSpinBox_Angle->value(),ui->doubleSpinBox_Stretch->value(),
                        ui->doubleSpinBox_Progress->value());
    else
        tr=new Circular(g_defaultCloudParams.speed,ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),
                        g_defaultCloudParams.radius, g_defaultCloudParams.angle,g_defaultCloudParams.stretch,
                        g_defaultCloudParams.progress );
    triggerRef->setTrajectoryType(CIRCULAR);
    triggerVisRef->setTrajectory(tr);
    triggerVisRef->startTrajectory();
    have_trajectory_circular = true;
}

void TriggerDialog::on_radioButton_Trajectory_Static_toggled(bool checked)
{
    Trajectory *tr = nullptr;
    triggerRef->setTrajectoryType(STATIC);
    triggerVisRef->setTrajectory(tr);
    triggerVisRef->updateTriggerPosition(ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value());
}

void TriggerDialog::on_doubleSpinBox_RadiusInt_valueChanged(double arg1)
{
    ui->dial_RadiusInt->setValue((int) arg1);
    if (!linking) {
        if (triggerVisRef->getTrajectory() != nullptr)
            triggerVisRef->trajectoryChangeRadiusInt((int) arg1);
    }
}

void TriggerDialog::on_dial_RadiusInt_valueChanged(int value)
{
    ui->doubleSpinBox_RadiusInt->setValue(value);
}

void TriggerDialog::on_dial_Expansion_valueChanged(int value)
{
    ui->doubleSpinBox_Expansion->setValue(value);
}

void TriggerDialog::on_doubleSpinBox_Expansion_valueChanged(double arg1)
{
    ui->dial_Expansion->setValue((int) arg1);
    if (!linking) {
        if (triggerVisRef->getTrajectory() != nullptr)
            triggerVisRef->trajectoryChangeExpansion((int) arg1);
    }
}

void TriggerDialog::on_dial_Progress_valueChanged(int value)
{
    ui->doubleSpinBox_Progress->setValue(value);
}

void TriggerDialog::on_doubleSpinBox_Progress_valueChanged(double arg1)
{
    ui->dial_Progress->setValue((int) arg1);
    if (!linking) {
        if (triggerVisRef->getTrajectory() != nullptr)
            triggerVisRef->trajectoryChangeProgress((int) arg1);
    }
}

void TriggerDialog::on_radioButton_Trajectory_Hypotrochoid_toggled(bool checked)
{
    Trajectory *tr=nullptr;
    if ((triggerVisRef->getTrajectory() != nullptr) and (have_trajectory_hypotrochoid))
        tr=new Hypotrochoid(ui->doubleSpinBox_Speed->value(), ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),ui->doubleSpinBox_Radius->value(),
                            ui->doubleSpinBox_RadiusInt->value(), ui->doubleSpinBox_Expansion->value(), ui->doubleSpinBox_Angle->value(),
                            ui->doubleSpinBox_Progress->value());
    else {
        tr=new Hypotrochoid(g_defaultCloudParams.speed, ui->doubleSpinBox_X->value(),ui->doubleSpinBox_Y->value(),g_defaultCloudParams.radius,
                            g_defaultCloudParams.radiusInt, g_defaultCloudParams.expansion, g_defaultCloudParams.angle, g_defaultCloudParams.progress );
    }
    triggerRef->setTrajectoryType(HYPOTROCHOID);
    triggerVisRef->setTrajectory(tr);
    triggerVisRef->startTrajectory();
    have_trajectory_hypotrochoid = true;
}

void TriggerDialog::on_commandLinkButton_go_toggled(bool checked)
{
    if (!linking){
        if (checked)
            triggerVisRef->stopTrajectory();
        else
            triggerVisRef->startTrajectory();
     }
}

void TriggerDialog::on_commandLinkButton_stop_clicked()
{
    if (!linking){
        triggerVisRef->restartTrajectory();
    }
}

void TriggerDialog::on_radioButton_Trajectory_Recorded_toggled(bool checked)
{
    if (!triggerVisRef->getTrajectoryType() == RECORDED) {
        Trajectory *tr=nullptr;
        tr=new Recorded(0, triggerVisRef->getOriginX(),triggerVisRef->getOriginY());
        triggerRef->setTrajectoryType(RECORDED);
        triggerVisRef->updateTriggerPosition(triggerVisRef->getOriginX(),triggerVisRef->getOriginY());
        triggerVisRef->setTrajectory(tr);
        triggerVisRef->setRecordTrajectoryAsked(true);
        triggerVisRef->startTrajectory();
        have_trajectory_recorded = true;
    }
}

void TriggerDialog::on_radioButton_In_Nothing_toggled(bool checked)
{
    if ((!linking) & (checked))
        triggerRef->setIn(NOTHING);    
}

void TriggerDialog::on_radioButton_In_On_toggled(bool checked)
{
    if ((!linking) & (checked))
        triggerRef->setIn(ON);        
}

void TriggerDialog::on_radioButton_In_Off_toggled(bool checked)
{
    if ((!linking) & (checked))
        triggerRef->setIn(OFF);       
}

void TriggerDialog::on_radioButton_In_Commute_toggled(bool checked)
{
    if ((!linking) & (checked))
        triggerRef->setIn(COMMUTE);        
}

void TriggerDialog::on_radioButton_Out_Nothing_toggled(bool checked)
{
    if ((!linking) & (checked))
        triggerRef->setOut(NOTHING);            
}

void TriggerDialog::on_radioButton_Out_On_toggled(bool checked)
{
    if ((!linking) & (checked))
        triggerRef->setOut(ON);                
}

void TriggerDialog::on_radioButton_Out_Off_toggled(bool checked)
{
    if ((!linking) & (checked))
        triggerRef->setOut(OFF);                
}

void TriggerDialog::on_radioButton_Out_Commute_toggled(bool checked)
{
    if ((!linking) & (checked))
        triggerRef->setOut(COMMUTE);                
}

void TriggerDialog::on_checkBox_Restart_toggled(bool checked)
{
    triggerRef->setActiveRestartTrajectory(checked);
}
