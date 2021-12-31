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
#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <memory>
#include <iostream>
#include <stdio.h>
#include "theglobals.h"
#include "utility/GTime.h"
#include <cmath>

enum { STATIC, BOUNCING, CIRCULAR, HYPOTROCHOID, RECORDED};

// abstract class as a base for every kind of trajectory
class Trajectory {
public:
    virtual pt2d computeTrajectory(double dt) = 0;
    explicit Trajectory(double s,double x, double y);
    virtual ~Trajectory(){};
    double getSpeed();
    double getPhase();
    pt2d getOrigin();
    void setPhase(double l_phase);
    void setSpeed(double s);
    void setOrigin(double x, double y);
    void updateOrigin(double x, double y);
    virtual int getType() = 0;
    //virtual void addPosition(Position l_Position);
    void restart();
    double getProgress();
    void setProgress(double l_progress);
    void setDistanceToCenter(double l_istance);
    double getDistanceToCenter();
    int getTrajectoryType();
    void setTrajectoryType (int l_trajectoryType);
    void setAngle(double l_angle);
    double getAngle();
    void setRadius(double l_radius);
    double getRadius();
    void setStretch(double l_stretch);
    double getStretch();
    void setRadiusInt(double l_RadiusInt);
    double getRadiusInt();
    void setExpansion(double l_Expansion);
    double getExpansion();
    double getTrajectoryStartTime();
    void setTrajectoryStartTime();
    void setLastComputedPosition(int l_lcp);
    int getLastComputedPosition();
    void setDelayCumul(double l_dc);
    double getDelayCumul();


private:
    int trajectoryType;
    //speed express in hertz
    double speed;
    // parameter between 0 and 1
    double phase;
    //origin of the trajectory
    int xOrigin,yOrigin = 0;
    double distanceToCenter;
    double progress;
    double radius;
    double angle;
    double stretch;
    double radiusInt;
    double expansion;
    double trajectoryStartTime = 0;
    int lastComputedPosition = 0;
    double delayCumul = 0;
};


#endif
