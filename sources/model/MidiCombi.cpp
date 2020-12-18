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
#include "MidiCombi.h"

Combination::Combination()
{

}

void Combination::addCloud(int noteForAdd, int cldId, int minVelo, int maxVelo)
{
    Layer l_Layer;
    l_Layer.cloudId = cldId;
    l_Layer.velocity.min = minVelo;
    l_Layer.velocity.max = maxVelo;
    note[noteForAdd].addCloudLayer(l_Layer);
}

void Combination::removeCloud(int noteForRemove, int cldId)
{
    note[noteForRemove].removeCloud(cldId);
}

void Combination::setName(QString newName)
{
    myName = newName;
}

Note Combination::getNote(int numNote)
{
    return note[numNote];
}

void Combination::setNote(Note newNote, int numNote)
{
    note[numNote] = newNote;
}

QString Combination::getName()
{
    return myName;
}

void Note::addCloudLayer(Layer newLayer)
{
    int i = 0;
    bool foundLayer = false;
    while (i < cloudLayer.size()){
        if (cloudLayer[i].cloudId == newLayer.cloudId){
            cloudLayer[i].velocity.min = newLayer.velocity.min;
            cloudLayer[i].velocity.max = newLayer.velocity.max;
            foundLayer = true;
            i = cloudLayer.size();
        }
        else
            i++;
    }
    if (!foundLayer)
        cloudLayer.push_back(newLayer);
}

void Note::removeCloud(int cldId)
{
    int i = 0;
    while (i < cloudLayer.size()){
        if (cloudLayer[i].cloudId == cldId)
            cloudLayer.erase(cloudLayer.begin()+i);
        else
            i++;
    }
}
