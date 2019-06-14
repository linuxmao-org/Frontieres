#include "Hypotrochoid.h"

Hypotrochoid::Hypotrochoid(double c_speed, double c_xOr, double c_yOr, double c_radius, double c_radiusInt, double c_expansion, double c_angle)
                            : Trajectory(c_speed,c_xOr,c_yOr)
{
    pt2d  orig = getOrigin();
    centerX = orig.x;
    centerY = orig.y;
    radius = c_radius;
    radiusInt = c_radiusInt;
    distanceToCenter=0;
    angle = c_angle;
    expansion = c_expansion;
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
    //define the phase modulo the period of the trajectory to avoid having phase going to infinity
    setPhase(ph - int(ph));

    pt2d orig = getOrigin();
    pt2d vecPos{0., 0.};

    double progresAngle = getPhase() * 2 * PI;

    vecPos.x = orig.x + (radius - radiusInt) * cos(progresAngle) + expansion * cos ((radius - radiusInt) / radiusInt * progresAngle);
    vecPos.y = orig.y + (radius - radiusInt) * sin(progresAngle) - expansion * sin ((radius - radiusInt) / radiusInt * progresAngle);

    return vecPos;
}

Hypotrochoid::~Hypotrochoid()
{

}

int Hypotrochoid::getType()
{
        return HYPOTROCHOID;
}
