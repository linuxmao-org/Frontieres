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
