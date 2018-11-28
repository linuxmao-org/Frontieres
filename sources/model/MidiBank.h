#ifndef MIDIBANK_H
#define MIDIBANK_H

#include <QMap>
#include "MidiCombi.h"

class MidiBank
{
public:
    MidiBank();
    bool existCombi(int numCombi);
    void createCombi(int newNumCombi);
    Combination findCombi(int numCombi);
    void updateCombi(Combination *newCombi, int numCombi);
private:
    QMap<unsigned, Combination> myBank;
};

#endif // MIDIBANK_H
