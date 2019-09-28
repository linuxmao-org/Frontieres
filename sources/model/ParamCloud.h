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
#include "ParamAdsr.h"
#include "theglobals.h"

// clouds
struct CloudParams
{
    // cloud params
    float duration;
    float overlap;
    float pitch;
    float pitchLFOFreq;
    float pitchLFOAmount;
    int dirMode;
    int windowType;
    int spatialMode;
    int outputFirst;
    int outputLast;
    int channelLocation;
    int midiChannel;
    int midiNote;
    float volumeDB;
    int numGrains;
    bool activateState;
    float xRandExtent;
    float yRandExtent;
    ParamEnv envelope;
    int trajectoryType;
    bool hasTrajectory;
    float speed;
    float radius;
    float radiusInt;
    float angle;
    float stretch;
    float expansion;
    float progress;
    vector<Position *> myPosition;
};
struct ValueMax
{
    const float duration = 5000;
    const float overlap = 1;
    const float pitch = 60;
    const float pitchLFOFreq = 100;
    const float pitchLFOAmount = 100;
    const float volumeDB = 10;
    const int numGrains = 50;
    const float xRandExtent = 2000;
    const float yRandExtent = 2000;
    const float x = 2000;
    const float y = 2000;
    const float speed = 15;
    const float radius = 1000;
    const float radiusInt = 1000;
    const float angle = 360;
    const float stretch = 1;
    const float expansion = 1000;
    const float progress = 10000;
};

struct ValueMin
{
    const float duration = 1;
    const float overlap = 0;
    const float pitch = -60;
    const float pitchLFOFreq = 0;
    const float pitchLFOAmount = 0;
    const float volumeDB = -60;
    const int midiVelocityBoost = 50; // % to boost volume for small velocity
    const int numGrains = 1;
    const float xRandExtent = 0;
    const float yRandExtent = 0;
    const float x = -2000;
    const float y = -2000;
    const float speed = -15;
    const float radius = 0;
    const float radiusInt = 0;
    const float angle = 0;
    const float stretch = 0;
    const float expansion = -1000;
    const float progress = 1;
};
