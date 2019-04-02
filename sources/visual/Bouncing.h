#ifndef BOUNCING_H
#define BOUNCING_H

#include <vector>
#include <memory>
#include <iostream>
#include "theglobals.h"
#include "Trajectory.h"

//abstract class as a base for every kind of trajectory
class Bouncing : public Trajectory{
public :

	Bouncing(double bw,double s);
	void setBounceWidth(double bw);
	double getBounceWidth();
	std::vector<double> computeTrajectory(double t,double initX, double initY);
	~Bouncing();

private :
	double bounceWidth;
};

#endif
