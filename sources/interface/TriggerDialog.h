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
#ifndef TRIGGERDIALOG_H
#define TRIGGERDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include "model/Trigger.h"
#include "visual/TriggerVis.h"
#include "visual/Circular.h"
#include "visual/Hypotrochoid.h"
#include "visual/Recorded.h"
#include "visual/Trajectory.h"

namespace Ui {
class TriggerDialog;
}

class TriggerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TriggerDialog(QWidget *parent = 0);
    ~TriggerDialog();
    void linkTrigger (Trigger *triggerLinked, TriggerVis *triggerVisLinked);
    void setDisableAllWidgets(bool disabled);

private slots:
    void autoUpdate();

    void on_doubleSpinBox_Y_valueChanged(double arg1);

    void on_dial_Y_valueChanged(int value);

    void on_doubleSpinBox_X_valueChanged(double arg1);

    void on_dial_X_valueChanged(int value);

    void on_dial_Extent_valueChanged(int value);

    void on_doubleSpinBox_Extent_valueChanged(double arg1);

    void on_checkBox_Active_toggled(bool checked);

    void on_checkBox_Locked_toggled(bool checked);

    void on_lineEdit_Name_textEdited(const QString &arg1);

    void on_dial_Speed_valueChanged(int value);

    void on_doubleSpinBox_Speed_valueChanged(double arg1);

    void on_dial_Radius_valueChanged(int value);

    void on_doubleSpinBox_Radius_valueChanged(double arg1);

    void on_dial_Angle_valueChanged(int value);

    void on_doubleSpinBox_Angle_valueChanged(double arg1);

    void on_dial_Stretch_valueChanged(int value);

    void on_doubleSpinBox_Stretch_valueChanged(double arg1);

    void on_radioButton_Trajectory_Bouncing_toggled(bool checked);

    void on_radioButton_Trajectory_Circular_toggled(bool checked);

    void on_radioButton_Trajectory_Static_toggled(bool checked);

    void on_doubleSpinBox_RadiusInt_valueChanged(double arg1);

    void on_dial_RadiusInt_valueChanged(int value);

    void on_dial_Expansion_valueChanged(int value);

    void on_doubleSpinBox_Expansion_valueChanged(double arg1);

    void on_dial_Progress_valueChanged(int value);

    void on_doubleSpinBox_Progress_valueChanged(double arg1);

    void on_radioButton_Trajectory_Hypotrochoid_toggled(bool checked);

    void on_commandLinkButton_go_toggled(bool checked);

    void on_commandLinkButton_stop_clicked();

    void on_radioButton_Trajectory_Recorded_toggled(bool checked);
    
    void on_radioButton_In_Nothing_toggled(bool checked);
    
    void on_radioButton_In_On_toggled(bool checked);
    
    void on_radioButton_In_Off_toggled(bool checked);
    
    void on_radioButton_In_Commute_toggled(bool checked);
    
    void on_radioButton_Out_Nothing_toggled(bool checked);
    
    void on_radioButton_Out_On_toggled(bool checked);
    
    void on_radioButton_Out_Off_toggled(bool checked);
    
    void on_radioButton_Out_Commute_toggled(bool checked);

    void on_spinBox_Priority_valueChanged(int arg1);

    void on_checkBox_Phrase_Restart_toggled(bool checked);

    void on_checkBox_Restart_toggled(bool checked);

private:
    Ui::TriggerDialog *ui;
    bool linking = false;
    bool editing = false;
    double passageValue;
    Trigger *triggerRef;
    TriggerVis *triggerVisRef;
    bool autoUpdating = false;
    bool have_trajectory_bouncing = false;
    bool have_trajectory_circular = false;
    bool have_trajectory_hypotrochoid = false;
    bool have_trajectory_recorded =false;

};

#endif // TRIGGERDIALOG_H
