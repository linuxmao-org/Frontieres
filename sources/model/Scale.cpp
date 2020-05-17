#include "Scale.h"

Scale::Scale()
{

}

Scale::~Scale()
{
    reset();
}

void Scale::insertScalePosition(ScalePosition n_scalePosition)
{
    seePositions();

    unsigned long i = myScalePositions.size();
    myScalePositions.push_back(new ScalePosition);
    setScalePosition(i, n_scalePosition);

    seePositions();
}

void Scale::deleteScalePosition(unsigned long i)
{
    for (unsigned long j = i; j < myScalePositions.size() - 1; i = i + 1) {
        myScalePositions[j] = myScalePositions[j + 1];
    }
    myScalePositions.pop_back();
}

ScalePosition Scale::getScalePosition(unsigned long i)
{
    if (i < myScalePositions.size())
        return *myScalePositions[i];
}

void Scale::setScalePosition(unsigned long i, ScalePosition n_scalePosition)
{
    myScalePositions[i]->pitchInterval = n_scalePosition.pitchInterval;
}

int Scale::getSize()
{
    return myScalePositions.size();
}

int Scale::reset()
{
    int l_max = myScalePositions.size();
    for (unsigned long i = 0; i < l_max; i++) {
        myScalePositions.pop_back();
    }
}

double Scale::nearest(double c_interval, double c_minInterval, double c_maxInterval)
{
    int l_num = 0;
    double l_interval = c_interval;
    if (myScalePositions.size() != 0) {
        l_num = 1;
        l_interval = myScalePositions[0]->pitchInterval;
        double n_proxi = abs(c_interval - myScalePositions[0]->pitchInterval);
        double a_proxi = n_proxi;
        for (int i = 0; i < myScalePositions.size(); i = i + 1) {
            n_proxi = abs(c_interval - myScalePositions[i]->pitchInterval);
            if ((n_proxi < a_proxi) &&
                (c_minInterval <= myScalePositions[i]->pitchInterval) &&
                (c_maxInterval >= myScalePositions[i]->pitchInterval)) {
                l_num = i + 1;
                a_proxi = n_proxi;
                l_interval = myScalePositions[i]->pitchInterval;
            }
        }
    }
    return l_interval;
}

void Scale::seePositions()
{
/*    for (int i = 0; i < myScalePositions.size(); i=i+1) {
        cout << "i=" << i ;
        cout << " , interval ="<< myScalePositions[i]->pitchInterval << endl;
    }*/
}
