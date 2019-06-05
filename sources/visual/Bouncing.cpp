#include "visual/Bouncing.h"


Bouncing::Bouncing(double bw, double s,double x, double y)
    : Trajectory(s,x,y)
{
    this->bounceWidth = bw;
}

void Bouncing::setBounceWidth(double bw)
{
    this->bounceWidth = bw;
}
double Bouncing::getBounceWidth()
{
    return this->bounceWidth;
}

pt2d Bouncing::computeTrajectory(double dt)
{
    double sp = this->getSpeed();
    double ph=this->getPhase();
    const double PI  =3.141592653589793238463;
    ph+=sp*dt;
    //define the phase modulo the period of the trajectory to avoid having phase going to infinity
    setPhase(ph-int(ph));
    pt2d  orig=getOrigin();
    pt2d vecPos{0., 0.};
    vecPos.x = this->bounceWidth * sin(getPhase()*2*PI) + orig.x;
    vecPos.y =  orig.y;
    return vecPos;
}

int Bouncing::getType()
{
    return 1;
}

Bouncing::~Bouncing()
{
}
