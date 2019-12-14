#include "visual/Trajectory.h"
#include "Circular.h"
#include "Hypotrochoid.h"

void Trajectory::setSpeed(double s)
{
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
    setTrajectoryStartTime();
    setLastComputedPosition(0);
    setDelayCumul(0);
}

double Trajectory::getProgress()
{
    return progress;
}

void Trajectory::setProgress(double l_progress)
{
    progress = l_progress;
}

void Trajectory::setDistanceToCenter(double l_istance)
{
    distanceToCenter = l_istance;
}

double Trajectory::getDistanceToCenter()
{
    return distanceToCenter;
}

int Trajectory::getTrajectoryType()
{
    return trajectoryType;
}

void Trajectory::setTrajectoryType(int l_trajectoryType)
{
    trajectoryType = l_trajectoryType;
}

void Trajectory::setAngle(double l_angle)
{
    angle = l_angle;
}

double Trajectory::getAngle()
{
    return angle;
}

void Trajectory::setRadius(double l_radius)
{
    radius = l_radius;
}

double Trajectory::getRadius()
{
    return radius;
}

void Trajectory::setStretch(double l_stretch)
{
    stretch = l_stretch;
}

double Trajectory::getStretch()
{
    return stretch;
}

void Trajectory::setRadiusInt(double l_RadiusInt)
{
    radiusInt = l_RadiusInt;
}

double Trajectory::getRadiusInt()
{
    return radiusInt;
}

void Trajectory::setExpansion(double l_Expansion)
{
    expansion = l_Expansion;
}

double Trajectory::getExpansion()
{
    return expansion;
}

double Trajectory::getTrajectoryStartTime()
{
    return trajectoryStartTime;
}

void Trajectory::setTrajectoryStartTime()
{
    trajectoryStartTime = GTime::instance().sec;
}
void Trajectory::setLastComputedPosition(int l_lcp)
{
    lastComputedPosition = l_lcp;
}

int Trajectory::getLastComputedPosition()
{
    return lastComputedPosition;
}

void Trajectory::setDelayCumul(double l_dc)
{
    delayCumul = l_dc;
}

double Trajectory::getDelayCumul()
{
    return delayCumul;
}

pt2d Trajectory::getOrigin()
{
    pt2d origin(xOrigin,yOrigin);
    return origin;
}

void Trajectory::setPhase(double l_phase)
{
    phase = l_phase;
}

void Trajectory::setOrigin(double x, double y)
{
    xOrigin=x;
    yOrigin=y;
}

