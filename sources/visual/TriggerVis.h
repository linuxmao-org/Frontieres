// More information is available at
//     https://github.com/linuxmao-org/Frontieres
//
// Copyright (C) 2019  Olivier Flatres
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

#ifndef TRIGGERVIS_H
#define TRIGGERVIS_H

#include <vector>
#include <memory>
#include <iostream>
#include "theglobals.h"
#include "Trajectory.h"
#include "Circular.h"
#include "Hypotrochoid.h"
#include "Recorded.h"

using namespace std;

class Trigger;

class TriggerVis {

public:
    // destructor
    virtual ~TriggerVis();

    // constructor
    TriggerVis(float x, float y);

    // render
    void draw();

    void updateTriggerPosition(float newX, float newY);
    void updateTriggerOrigin (float newOriginX, float newOriginY);
    void updateTriggerTrajectoryPosition (float newX, float newY);

    float getX();
    float getY();
    float getOriginX();
    float getOriginY();
    void setX(int newX);
    void setY(int newY);
    void setOriginX(int newX);
    void setOriginY(int newY);
    bool changedGcX();
    bool changedGcY();

    void changesDone(bool done);

    Trajectory* getTrajectory();
    bool getIsMoving();
    void setTrajectory(Trajectory *tr);
    void stopTrajectory();
    void startTrajectory();
    void restartTrajectory();
    void setRecordingTrajectory(bool l_state);
    bool getRecordingTrajectory();
    void setRecordTrajectoryAsked(bool l_state);
    bool getRecordTrajectoryAsked();
    bool hasTrajectory();
    bool getIsPlayed();
    void setIsPlayed(bool l_isPlayed);
    void setTrajectoryType(int l_trajectoryType);
    int getTrajectoryType();

    void setState(bool l_state);
    void setSelectState(bool state);
    bool select(float x, float y);

    float getZoneExtent();
    void setZoneExtent(float mouseX, float mouseY);
    bool changedZoneExtent();
    void setFixedZoneExtent(float X, float Y);

    void describe(std::ostream &out);

    void registerTrigger(Trigger *triggerToRegister);

    void trajectoryAddPosition(int l_x, int l_y);
    void trajectoryAddPositionDelayed(int l_x, int l_y, double l_delay);

    void trajectoryChangeSpeed(double newValue);
    void trajectoryChangeProgress(double newValue);
    void trajectoryChangeRadius(double newValue);
    void trajectoryChangeAngle(double newValue);
    void trajectoryChangeStretch(double newValue);
    void trajectoryChangeRadiusInt(double newValue);
    void trajectoryChangeExpansion(double newValue);

private:

    bool isMoving;
    bool isOn, isSelected;
    bool isPlayed = false;
    double startTime;
    double lastDrawTime;

    Trigger *myTrigger;

    float zoneExtent;
    bool changed_zoneExtent = false;

    float freq;
    float gcX, gcY;
    float origin_gcX, origin_gcY;
    bool changed_gcX = false;
    bool changed_gcY = false;
    float selRad, lambda, maxSelRad, minSelRad, targetRad;

    int trajectoryType;
    Trajectory *myTrajectory;
    bool restartingTrajectory = false;
    bool recordingTrajectory = false;
    bool recordTrajectoryAsked = false;

};

#endif // TRIGGERVIS_H
