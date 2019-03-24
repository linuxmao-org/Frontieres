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
#include <mutex>
#include <QApplication>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
class Scene;
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

// library path
extern std::string g_audioPathUser;
extern std::string g_audioPathSystem;

// current scene
extern Scene *currentScene;
// current scene mutex
extern std::mutex currentSceneMutex;

// selection helper vars
extern bool menuFlag;

// sample name display
extern bool showSampleNames;

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
    VOLUME,
    NUM,
    NAME
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

void draw_string(float x, float y, float z, const QString &str, float scale = 1.0f);

void updateMouseCoords(int x, int y);

// CPU usage value by audio processor (0,1)
extern float dspMonitorValue;

void printUsage();
void printParam();

void cleaningFunction();

#endif  // FRONTIERES_H
