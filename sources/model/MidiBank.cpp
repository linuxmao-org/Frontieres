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

void MidiBank::updateCombi(Combination newCombi, int numCombi)
{
    myBank[numCombi] = newCombi;
}

void MidiBank::deleteCombi(int numCombi)
{
    if (existCombi(numCombi)){
        myBank.remove(numCombi);
    }
}
