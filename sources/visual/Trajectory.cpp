#include "visual/Trajectory.h"

void Trajectory::setSpeed(double s)
{
    this->speed = s;
}

double Trajectory::getSpeed()
{
    return this->speed;
}

Trajectory::Trajectory(double s)
{
    this->speed = s;
}


