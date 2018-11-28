#include "MidiBank.h"

MidiBank::MidiBank()
{

}

bool MidiBank::existCombi(int numCombi)
{
    return (myBank.count(numCombi) > 0);
}

void MidiBank::createCombi(int newNumCombi)
{
    //Combination *newCombi = new Combination;
    Combination newCombi;
    myBank.insert(newNumCombi, newCombi);
}

Combination MidiBank::findCombi(int numCombi)
{
    return myBank[numCombi];
}

void MidiBank::updateCombi(Combination *newCombi, int numCombi)
{
    myBank[numCombi] = *newCombi;
    for (unsigned i = 0; i < 128; i++){
        myBank[numCombi].setNote(newCombi->getNote(i), i);
        for (unsigned j = 0; j < newCombi->getNote(i).cloudLayer.size(); j++){

        }
    }

}
