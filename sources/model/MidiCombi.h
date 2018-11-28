//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2018  Olivier Flatres
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

#pragma once
#ifndef COMBINATION_H
#define COMBINATION_H

#include <vector>
#include <QString>
#include <iostream>
#include "stdio.h"

struct VeloArray {
    int min;
    int max;
};

struct Layer {
    VeloArray velocity;
    int cloudId;
};

struct Note {
public:
    std::vector<Layer> cloudLayer;
    void addCloudLayer(Layer newLayer);
    void removeCloud(int cldId);
};

class Combination
{
public:
    Combination();
    void addCloud(int noteForAdd, int cldId, int minVelo, int maxVelo);
    void removeCloud(int noteForRemove, int cldId);
    void setName(QString newName);
    Note getNote(int numNote);
    void setNote(Note newNote, int numNote);
    QString getName();
private:
    QString myName = "";
    Note note[128];
};

#endif // COMBINATION_H
