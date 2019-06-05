#ifndef CIRCULAR_H
#define CIRCULAR_H

#include <memory>
#include <iostream>
#include "theglobals.h"
#include "Trajectory.h"


class Circular : public Trajectory {
public:
    Circular(double s,double xOr, double yOr,double xc,double yc);
    Circular(double s,double xOr, double yOr,double r);
    double getCenterX();
    double getCenterY();
    double getRadius();
    void setRadius(double newRadius);
    void setCenter(double x,double y);
    pt2d computeTrajectory(double dt);
    ~Circular();
    int getType();

private:
    double centerX;
    double centerY;
    //to allow for a smooth way travel into orbit
    double distanceToCenter;
    double radius;
};

#endif
