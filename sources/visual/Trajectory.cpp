#include "visual/Trajectory.h"

void Trajectory::setSpeed(double s)
{
    this->speed = s;
}

double Trajectory::getSpeed()
{
    return this->speed;
}

Trajectory::Trajectory(double s, double x, double y)
{
    this->speed = s;
    this->phase=0.;
    this->xOrigin=x;
    this->yOrigin=y;
}



double Trajectory::getPhase()
{
    return this->phase;
}

void Trajectory::updateOrigin(double x, double y)
{
    xOrigin=x;
    yOrigin=y;
}

std::vector<double> Trajectory::getOrigin()
{
    std::vector<double> origin {xOrigin,yOrigin};
    return origin;
}

void Trajectory::setPhase(double ph)
{
    phase=ph;
}

void Trajectory::setOrigin(double x, double y)
{
    xOrigin=x;
    yOrigin=y;
}

