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
