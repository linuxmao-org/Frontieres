#ifndef RECORDED_H
#define RECORDED_H

#include <memory>
#include <iostream>
#include <vector>
#include "theglobals.h"
#include "Trajectory.h"

using namespace std;

class Recorded : public Trajectory {

public:
    Recorded(double c_speed, double c_xOr, double c_yOr);
    double getCenterX();
    double getCenterY();
    void setCenter(double x,double y);
    pt2d computeTrajectory(double dt);
    Position getPosition(int cpt);
    int lastPosition();
    void addPosition(int l_x, int l_y);
    void addPositionDelayed(int l_x, int l_y, double l_delay);
    ~Recorded();
    int getType();
    void setRecording(bool l_recording);
    bool getRecording();
    vector<Position *> myPosition;

private:

    double centerX;
    double centerY;
    bool recording = false;
    int lastComputedPosition = 0;
    double delayCumul = 0;
};

#endif // RECORDED_H
