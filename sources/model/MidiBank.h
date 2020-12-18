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
    void updateCombi(Combination newCombi, int numCombi);
    void deleteCombi(int numCombi);
private:
    QMap<unsigned, Combination> myBank;
};

#endif // MIDIBANK_H
