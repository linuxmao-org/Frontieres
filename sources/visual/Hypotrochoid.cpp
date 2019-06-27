#include "Hypotrochoid.h"

Hypotrochoid::Hypotrochoid(double c_speed, double c_xOr, double c_yOr, double c_radius, double c_radiusInt, double c_expansion, double c_angle, double c_progress)
                            : Trajectory(c_speed,c_xOr,c_yOr)
{
    pt2d  orig = getOrigin();
    centerX = orig.x;
    centerY = orig.y;
    radius = c_radius;
    radiusInt = c_radiusInt;
    setDistanceToCenter(0);
    angle = c_angle;
    expansion = c_expansion;
    setProgress(c_progress);
}

double Hypotrochoid::getCenterX()
{
    return centerX;
}

double Hypotrochoid::getCenterY()
{
    return centerY;
}

double Hypotrochoid::getRadius()
{
    return radius;
}

double Hypotrochoid::getRadiusInt()
{
    return radiusInt;
}

double Hypotrochoid::getAngle()
{
    return angle;
}

double Hypotrochoid::getExpansion()
{
    return expansion;
}

void Hypotrochoid::setRadius(double newRadius)
{
    radius = newRadius;
}

void Hypotrochoid::setRadiusInt(double newRadiusInt)
{
    radiusInt = newRadiusInt;
}

void Hypotrochoid::setCenter(double x, double y)
{

}

void Hypotrochoid::setAngle(double newAngle)
{
    angle = newAngle;
}

void Hypotrochoid::setExpansion(double newExpansion)
{
    expansion = newExpansion;
}

pt2d Hypotrochoid::computeTrajectory(double dt)
{
    double sp = this->getSpeed();
    double ph = this->getPhase();
    const double PI  =3.141592653589793238463;
    ph += sp * dt;
    //define the phase modulo the period of the trajectory to avoid having phase going to infinity after maxTurn
    if ((ph > maxTurn) & ((ph - int(ph)) * 100 < 1))
        setPhase(ph - int(ph));
    else
        setPhase(ph);

    pt2d orig = getOrigin();
    pt2d vecPos{0., 0.};

    //see Euler spiral for a smooth passage from a spiral to an hypotrochoid
    if (getDistanceToCenter() < 1)
        setDistanceToCenter(getDistanceToCenter() + 1/getProgress());
    else
        setDistanceToCenter(1);

    //the modulus and the angle, a polar complex number

    double progresAngle = (getPhase() - (angle/360) + 0.25) * 2 * PI ;

    vecPos.x = orig.x + (radius - radiusInt) * getDistanceToCenter() * cos(progresAngle) + expansion * getDistanceToCenter() * cos ((radius - radiusInt) * getDistanceToCenter() / radiusInt / getDistanceToCenter() * progresAngle);
    vecPos.y = orig.y + (radius - radiusInt) * getDistanceToCenter() * sin(progresAngle) - expansion * getDistanceToCenter() * sin ((radius - radiusInt) * getDistanceToCenter() / radiusInt / getDistanceToCenter() * progresAngle);

    return vecPos;
}

Hypotrochoid::~Hypotrochoid()
{

}

int Hypotrochoid::getType()
{
    return HYPOTROCHOID;
}
