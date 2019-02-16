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
//  CloudVis.cpp
//  Frontières
//
//  Created by Christopher Carlson on 11/15/11.
//

#include "visual/CloudVis.h"
#include "visual/GrainVis.h"
#include "visual/SampleVis.h"
#include "model/Scene.h"
#include "model/ParamCloud.h"
#include "utility/GTime.h"

// TODO avoid this
#include "interface/MyGLApplication.h"
#include "interface/MyGLWindow.h"

#include "model/Cloud.h"
// graphics includes
#ifdef __MACOSX_CORE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <math.h>

extern CloudParams g_defaultCloudParams;

//-----------------------------------------------------------------------------------------------
// GRAPHICS
//-----------------------------------------------------------------------------------------------

CloudVis::~CloudVis()
{
    for (GrainVis *vis : myGrainsV)
        delete vis;
}

CloudVis::CloudVis(float x, float y, unsigned int numGrainsVis,
                                 VecSceneSample *rects)
{
    // get screen width and height
    MyGLScreen *screen = theApplication->GLwindow()->screen();
    screenWidth = screen->width();
    screenHeight = screen->height();

    startTime = GTime::instance().sec;

    gcX = x;
    gcY = y;

    xRandExtent = g_defaultCloudParams.xRandExtent;
    yRandExtent = g_defaultCloudParams.yRandExtent;

    // init add and remove flags to false
    addFlag = false;
    removeFlag = false;

    // select on instantiation
    isSelected = true;

    // pulse frequency
    freq = 1.0f;

    // pointer to landscape visualization objects
    theLandscape = rects;

    for (int i = 0; i < numGrainsVis; i++) {
        myGrainsV.push_back(new GrainVis(gcX, gcY));
    }

    numGrains = numGrainsVis;


    // visualization stuff
    minSelRad = 15.0f;
    maxSelRad = 19.0f;
    lambda = 0.997;
    selRad = minSelRad;
    targetRad = maxSelRad;
}

void CloudVis::setDuration(float dur)
{
    freq = 1000.0 / dur;
}

// print information
void CloudVis::describe(std::ostream &out)
{
    out << "- X : " << getX() << "\n";
    out << "- Y : " << getY() << "\n";
    out << "- X extent : " << getXRandExtent() << "\n";
    out << "- Y extent : " << getYRandExtent() << "\n";
}

void CloudVis::registerCloud(Cloud *cloudToRegister)
{
    myCloud = cloudToRegister;
}

void CloudVis::changesDone(bool done)
{
    changed_gcX= done;
    changed_gcY = done;
    changed_xRandExtent = done;
    changed_yRandExtent = done;
}

// return cloud x
float CloudVis::getX()
{
    return gcX;
}
// return cloud y
float CloudVis::getY()
{
    return gcY;
}

void CloudVis::draw()
{
    double t_sec = GTime::instance().sec - startTime;
    // cout << t_sec << endl;

    // if ((g_time -last_gtime) > 50){
    glPushMatrix();
    glTranslatef((GLfloat)gcX, (GLfloat)gcY, 0.0);
    // Cloud representation
    if (isSelected)
        glColor4f(0.1, 0.7, 0.6, 0.35);
    else
        glColor4f(0.0, 0.4, 0.7, 0.3);

    selRad = minSelRad + 0.5 * (maxSelRad - minSelRad) *
                             sin(2 * PI * (freq * t_sec + 0.125));
    gluDisk(gluNewQuadric(), selRad, selRad + 5.0, 128, 2);
    glPopMatrix();
    glPushMatrix();
    // update grain motion;
    // Individual grains
    for (int i = 0; i < numGrains; i++) {
        myGrainsV[i]->draw();
    }
    glPopMatrix();

    // end point version
}


// get trigger position/volume relative to sample visuals for single grain voice
void CloudVis::getTriggerPos(unsigned int idx, double *playPos,
                                    double *playVol, float theDur)
{
    bool trigger = false;
    SampleVis *theRect = NULL;
    if (idx < myGrainsV.size()) {
        GrainVis *theGrain = myGrainsV[idx];
        // TODO: motion models
        // updateGrainPosition(idx,gcX + randf()*50.0 + randf()*(-50.0),gcY + randf()*50.0 + randf()*(-50.0));
        updateGrainPosition(idx, gcX + (randf() * xRandExtent - randf() * xRandExtent),
                            gcY + (randf() * yRandExtent - randf() * yRandExtent));
        VecSceneSample &landscape = *theLandscape;
        for (int i = 0, n = landscape.size(); i < n; i++) {
            theRect = landscape[i]->view.get();
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


// rand cloud size
void CloudVis::setFixedXRandExtent(float X)
{
    xRandExtent = X;
    changed_xRandExtent = true;
}

void CloudVis::setFixedYRandExtent(float Y)
{
    yRandExtent = Y;
    changed_yRandExtent = true;
}
void CloudVis::setFixedRandExtent(float X, float Y)
{
    setFixedXRandExtent(X);
    setFixedYRandExtent(Y);
}

bool CloudVis::changedXRandExtent()
{
    return changed_xRandExtent;
}

bool CloudVis::changedYRandExtent()
{
    return changed_yRandExtent;
}

void CloudVis::setXRandExtent(float mouseX)
{
    xRandExtent = fabs(mouseX - gcX);
    if (xRandExtent < 2.0f)
        xRandExtent = 0.0f;
    changed_xRandExtent = true;
}
void CloudVis::setYRandExtent(float mouseY)
{
    yRandExtent = fabs(mouseY - gcY);
    if (yRandExtent < 2.0f)
        yRandExtent = 0.0f;
    changed_yRandExtent = true;
}
void CloudVis::setRandExtent(float mouseX, float mouseY)
{
    setXRandExtent(mouseX);
    setYRandExtent(mouseY);
}
float CloudVis::getXRandExtent()
{
    return xRandExtent;
}
float CloudVis::getYRandExtent()
{
    return yRandExtent;
}
void CloudVis::setX(int newX)
{
    updateCloudPosition(newX, gcY);
}

void CloudVis::setY(int newY)
{
    updateCloudPosition(gcX, newY);
}

bool CloudVis::changedGcX()
{
    return changed_gcX;
}

bool CloudVis::changedGcY()
{
    return changed_gcY;
}
//
void CloudVis::updateCloudPosition(float x, float y)
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
    changed_gcX = true;
    changed_gcY = true;
}

void CloudVis::updateGrainPosition(int idx, float x, float y)
{
    if (idx < numGrains)
        myGrainsV[idx]->moveTo(x, y);
}


// check mouse selection
bool CloudVis::select(float x, float y)
{
    float xdiff = x - gcX;
    float ydiff = y - gcY;

    if (sqrt(xdiff * xdiff + ydiff * ydiff) < maxSelRad)
        return true;
    else
        return false;
}

void CloudVis::setSelectState(bool selectState)
{
    isSelected = selectState;
}

void CloudVis::addGrain()
{
    //    addFlag = true;
    myGrainsV.push_back(new GrainVis(gcX, gcY));
    numGrains = myGrainsV.size();
}

// remove a grain from the cloud (visualization only)
void CloudVis::removeGrain()
{
    //    removeFlag = true;
    if (numGrains > 1) {
        // delete object
        delete myGrainsV[myGrainsV.size() - 1];
        myGrainsV.pop_back();
        numGrains = myGrainsV.size();
    }
}
