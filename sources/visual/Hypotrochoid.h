#ifndef HYPOTROCHOID_H
#define HYPOTROCHOID_H

#include <memory>
#include <iostream>
#include "theglobals.h"
#include "Trajectory.h"


class Hypotrochoid : public Trajectory {
public:
    Hypotrochoid(double c_speed, double c_xOr, double c_yOr, double c_radius, double c_radiusInt, double c_expansion, double c_angle, double c_progress);
    double getCenterX();
    double getCenterY();
    double getRadius();
    double getRadiusInt();
    double getAngle();
    double getExpansion();
    double getProgress();
    void setRadius(double newRadius);
    void setRadiusInt(double newRadiusInt);
    void setCenter(double x,double y);
    void setAngle(double newAngle);
    void setExpansion(double newExpansion);
    void setProgress(double newProgress);
    pt2d computeTrajectory(double dt);
    ~Hypotrochoid();
    int getType();

private:
    double centerX;
    double centerY;
    //to allow for a smooth way travel into orbit
    double distanceToCenter;
    double radius;
    double radiusInt;

    double angle;
    double expansion;
    double progress;
    const int maxTurn = 1000;
};

#endif // HYPOTROCHOID_H
