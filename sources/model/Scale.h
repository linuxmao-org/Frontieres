#ifndef SCALE_H
#define SCALE_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <QtCore>
#include <QtGui>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

using namespace std;

struct ScalePosition {
public:
//    QString name;
    double pitchInterval;
//    QColor lineColor;
//    QGraphicsItem *line;
};

class Scale
{
public:
    Scale();
    // destructor
    virtual ~Scale();
    void insertScalePosition(ScalePosition n_scalePosition);
    void deleteScalePosition(unsigned long i);
    ScalePosition getScalePosition(unsigned long i);
    void setScalePosition(unsigned long i, ScalePosition n_scalePosition);
    int getSize();
    int reset();
    double nearest(double c_interval, double c_minInterval, double c_maxInterval);
    void seePositions();

private:
    vector<ScalePosition *> myScalePositions;

};

#endif // SCALE_H
