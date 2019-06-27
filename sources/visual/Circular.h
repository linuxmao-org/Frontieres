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
    double getRadius();
    double getAngle();
    double getStrech();
    void setRadius(double newRadius);
    void setCenter(double x,double y);
    void setAngle(double newAngle);
    void setStrech(double newStrech);
    pt2d computeTrajectory(double dt);
    ~Circular();
    int getType();

private:
    double centerX;
    double centerY;
    //to allow for a smooth way travel into orbit
    double radius;
    double angle;
    double strech;
};

#endif
