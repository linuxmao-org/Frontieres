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
//  GrainClusterVis.cpp
//  Frontières
//
//  Created by Christopher Carlson on 11/15/11.
//

#include "GrainClusterVis.h"
#include "GrainVis.h"
#include "SoundRect.h"
#include "utility/GTime.h"

// TODO avoid this
#include "interface/MyGLApplication.h"
#include "interface/MyGLWindow.h"

// graphics includes
#ifdef __MACOSX_CORE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <math.h>

//-----------------------------------------------------------------------------------------------
// GRAPHICS
//-----------------------------------------------------------------------------------------------

GrainClusterVis::~GrainClusterVis()
{
    for (GrainVis *vis : myGrainsV)
        delete vis;
}

GrainClusterVis::GrainClusterVis(float x, float y, unsigned int numVoices,
                                 std::vector<SoundRect *> *rects)
{
    // get screen width and height
    MyGLScreen *screen = theApplication->GLwindow()->screen();
    screenWidth = screen->width();
    screenHeight = screen->height();

    startTime = GTime::instance().sec;
    // cout << "cluster started at : " << startTime << " sec " << endl;
    gcX = x;
    gcY = y;

    //    cout << "cluster x" << gcX << endl;
    //    cout << "cluster y" << gcY  << endl;


    // randomness params
    xRandExtent = 3.0;
    yRandExtent = 3.0;

    // init add and remove flags to false
    addFlag = false;
    removeFlag = false;

    // select on instantiation
    isSelected = true;

    // pulse frequency
    freq = 1.0f;

    // pointer to landscape visualization objects
    theLandscape = rects;

    for (int i = 0; i < numVoices; i++) {
        myGrainsV.push_back(new GrainVis(gcX, gcY));
    }

    numGrains = numVoices;


    // visualization stuff
    minSelRad = 15.0f;
    maxSelRad = 19.0f;
    lambda = 0.997;
    selRad = minSelRad;
    targetRad = maxSelRad;
}

void GrainClusterVis::setDuration(float dur)
{
    freq = 1000.0 / dur;
}

// print information
void GrainClusterVis::describe(std::ostream &out)
{
    out << "- X : " << getX() << "\n";
    out << "- Y : " << getY() << "\n";
    out << "- X extent : " << getXRandExtent() << "\n";
    out << "- Y extent : " << getYRandExtent() << "\n";
}

// return cluster x
float GrainClusterVis::getX()
{
    return gcX;
}
// return cluster y
float GrainClusterVis::getY()
{
    return gcY;
}

void GrainClusterVis::draw()
{
    double t_sec = GTime::instance().sec - startTime;
    // cout << t_sec << endl;

    // if ((g_time -last_gtime) > 50){
    glPushMatrix();
    glTranslatef((GLfloat)gcX, (GLfloat)gcY, 0.0);
    // Grain cluster representation
    if (isSelected)
        glColor4f(0.1, 0.7, 0.6, 0.35);
    else
        glColor4f(0.0, 0.4, 0.7, 0.3);

    selRad = minSelRad + 0.5 * (maxSelRad - minSelRad) *
                             sin(2 * PI * (freq * t_sec + 0.125));
    gluDisk(gluNewQuadric(), selRad, selRad + 5.0, 128, 2);
    glPopMatrix();

    // update grain motion;
    // Individual voices

    // disc version (lower quality, but works on graphics cards that don't support GL_POINT_SMOOTH)
    //
    //    for (int i = 0; i < numGrains; i++){
    //        glPushMatrix();
    //        myGrainsV[i]->draw(mode);
    //        glPopMatrix();
    //    }

    // end disc version

    // point version (preferred)
    glPushMatrix();
    // update grain motion;
    // Individual voices
    for (int i = 0; i < numGrains; i++) {
        myGrainsV[i]->draw();
    }
    glPopMatrix();

    // end point version
}


// get trigger position/volume relative to sound rects for single grain voice
void GrainClusterVis::getTriggerPos(unsigned int idx, double *playPos,
                                    double *playVol, float theDur)
{
    bool trigger = false;
    SoundRect *theRect = NULL;
    if (idx < myGrainsV.size()) {
        GrainVis *theGrain = myGrainsV[idx];
        // TODO: motion models
        // updateGrainPosition(idx,gcX + randf()*50.0 + randf()*(-50.0),gcY + randf()*50.0 + randf()*(-50.0));
        updateGrainPosition(idx, gcX + (randf() * xRandExtent - randf() * xRandExtent),
                            gcY + (randf() * yRandExtent - randf() * yRandExtent));
        for (int i = 0; i < theLandscape->size(); i++) {
            theRect = theLandscape->at(i);
            bool tempTrig = false;
            tempTrig = theRect->getNormedPosition(playPos, playVol, theGrain->getX(),
                                                  theGrain->getY(), i);
            if (tempTrig == true)
                trigger = true;
            // cout << "playvol: " << *playPos << ", playpos: " << *playVol << endl;
        }
        if (trigger == true) {
            theGrain->trigger(theDur);
        }
    }
}


// rand cluster size
void GrainClusterVis::setFixedXRandExtent(float X)
{
    xRandExtent = X;
}

void GrainClusterVis::setFixedYRandExtent(float Y)
{
    yRandExtent = Y;
}
void GrainClusterVis::setFixedRandExtent(float X, float Y)
{
    setFixedXRandExtent(X);
    setFixedYRandExtent(Y);
}
void GrainClusterVis::setXRandExtent(float mouseX)
{
    xRandExtent = fabs(mouseX - gcX);
    if (xRandExtent < 2.0f)
        xRandExtent = 0.0f;
}
void GrainClusterVis::setYRandExtent(float mouseY)
{
    yRandExtent = fabs(mouseY - gcY);
    if (yRandExtent < 2.0f)
        yRandExtent = 0.0f;
}
void GrainClusterVis::setRandExtent(float mouseX, float mouseY)
{
    setXRandExtent(mouseX);
    setYRandExtent(mouseY);
}
float GrainClusterVis::getXRandExtent()
{
    return xRandExtent;
}
float GrainClusterVis::getYRandExtent()
{
    return yRandExtent;
}

//
void GrainClusterVis::updateCloudPosition(float x, float y)
{
    float xDiff = x - gcX;
    float yDiff = y - gcY;
    gcX = x;
    gcY = y;
    for (int i = 0; i < myGrainsV.size(); i++) {
        float newGrainX = myGrainsV[i]->getX() + xDiff;
        float newGrainY = myGrainsV[i]->getY() + yDiff;
        myGrainsV[i]->moveTo(newGrainX, newGrainY);
    }
}

void GrainClusterVis::updateGrainPosition(int idx, float x, float y)
{
    if (idx < numGrains)
        myGrainsV[idx]->moveTo(x, y);
}


// check mouse selection
bool GrainClusterVis::select(float x, float y)
{
    float xdiff = x - gcX;
    float ydiff = y - gcY;

    if (sqrt(xdiff * xdiff + ydiff * ydiff) < maxSelRad)
        return true;
    else
        return false;
}

void GrainClusterVis::setSelectState(bool selectState)
{
    isSelected = selectState;
}

void GrainClusterVis::addGrain()
{
    //    addFlag = true;
    myGrainsV.push_back(new GrainVis(gcX, gcY));
    numGrains = myGrainsV.size();
}

// remove a grain from the cloud (visualization only)
void GrainClusterVis::removeGrain()
{
    //    removeFlag = true;
    if (numGrains > 1) {
        // delete object
        myGrainsV.pop_back();
        numGrains = myGrainsV.size();
    }
}
