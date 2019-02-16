#include "MidiInstrum.h"

MidiInstrument::MidiInstrument()
{

}

int MidiInstrument::getMidiCombi(int l_channel)
{
    return midiChannelCombi[l_channel - 1];
}

void MidiInstrument::setMidiCombi(int l_channel, int l_numCombi)
{
    midiChannelCombi[l_channel - 1] = l_numCombi;
}
