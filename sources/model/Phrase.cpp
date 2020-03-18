#include "Phrase.h"

Phrase::Phrase()
{

}

Phrase::~Phrase()
{
    for (unsigned long i = 0; i < myControl.size(); i++) {
        delete myControl[i];
    }
}

void Phrase::addControl(Control *newControl, unsigned long l_num)
{
    if (l_num > myControl.size() - 1)
        myControl.push_back(newControl);
    else {

    }
}

Control Phrase::getControl(unsigned long l_numControl)
{
    return *myControl[l_numControl];
}
