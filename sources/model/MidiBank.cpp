//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     https://github.com/linuxmao-org/Frontieres
//
// Copyright (C) 2020  Olivier Flatres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
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
