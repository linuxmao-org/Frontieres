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

std::vector<double> Bouncing::computeTrajectory(double dt)
{
    double sp = this->getSpeed();
    double ph=this->getPhase();
    const double PI  =3.141592653589793238463;
    ph+=sp*dt;
    //define the phase modulo the period of the trajectory to avoid having phase going to infinity
    setPhase(ph-int(ph));
    std::vector<double>  orig=getOrigin();
    std::vector<double> vecPos{0., 0.};
    vecPos[0] = this->bounceWidth * sin(getPhase()*2*PI) + orig[0];
    vecPos[1] =  orig[1];
    return vecPos;
}

int Bouncing::getType()
{
    return 1;
}

Bouncing::~Bouncing()
{
}
