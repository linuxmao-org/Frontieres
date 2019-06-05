#ifndef BOUNCING_H
#define BOUNCING_H

#include <memory>
#include <iostream>
#include "theglobals.h"
#include "Trajectory.h"


class Bouncing : public Trajectory {
public:
    Bouncing(double bw, double s,double xOr,double yOr);
    void setBounceWidth(double bw);
    double getBounceWidth();
    pt2d computeTrajectory(double dt);
    int getType();
    ~Bouncing();

private:
    double bounceWidth;
};

#endif
