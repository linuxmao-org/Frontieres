#include "visual/Trajectory.h"

void Trajectory::setSpeed(double s)
{
//    std::cout << "setspeed = " << s << std::endl;
    this->speed = s;
}

double Trajectory::getSpeed()
{
    return speed;
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

void Trajectory::restart()
{
    setDistanceToCenter(0);
    setPhase(0);
}

double Trajectory::getProgress()
{
    return progress;
}

void Trajectory::setProgress(double newProgress)
{
    progress = newProgress;
}

void Trajectory::setDistanceToCenter(double newDistance)
{
    distanceToCenter = newDistance;
}

double Trajectory::getDistanceToCenter()
{
    return distanceToCenter;
}

pt2d Trajectory::getOrigin()
{
    pt2d origin(xOrigin,yOrigin);
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

