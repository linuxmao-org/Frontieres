#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <vector>
#include <memory>
#include <iostream>
#include "theglobals.h"
#include <cmath>

// abstract class as a base for every kind of trajectory
class Trajectory {
public:
    virtual std::vector<double> computeTrajectory(double t, double initX, double initY){};
    Trajectory(double s);
    virtual ~Trajectory(){};
    void setSpeed(double s);
    double getSpeed();

private:
    double speed;
};


#endif
