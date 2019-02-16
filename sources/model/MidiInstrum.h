#ifndef MIDIINSTRUMENT_H
#define MIDIINSTRUMENT_H

#include "MidiCombi.h"

class MidiInstrument
{
public:
    MidiInstrument();
    int getMidiCombi(int l_channel);
    void setMidiCombi(int l_channel, int l_numCombi);
private:
    int midiChannelCombi[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
};

#endif // MIDIINSTRUMENT_H
