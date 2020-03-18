#ifndef CONTROL_H
#define CONTROL_H

class Control
{
public:
    // constructor
    Control();

    // destructor
    virtual ~Control();

    float getShade();
    void setShade(float newShade);
    float getInterval();
    void setIntervale(float newInterval);
    bool getActiveState();
    void setActiveState(bool newActiveState);

private:
    bool activeState = false;
    float shade = 0;
    float interval = 0;
};

#endif // CONTROL_H
