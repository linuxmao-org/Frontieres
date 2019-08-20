#include "Recorded.h"

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

    //std::cout << "compute"<<std::endl;
    pt2d orig = getOrigin();
    pt2d vecPos{0., 0.};

    if (!recording) {
        int i = getLastComputedPosition();

        //std::cout << "delaycumul="<< getDelayCumul()<<", dt="<<dt<<std::endl;

        setDelayCumul(getDelayCumul() + dt);

        if (!(getLastComputedPosition() == myPosition.size()-1)) {
           if (int(getDelayCumul() * 1000000) >= int(myPosition[i+1]->delay * 1000000)) {
                i = i + 1;
            }
        }

        setLastComputedPosition(i);

        vecPos.x = orig.x + myPosition[i]->x;
        vecPos.y = orig.y + myPosition[i]->y;

        //std::cout << "x=" << vecPos.x << ", y="<< vecPos.y<< std::endl;

        if ((getLastComputedPosition() == myPosition.size()-1) & (myPosition.size() > 1)) {
            setLastComputedPosition(0);
            setDelayCumul(0);
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

int Recorded::lastPosition()
{
    return myPosition.size();
}

void Recorded::addPosition(int l_x, int l_y)
{
    Position *tr_Position = new Position;
    pt2d orig = getOrigin();

    tr_Position->x = l_x - orig.x;
    tr_Position->y = l_y - orig.y;
    tr_Position->delay = GTime::instance().sec - getTrajectoryStartTime();

    //std::cout << "addposition, " << tr_Position->x << ", " << tr_Position->y <<", " << tr_Position->delay << std::endl;

    myPosition.push_back(tr_Position);
    //std::cout << "max="<< myPosition.size()<<std::endl;
}

void Recorded::addPositionDelayed(int l_x, int l_y, double l_delay)
{
    Position *tr_Position = new Position;
    pt2d orig = getOrigin();

    tr_Position->x = l_x;
    tr_Position->y = l_y;
    tr_Position->delay = l_delay;

    //std::cout << "addpositiondelay, " << tr_Position->x << ", " << tr_Position->y <<", " << tr_Position->delay << std::endl;

    myPosition.push_back(tr_Position);
    //std::cout << "max="<< myPosition.size()<<std::endl;
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
        setTrajectoryStartTime();
    }
    recording = l_recording;
}

bool Recorded::getRecording()
{
    return recording;
}
