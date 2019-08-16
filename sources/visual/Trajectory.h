#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <memory>
#include <iostream>
#include <stdio.h>
#include "theglobals.h"
#include <cmath>

enum { STATIC, BOUNCING, CIRCULAR, HYPOTROCHOID, RECORDED};

struct Position {
    int x;
    int y;
    double delay;
};

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
    void setStrech(double l_strech);
    double getStrech();
    void setRadiusInt(double l_RadiusInt);
    double getRadiusInt();
    void setExpansion(double l_Expansion);
    double getExpansion();


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
    double strech;
    double radiusInt;
    double expansion;
};


#endif
