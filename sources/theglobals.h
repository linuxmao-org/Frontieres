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
//  theglobals.h
//  Frontières
//
//  Created by Christopher Carlson on 11/15/11.
//


#ifndef THEGLOBALS_H
#define THEGLOBALS_H


#include <stdlib.h>


// create BUFFERPREC datatype
typedef float BUFFERPREC;
// number of output channels
extern int theChannelCount;

// window length
#define WINDOW_LEN 2048
// graphics picking
#define NAMEINCREMENT 100

// function to produce a random float
#define randf() ((float)rand() / RAND_MAX)

#define randd() ((double)rand() / RAND_MAX)


// global attenuation to prevent clipping
static const double globalAtten = 0.5;

int constexpr g_maxMidiVoices(128);

// osc local port
//#define LOCAL_PORT 10001
//#define OUTPUT_BUFFER_SIZE 1024


//-----------------------------------------------------------------------------
// Defines a point in a 3D space (coords x, y and z)
//-----------------------------------------------------------------------------
struct pt3d {
    pt3d(float _x, float _y, float _z)
        : x(_x), y(_y), z(_z){}

    float x;
    float y;
    float z;
};

////-----------------------------------------------------------------------------
//// Defines a point in a 2D space (coords x, y)
////-----------------------------------------------------------------------------

struct pt2d
{
   pt2d( float _x, float _y ) : x(_x), y(_y) {}

   float x;
   float y;
};

// position for trajectory
struct Position {
    int x;
    int y;
    double delay;
};

// scenes files read/write direction
enum class FileDirection {
    Load,
    Save
};

// scenes
static const char *g_extensionScene = ".scn";
static const char *g_extensionCloud = ".cld";
static const char *g_extensionTrajectory = ".trj";

#endif
