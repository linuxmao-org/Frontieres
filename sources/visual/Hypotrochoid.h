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
    void setCenter(double x,double y);
    pt2d computeTrajectory(double dt);
    ~Hypotrochoid();
    int getType();

private:
    double centerX;
    double centerY;
    const int maxTurn = 1000;
};

#endif // HYPOTROCHOID_H
