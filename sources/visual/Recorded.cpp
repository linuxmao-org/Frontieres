#include "Recorded.h"
#include "utility/GTime.h"

Recorded::Recorded(double c_speed, double c_xOr, double c_yOr)
    : Trajectory(c_speed,c_xOr,c_yOr)
{
pt2d  orig = getOrigin();
centerX = orig.x;
centerY = orig.y;
setDistanceToCenter(0);

Position *tr_Position = new Position;
tr_Position->x = 0;
tr_Position->y = 0;
tr_Position->delay = 0;
myPosition.push_back(tr_Position);
setRecording(true);
}

double Recorded::getCenterX()
{
    return centerX;
}

double Recorded::getCenterY()
{
    return centerY;
}

pt2d Recorded::computeTrajectory(double dt)
{
    pt2d orig = getOrigin();
    pt2d vecPos{0., 0.};

    if (!recording) {
        int i = lastComputedPosition;
        delayCumul = delayCumul + dt;

        if (!(lastComputedPosition == myPosition.size()-1)) {
           if (int(delayCumul*1000000) >= int(myPosition[i+1]->delay*1000000)) {
                i = i + 1;
            }
        }

        lastComputedPosition = i;

        vecPos.x = orig.x + myPosition[i]->x;
        vecPos.y = orig.y + myPosition[i]->y;

        if ((lastComputedPosition == myPosition.size()-1) & (myPosition.size() > 1)) {
            lastComputedPosition = 0;
            delayCumul = 0;
        }
    }
    else {
        vecPos.x = orig.x + myPosition[myPosition.size()-1]->x;
        vecPos.y = orig.y + myPosition[myPosition.size()-1]->y;
    }
    return vecPos;
}

Position Recorded::getPosition(int cpt)
{
    Position l_Position;
    l_Position.x = myPosition[cpt]->x;
    l_Position.y = myPosition[cpt]->y;
    l_Position.delay = myPosition[cpt]->delay;
    return l_Position;
}

Position Recorded::lastPosition()
{
    Position l_Position;
    l_Position.x = myPosition[myPosition.size()-1]->x;
    l_Position.y = myPosition[myPosition.size()-1]->y;
    l_Position.delay = myPosition[myPosition.size()-1]->delay;
    return l_Position;
}

void Recorded::addPosition(int l_x, int l_y)
{
    Position *tr_Position = new Position;
    pt2d orig = getOrigin();

    tr_Position->x = l_x - orig.x;
    tr_Position->y = l_y - orig.y;
    tr_Position->delay = GTime::instance().sec - recordedTrajectoryStartTime;

    myPosition.push_back(tr_Position);
}

Recorded::~Recorded()
{
    for (int i = 0; i < myPosition.size(); i++) {
        delete myPosition[i];
    }
}

int Recorded::getType()
{
    return RECORDED;
}

void Recorded::setRecording(bool l_recording)
{
    if (l_recording) {
        recordedTrajectoryStartTime = GTime::instance().sec;
    }
    recording = l_recording;
}

bool Recorded::getRecording()
{
    return recording;
}
