//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2011  Christopher Carlson
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
//  GrainVis.h
//  Frontières
//
//  Created by Christopher Carlson on 11/13/11.
//

#ifndef GRAIN_VIS_H
#define GRAIN_VIS_H

#include <iostream>

// GRAPHICS CLASS
// handle display, picking.
// register selection listener
class GrainVis {
public:
    // destructor
    ~GrainVis();
    // constructor
    GrainVis(float x, float y, bool c_isMidiVis);
    void draw(bool isMidiVis);
    // position update function
    void moveTo(float x, float y);
    float getX();
    float getY();
    void trigger(float theDur);

    void setGrainPosition (int l_x, int l_y);
    int getGrainPositionX ();
    int getGrainPositionY ();

private:
    bool isOn, firstTrigger;
    bool isMidiVis = false;
    double startTime;
    double triggerTime;
    float gX, gY;
    float colR, colG, colB, colA;
    float defG, defB;
    float mySize, defSize, onSize;  // GL point size
    float durSec;

    int grainPositionX = 0;
    int grainPositionY = 0;
};

#endif
