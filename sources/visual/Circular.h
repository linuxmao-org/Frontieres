#ifndef CIRCULAR_H
#define CIRCULAR_H

#include <memory>
#include <iostream>
#include "theglobals.h"
#include "Trajectory.h"
#include <complex>


class Circular : public Trajectory {
public:
    Circular(double s, double xOr, double yOr, double r, double ang, double stch, double prgs);
    double getCenterX();
    double getCenterY();
    pt2d computeTrajectory(double dt);
    ~Circular();
    int getType();

private:
    double centerX;
    double centerY;
};

#endif
