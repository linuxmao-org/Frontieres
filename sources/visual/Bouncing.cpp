#include "visual/Bouncing.h"


Bouncing::Bouncing(double bw,double s) : Trajectory(s) 
{
	this->bounceWidth=bw;
}

void Bouncing::setBounceWidth(double bw)
{
	this->bounceWidth=bw;
}
double Bouncing::getBounceWidth()
{
	return this->bounceWidth;
}

std::vector<double> Bouncing::computeTrajectory(double t,double initX, double initY){
	std::vector<double>  vecPos {0.,0.};
	double sp=this->getSpeed();
	vecPos[0]=this->bounceWidth*cos(sp*t)+initX;
	vecPos[1]=initY;
	return vecPos;
}

Bouncing::~Bouncing(){

}
