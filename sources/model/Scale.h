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
#include <theglobals.h>

using namespace std;

struct ScalePosition {
public:
    //String name;
    double pitchInterval;
    //QColor lineColor;
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
    void reset();
    double nearest(double c_interval, double c_minInterval, double c_maxInterval);
    void seePositions();
    bool saveScale();
    bool loadScale();
    string askNameScale(FileDirection direction);
    bool save(QFile &scaleFile);
    bool load(QFile &scaleFile);


private:
    vector<ScalePosition *> myScalePositions;

};

#endif // SCALE_H
