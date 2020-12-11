#ifndef PHRASEDIALOG_H
#define PHRASEDIALOG_H

#include <QDialog>
#include "model/Cloud.h"
#include "interface/Node.h"

struct PhrasePosition {
public:
    Node myNode;
    QGraphicsLineItem myLine;
    QPointF myPointNode;
    NodeLimits myLimits;
};

namespace Ui {
class PhraseDialog;
}

class PhraseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PhraseDialog(QWidget *parent = nullptr);
    ~PhraseDialog();
    void linkCloud (Cloud *cloudLinked);
    void createShadePositions();
    void deleteShadePositions();
    void updateShadePosition(unsigned long l_numShade, int new_x, int new_y);
    void createIntervalPositions();
    void deleteIntervalPositions();
    void updateIntervalPosition(unsigned long l_numInterval, int new_x, int new_y);

public slots:
    void updateIntervalFromGraph();
    void updateShadeFromGraph();

private slots:
    void on_pushButton_Interval_Add_pressed();

    void on_pushButton_Shade_add_pressed();

    void on_doubleSpinBox_Horizontal_editingFinished();

    void on_doubleSpinBox_Vertical_editingFinished();

    void on_checkBox_Active_toggled(bool checked);

    void on_doubleSpinBox_Interval_Delay_editingFinished();

    void on_doubleSpinBox_Shade_Delay_editingFinished();

    void on_doubleSpinBox_Interval_Value_editingFinished();

    void on_doubleSpinBox_Shade_Value_editingFinished();

    void on_doubleSpinBox_Interval_Number_editingFinished();

    void on_doubleSpinBox_Shade_Number_editingFinished();

    void on_doubleSpinBox_Tempo_editingFinished();

    void on_pushButton_Interval_Delete_pressed();

    void on_pushButton_Shade_Delete_pressed();

    void on_checkBox_Silence_toggled(bool checked);

    void on_pushButton_Save_pressed();

    void on_pushButton_Load_pressed();

    void on_pushButton_pressed();

private:
    const int ampControl = 120;
    Ui::PhraseDialog *ui;
    Cloud *cloudRef;
    QGraphicsScene *shadeGraphicScene;
    QGraphicsScene *intervalGraphicScene;
    vector<PhrasePosition *> myShadePositions;
    vector<PhrasePosition *> myIntervalPositions;
};

#endif // PHRASEDIALOG_H
