#include "visual/Circular.h"


Circular::Circular(double s,double xOr, double yOr,double xc,double yc)
    : Trajectory(s,xOr,yOr)
{
    this->centerX = xc;
    this->centerY = yc;
    std::vector<double>  orig=getOrigin();
    this->radius=sqrt((orig[0]-centerX)*(orig[0]-centerX)+(orig[1]-centerY)*(orig[1]-centerY));
    distanceToCenter=0;
}

Circular::Circular(double s,double xOr, double yOr,double r)
    : Trajectory(s,xOr,yOr)
{
    std::vector<double>  orig=getOrigin();
    this->centerX = orig[0];
    this->centerY = orig[1];
    this->radius=r;
    distanceToCenter=0;
}

std::vector<double> Circular::computeTrajectory(double dt)
{
    double sp = this->getSpeed();
    double ph=this->getPhase();
    const double PI  =3.141592653589793238463;
    ph+=sp*dt;
    //define the phase modulo the period of the trajectory to avoid having phase going to infinity
    setPhase(ph-int(ph));
    std::vector<double>  orig=getOrigin();
    std::vector<double> vecPos{0., 0.};
    //see Euler spiral for a smooth passage from a spiral to a circle
    if (distanceToCenter<=radius)
        distanceToCenter+=radius/150.;

    if(distanceToCenter<radius){
        vecPos[0] =   distanceToCenter*cos(ph*2*PI)+orig[0];
        vecPos[1] =   distanceToCenter*sin(ph*2*PI)+orig[1];
    }
    else{
        vecPos[0] =   radius*cos(ph*2*PI)+orig[0];
        vecPos[1] =   radius*sin(ph*2*PI)+orig[1];
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
