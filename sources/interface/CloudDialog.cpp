#include "interface/CloudDialog.h"
#include "ui_CloudDialog.h"
#include "model/ParamCloud.h"
#include <iostream>
#include <stdio.h>

extern ValueMin g_cloudValueMin;
extern ValueMax g_cloudValueMax;

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

    ui->doubleSpinBox_Output_First->setMinimum(0);
    ui->doubleSpinBox_Output_Last->setMinimum(0);
    ui->doubleSpinBox_Output_First->setMaximum(MY_CHANNELS - 1);
    ui->doubleSpinBox_Output_Last->setMaximum(MY_CHANNELS - 1);

    QTimer *tmAutoUpdate = new QTimer(this);
    connect(tmAutoUpdate, &QTimer::timeout, this, &CloudDialog::autoUpdate);
    tmAutoUpdate->start(500);
}

CloudDialog::~CloudDialog()
{
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
    if (editing == true)
        return;
    linking = true;
    cloudRef = cloudLinked;
    cloudVisRef = cloudVisLinked;
    if (cloudLinked->changedDurationMs())
        ui->doubleSpinBox_Duration->setValue(cloudLinked->getDurationMs());
    if (cloudLinked->changedNumGrains())
        ui->doubleSpinBox_Grains ->setValue(cloudLinked->getNumGrains());
    if (cloudLinked->changedPitchLFOAmount())
        ui->doubleSpinBox_LFO_Amp->setValue(cloudLinked->getPitchLFOAmount());
    if (cloudLinked->changedPitchLFOFreq())
        ui->doubleSpinBox_LFO_Freq->setValue(cloudLinked->getPitchLFOFreq());
    if (cloudLinked->changedOverlap())
        ui->doubleSpinBox_Overlap->setValue(cloudLinked->getOverlap());
    if (cloudLinked->changedVolumeDb())
        ui->doubleSpinBox_Volume->setValue(cloudLinked->getVolumeDb());
    if (cloudLinked->changedPitch())
        //ui->doubleSpinBox_Pitch->setValue(12*log2(cloudLinked->getPitch()));
        ui->doubleSpinBox_Pitch->setValue(cloudLinked->getPitch());
    if (cloudVisLinked->changedXRandExtent())
        ui->doubleSpinBox_X_Extent->setValue(cloudVisLinked->getXRandExtent());
    if (cloudVisLinked->changedYRandExtent())
        ui->doubleSpinBox_Y_Extent->setValue(cloudVisLinked->getYRandExtent());
    if (cloudVisLinked->changedGcX())
        ui->doubleSpinBox_X->setValue(cloudVisLinked->getX());
    if (cloudVisLinked->changedGcY())
        ui->doubleSpinBox_Y->setValue(cloudVisLinked->getY());
    if (cloudLinked->changedMidiNote())
        ui->doubleSpinBox_Midi_Note->setValue(cloudLinked->getMidiNote());
    ui->label_Id_Value->setText(QString::number(cloudLinked->getId()));
  //  if (cloudLinked->changedName())
    if (autoUpdating == false)
        ui->lineEdit_Name->setText(cloudLinked->getName());
    ui->checkBox_Active->setChecked(cloudLinked->getActiveState());
    ui->checkBox_Locked->setChecked(cloudLinked->getLockedState());
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
}

void CloudDialog::on_dial_Overlap_valueChanged(int value)
{
    ui->doubleSpinBox_Overlap->setValue((double) value / 100);
}

void CloudDialog::on_doubleSpinBox_Overlap_valueChanged(double arg1)
{
    ui->dial_Overlap->setValue(arg1 * 100);
    if (!linking)
        cloudRef->setOverlap(arg1);
}

void CloudDialog::on_doubleSpinBox_Grains_valueChanged(double arg1)
{
    ui->dial_Grains->setValue((int) arg1);
    if (!linking)
        cloudRef->setNumGrains((int) arg1);
}

void CloudDialog::on_dial_Grains_valueChanged(int value)
{
    ui->doubleSpinBox_Grains->setValue(value);
}

void CloudDialog::on_doubleSpinBox_Y_valueChanged(double arg1)
{
    ui->dial_Y->setValue((int) arg1);
    if (!linking)
        editing = true;
        passageValue = (double) arg1;
}

void CloudDialog::on_dial_Y_valueChanged(int value)
{
    ui->doubleSpinBox_Y->setValue(value);
    update_Y();
}

void CloudDialog::on_doubleSpinBox_Y_editingFinished()
{
    update_Y();
}

void CloudDialog::update_Y()
{
    if (!linking){
        cloudVisRef->setY((int) passageValue);
        editing = false;
    }
}

void CloudDialog::on_doubleSpinBox_X_valueChanged(double arg1)
{
    ui->dial_X->setValue((int) arg1);
    if (!linking)
        editing = true;
        passageValue = (double) arg1;
}

void CloudDialog::on_dial_X_valueChanged(int value)
{
    ui->doubleSpinBox_X->setValue(value);
    update_X();
}

void CloudDialog::on_doubleSpinBox_X_editingFinished()
{
    update_X();
}

void CloudDialog::update_X()
{
    if (!linking){
        cloudVisRef->setX((int) passageValue);
        editing = false;
    }
}

void CloudDialog::on_dial_X_Extent_valueChanged(int value)
{
    ui->doubleSpinBox_X_Extent->setValue(value);
    update_X_Extent();
}

void CloudDialog::on_doubleSpinBox_X_Extent_valueChanged(double arg1)
{
    ui->dial_X_Extent->setValue((int) arg1);
    if (!linking){
        editing = true;
        passageValue = (double) arg1;
    }
}

void CloudDialog::on_doubleSpinBox_X_Extent_editingFinished()
{
    update_X_Extent();
}

void CloudDialog::update_X_Extent()
{
    if (!linking){
        cloudVisRef->setFixedXRandExtent((int) passageValue);
        editing = false;
    }
}

void CloudDialog::on_dial_Y_Extent_valueChanged(int value)
{
    ui->doubleSpinBox_Y_Extent->setValue(value);
    update_Y_Extent();
}

void CloudDialog::on_doubleSpinBox_Y_Extent_valueChanged(double arg1)
{
    ui->dial_Y_Extent->setValue((int) arg1);
    if (!linking){
        editing = true;
        passageValue = (double) arg1;
    }
}

void CloudDialog::update_Y_Extent()
{
    if (!linking){
        cloudVisRef->setFixedYRandExtent((int) passageValue);
        editing = false;
    }
}

void CloudDialog::on_doubleSpinBox_Y_Extent_editingFinished()
{
    update_Y_Extent();
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
    ui->dial_Duration->setValue((int) arg1);
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
        //cloudRef->setPitch(pow(2, (float) (arg1 / 12)));
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
        cloudRef->setMidiNote((int) arg1);
}

void CloudDialog::on_doubleSpinBox_Midi_Channel_valueChanged(double arg1)
{
    if (!linking)
        cloudRef->setMidiChannel((int) arg1);
}

void CloudDialog::on_checkBox_Active_toggled(bool checked)
{
    if (!linking)
        cloudRef->setActiveState(checked);
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
