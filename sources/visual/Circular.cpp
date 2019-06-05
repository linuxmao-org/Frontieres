#include "visual/Circular.h"


Circular::Circular(double s,double xOr, double yOr,double xc,double yc)
    : Trajectory(s,xOr,yOr)
{
    this->centerX = xc;
    this->centerY = yc;
    pt2d  orig=getOrigin();
    this->radius=sqrt((orig.x-centerX)*(orig.x-centerX)+(orig.y-centerY)*(orig.y-centerY));
    distanceToCenter=0;
}

Circular::Circular(double s,double xOr, double yOr,double r)
    : Trajectory(s,xOr,yOr)
{
    pt2d  orig=getOrigin();
    this->centerX = orig.x;
    this->centerY = orig.y;
    this->radius=r;
    distanceToCenter=0;
}

pt2d Circular::computeTrajectory(double dt)
{
    double sp = this->getSpeed();
    double ph=this->getPhase();
    const double PI  =3.141592653589793238463;
    ph+=sp*dt;
    //define the phase modulo the period of the trajectory to avoid having phase going to infinity
    setPhase(ph-int(ph));
    pt2d  orig=getOrigin();
    pt2d vecPos{0., 0.};
    //see Euler spiral for a smooth passage from a spiral to a circle
    if (distanceToCenter<=radius)
        distanceToCenter+=radius/150.;

    if(distanceToCenter<radius){
        vecPos.x =   distanceToCenter*cos(ph*2*PI)+orig.x;
        vecPos.y =   distanceToCenter*sin(ph*2*PI)+orig.y;
    }
    else{
        vecPos.x =   radius*cos(ph*2*PI)+orig.x;
        vecPos.y =   radius*sin(ph*2*PI)+orig.y;
    }
    return vecPos;
}

double Circular::getCenterX()
{
    return centerX;
}

double Circular::getCenterY()
{
    return centerY;
}

double Circular::getRadius()
{
    return radius;
}

void Circular::setRadius(double newRadius)
{
    radius = newRadius;
}

int Circular::getType()
{
    return 2;
}

Circular::~Circular()
{
}
