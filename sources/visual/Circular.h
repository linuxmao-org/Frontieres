#ifndef CIRCULAR_H
#define CIRCULAR_H

#include <vector>
#include <memory>
#include <iostream>
#include "theglobals.h"
#include "Trajectory.h"


class Circular : public Trajectory {
public:
    Circular(double s,double xOr, double yOr,double xc,double yc);
    Circular(double s,double xOr, double yOr,double r);
    double getCenter();
    void setCenter(double x,double y);
    std::vector<double> computeTrajectory(double dt);
    ~Circular();

private:
    double centerX;
    double centerY;
    double radius;
};

#endif
