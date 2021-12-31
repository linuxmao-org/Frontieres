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
#ifndef RECORDED_H
#define RECORDED_H

#include <memory>
#include <iostream>
#include <vector>
#include "theglobals.h"
#include "Trajectory.h"

using namespace std;

class Recorded : public Trajectory {

public:
    Recorded(double c_speed, double c_xOr, double c_yOr);
    double getCenterX();
    double getCenterY();
    void setCenter(double x,double y);
    pt2d computeTrajectory(double dt);
    Position getPosition(int cpt);
    int lastPosition();
    void addPosition(int l_x, int l_y);
    void addPositionDelayed(int l_x, int l_y, double l_delay);
    ~Recorded();
    int getType();
    void setRecording(bool l_recording);
    bool getRecording();
    vector<Position *> myPosition;

private:

    double centerX;
    double centerY;
    bool recording = false;
    int lastComputedPosition = 0;
    double delayCumul = 0;
};

#endif // RECORDED_H
