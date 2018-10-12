//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2018  Jean Pierre Cimalando
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

#ifndef FRONTIERES_H
#define FRONTIERES_H

#include <string>
#include <vector>
#include <QApplication>
#include <QDialog>
#include <QFileDialog>
class SoundRect;
class GrainCluster;
class GrainClusterVis;
struct AudioFile;
class QtFont3D;

//-----------------------------------------------------------------------------
// Shared Data Structures, Global parameters
//-----------------------------------------------------------------------------
// parameter string
extern std::string paramString;

//-----------------------------------------------------------------------------
// Defines a point in a 3D space (coords x, y and z)
//-----------------------------------------------------------------------------
struct pt3d {
    pt3d(float _x, float _y, float _z)
        : x(_x)
        , y(_y)
        , z(_z){};

    float x;
    float y;
    float z;
};

// Initial camera movement vars
// my position
extern pt3d position;

// ENUMS
// user selection mode
enum { RECT, CLOUD };
enum { MOVE, RESIZE };
// default selection mode
extern int selectionMode;
extern int dragMode;
extern bool resizeDir;  // for rects

// audio file visualization objects
extern std::vector<SoundRect *> *soundViews;

// audio files
extern std::vector<AudioFile *> *mySounds;
// audio file visualization objects
extern std::vector<SoundRect *> *soundViews;
// grain cloud audio objects
extern std::vector<GrainCluster *> *grainCloud;
// grain cloud visualization objects
extern std::vector<GrainClusterVis *> *grainCloudVis;
// cloud counter
extern unsigned int numClouds;

// not used yet - for multiple selection
extern std::vector<int> *selectionIndices;

// selection helper vars
extern int selectedCloud;
extern int selectedRect;
extern bool menuFlag;
extern int selectionIndex;

// cloud parameter changing
enum {
    NUMGRAINS,
    DURATION,
    WINDOW,
    MOTIONX,
    MOTIONY,
    MOTIONXY,
    DIRECTION,
    OVERLAP,
    PITCH,
    ANIMATE,
    P_LFO_FREQ,
    P_LFO_AMT,
    SPATIALIZE,
    VOLUME
};

// flag indicating parameter change
extern bool paramChanged;
extern unsigned int currentParam;
extern double lastParamChangeTime;
extern double tempParamVal;

// mouse coordinate initialization
extern int mouseX;
extern int mouseY;
enum { veryHighNumber = 50000000 };
extern long lastDragX;
extern long lastDragY;

// text renderer
extern QtFont3D *text_renderer;
enum {
    text_fontSize = 12,
    text_fontScaleDown = 100
      //jpc: pour dessiner le texte à l'échelle en QtFont3D,
      //  j'ai mis un facteur qui donne à peu près comme le logiciel original.
};

void updateMouseCoords(int x, int y);
void deselect(int mode);

void printUsage();
void printParam();

void cleaningFunction();

#endif  // FRONTIERES_H
