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
//  CloudVis.h
//  Frontières
//
//  Created by Christopher Carlson on 11/15/11.
//

#ifndef CLOUD_VIS_H
#define CLOUD_VIS_H


// graphics includes
#ifdef __MACOSX_CORE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <vector>
#include <memory>
#include <iostream>
#include "theglobals.h"
class SceneSound;
class GrainVis;

typedef std::vector<std::unique_ptr<SceneSound>> VecSceneSound;
extern CloudParams g_defaultCloudParams;

// VISUALIZATION/CONTROLLER
class CloudVis {
public:
    // destructor
    ~CloudVis();

    // constructor (takes center position (x,y), number of grains, sound rectangles)
    CloudVis(float x, float y, unsigned int numGrainsVis, VecSceneSound *rects);

    // render
    void draw();
    // get playback position in registered rectangles and return to grain cloud
    void getTriggerPos(unsigned int idx, double *playPos, double *playVols, float dur);
    // move grains
    void updateCloudPosition(float x, float y);
    void updateGrainPosition(int idx, float x, float y);
    void setState(int idx, bool on);
    // add grain
    void addGrain();
    // remove grain element from visualization
    void removeGrain();
    // set selection (highlight)
    void setSelectState(bool state);
    // determine if mouse click is in selection range
    bool select(float x, float y);
    // get my x coordinate
    float getX();
    // get my y coordinate
    float getY();

    // randomness params for grain positions
    float getXRandExtent();
    float getYRandExtent();
    void setXRandExtent(float mouseX);
    void setYRandExtent(float mouseY);
    void setRandExtent(float mouseX, float mouseY);
    void setFixedXRandExtent(float X);
    void setFixedYRandExtent(float Y);
    void setFixedRandExtent(float X, float Y);

    // set the pulse duration (which determines the frequency of the pulse)
    void setDuration(float dur);

    // print information
    void describe(std::ostream &out);

protected:
private:
    bool isOn, isSelected;
    bool addFlag, removeFlag;
    double startTime;
    unsigned int screenWidth, screenHeight;

    float xRandExtent, yRandExtent;

    float freq;
    float gcX, gcY;
    float selRad, lambda, maxSelRad, minSelRad, targetRad;
    unsigned int numGrains;

    // grain visualizations
    std::vector<GrainVis *> myGrainsV;
    // registered sound rectangles
    VecSceneSound *theLandscape;
};

#endif
