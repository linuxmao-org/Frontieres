#ifndef CLOUDDIALOG_H
#define CLOUDDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include "model/Cloud.h"
#include "visual/CloudVis.h"
#include "visual/Circular.h"
#include "visual/Hypotrochoid.h"
#include "visual/Trajectory.h"

namespace Ui {
class CloudDialog;
}

class CloudDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CloudDialog(QWidget *parent = 0);
    ~CloudDialog();
    void linkCloud (Cloud *cloudLinked, CloudVis *cloudVisLinked);
    void setDisableAllWidgets(bool disable);

private slots:
    void autoUpdate();

    void on_dial_Overlap_valueChanged(int value);

    void on_doubleSpinBox_Overlap_valueChanged(double arg1);

    void on_dial_Grains_valueChanged(int value);

    void on_doubleSpinBox_Grains_valueChanged(double arg1);

    void on_doubleSpinBox_Y_valueChanged(double arg1);

    void on_dial_Y_valueChanged(int value);

    void on_doubleSpinBox_Y_editingFinished();

    void update_Y();

    void on_doubleSpinBox_X_valueChanged(double arg1);

    void on_dial_X_valueChanged(int value);

    void on_doubleSpinBox_X_editingFinished();

    void update_X();

    void on_dial_X_Extent_valueChanged(int value);

    void on_doubleSpinBox_X_Extent_valueChanged(double arg1);

    void on_doubleSpinBox_X_Extent_editingFinished();

    void update_X_Extent();

    void on_dial_Y_Extent_valueChanged(int value);

    void on_doubleSpinBox_Y_Extent_valueChanged(double arg1);

    void on_doubleSpinBox_Y_Extent_editingFinished();

    void update_Y_Extent();

    void on_dial_LFO_Freq_valueChanged(int value);

    void on_doubleSpinBox_LFO_Freq_valueChanged(double arg1);

    void on_dial_LFO_Amp_valueChanged(int value);

    void on_doubleSpinBox_LFO_Amp_valueChanged(double arg1);

    void on_dial_Duration_valueChanged(int value);

    void on_doubleSpinBox_Duration_valueChanged(double arg1);

    void on_dial_Pitch_valueChanged(int value);

    void on_doubleSpinBox_Pitch_valueChanged(double arg1);

    void on_verticalSlider_Volume_valueChanged(int value);

    void on_doubleSpinBox_Volume_valueChanged(double arg1);

    void on_doubleSpinBox_Midi_Note_valueChanged(double arg1);

    void on_doubleSpinBox_Midi_Channel_valueChanged(double arg1);

    void on_checkBox_Active_toggled(bool checked);

    void on_radioButton_Window_Hanning_toggled(bool checked);

    void on_radioButton_Window_Triangle_toggled(bool checked);

    void on_radioButton_Window_Expdec_toggled(bool checked);

    void on_radioButton_Window_Rexpdec_toggled(bool checked);

    void on_radioButton_Window_Sinc_toggled(bool checked);

    void on_radioButton_Window_Random_toggled(bool checked);

    void on_radioButton_Balance_Unity_toggled(bool checked);

    void on_radioButton_Balance_Stereo_toggled(bool checked);

    void on_radioButton_Balance_Around_toggled(bool checked);

    void on_radioButton_Direction_Forward_clicked(bool checked);

    void on_radioButton_Direction_Backward_clicked(bool checked);

    void on_radioButton_Direction_Random_clicked(bool checked);

    void on_checkBox_Locked_toggled(bool checked);

    void on_pushButton_Envelope_clicked();

    void on_lineEdit_Name_textEdited(const QString &arg1);

    void on_doubleSpinBox_Output_First_valueChanged(double arg1);

    void on_doubleSpinBox_Output_Last_valueChanged(double arg1);

    void on_dial_Speed_valueChanged(int value);

    void on_doubleSpinBox_Speed_valueChanged(double arg1);

    void on_doubleSpinBox_Speed_editingFinished();

    void update_Speed();

    void on_dial_Radius_valueChanged(int value);

    void on_doubleSpinBox_Radius_valueChanged(double arg1);

    void on_doubleSpinBox_Radius_editingFinished();

    void update_Radius();

    void on_dial_Angle_valueChanged(int value);

    void on_doubleSpinBox_Angle_valueChanged(double arg1);

    void on_doubleSpinBox_Angle_editingFinished();

    void update_Angle();

    void on_dial_Strech_valueChanged(int value);

    void on_doubleSpinBox_Strech_valueChanged(double arg1);

    void on_doubleSpinBox_Strech_editingFinished();

    void update_Strech();

    void on_radioButton_Trajectory_Bouncing_toggled(bool checked);

    void on_radioButton_Trajectory_Circular_toggled(bool checked);

    void on_radioButton_Trajectory_Static_toggled(bool checked);

    void on_doubleSpinBox_RadiusInt_valueChanged(double arg1);

    void on_dial_RadiusInt_valueChanged(int value);

    void on_doubleSpinBox_RadiusInt_editingFinished();

    void update_RadiusInt();

    void on_dial_Expansion_valueChanged(int value);

    void on_doubleSpinBox_Expansion_valueChanged(double arg1);

    void on_doubleSpinBox_Expansion_editingFinished();

    void update_Expansion();

    void on_dial_Progress_valueChanged(int value);

    void on_doubleSpinBox_Progress_valueChanged(double arg1);

    void on_doubleSpinBox_Progress_editingFinished();

    void update_Progress();

    void on_radioButton_Trajectory_Hypotrochoid_toggled(bool checked);

    void on_commandLinkButton_go_toggled(bool checked);

    void on_commandLinkButton_stop_clicked();

    void on_radioButton_Trajectory_Recorded_toggled(bool checked);

private:
    Ui::CloudDialog *ui;
    bool linking = false;
    bool editing = false;
    double passageValue;
    Cloud *cloudRef;
    CloudVis *cloudVisRef;
    bool autoUpdating = false;
    bool have_trajectory_bouncing = false;
    bool have_trajectory_circular = false;
    bool have_trajectory_hypotrochoid = false;
    bool have_trajectory_recorded =false;
};

#endif // CLOUDDIALOG_H
