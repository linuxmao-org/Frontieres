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
    
    void on_checkBox_Restart_toggled(bool checked);

    void on_spinBox_Priority_valueChanged(int arg1);

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
