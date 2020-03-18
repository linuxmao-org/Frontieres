#ifndef PHRASE_H
#define PHRASE_H

#include <vector>

using namespace std;

enum {SHADE, INTERVAL, ACTIVATE, TEMPO, LOOP};

struct Control
{
    float delay = 0;
    int type = SHADE;
    float value = 0;
    float area = 0;
};

class Phrase
{
public:
    Phrase();
    // destructor
    virtual ~Phrase();
    void addControl(Control *newControl, unsigned long l_num);
    Control getControl(unsigned long l_numControl);

private:
    vector<Control *> myControl;

};

#endif // PHRASE_H
