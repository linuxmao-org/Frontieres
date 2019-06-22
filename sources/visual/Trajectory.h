#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <memory>
#include <iostream>
#include <stdio.h>
#include "theglobals.h"
#include <cmath>

enum { STATIC, BOUNCING, CIRCULAR, HYPOTROCHOID};


// abstract class as a base for every kind of trajectory
class Trajectory {
public:
    virtual pt2d computeTrajectory(double dt) = 0;
    explicit Trajectory(double s,double x, double y);
    virtual ~Trajectory(){};
    double getSpeed();
    double getPhase();
    pt2d getOrigin();
    void setPhase(double ph);
    void setSpeed(double s);
    void setOrigin(double x, double y);
    void updateOrigin(double x, double y);
    virtual int getType() = 0;



private:
    //speed express in hertz
    double speed;
    // parameter between 0 and 1
    double phase;
    //origin of the trajectory
    double xOrigin,yOrigin;
};


#endif
