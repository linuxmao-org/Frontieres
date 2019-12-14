//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     https://github.com/linuxmao-org/Frontieres
//
// Copyright (C) 2019  Olivier Flatres
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
//  TriggerVis.cpp
//  Frontières
//
//  Created by Olivier Flatrès on 11/11/2019
//
#include "visual/TriggerVis.h"

#include "model/Scene.h"
#include "model/ParamCloud.h"
#include "utility/GTime.h"
#include "model/Trigger.h"

// TODO avoid this
#include "interface/MyGLApplication.h"
#include "interface/MyGLWindow.h"

// graphics includes
#include <qopengl.h>
#ifndef __APPLE__
#include <GL/glu.h>
#endif

#include <math.h>

extern CloudParams g_defaultCloudParams;

TriggerVis::~TriggerVis()
{
    delete myTrajectory;
}

TriggerVis::TriggerVis(float x, float y)
{
    startTime = GTime::instance().sec;

    isPlayed = g_defaultCloudParams.activateState;

    // initialise trajectory

    stopTrajectory();
    myTrajectory = nullptr;
    Trajectory *tr = nullptr;
    trajectoryType = g_defaultCloudParams.trajectoryType;

    switch (g_defaultCloudParams.trajectoryType) {
    case STATIC:
        isMoving = false;
        setTrajectoryType(STATIC);
        setTrajectory(tr);
        break;
    case BOUNCING:
        tr=new Circular(g_defaultCloudParams.speed,x,y,g_defaultCloudParams.radius,g_defaultCloudParams.angle,0,1);
        isMoving = true;
        setTrajectoryType(BOUNCING);
        setTrajectory(tr);
        break;
    case CIRCULAR:
        tr=new Circular(g_defaultCloudParams.speed,x,y,g_defaultCloudParams.radius,g_defaultCloudParams.angle,
                        g_defaultCloudParams.stretch,g_defaultCloudParams.progress );
        isMoving = true;
        setTrajectoryType(CIRCULAR);
        setTrajectory(tr);
        break;
    case HYPOTROCHOID:
        tr=new Hypotrochoid(g_defaultCloudParams.speed,x,y,g_defaultCloudParams.radius,g_defaultCloudParams.radiusInt,
                            g_defaultCloudParams.expansion, g_defaultCloudParams.angle,g_defaultCloudParams.progress);
        isMoving = true;
        setTrajectoryType(HYPOTROCHOID);
        setTrajectory(tr);
        break;
    case RECORDED:{
        tr=new Recorded(0, x, y);
        Recorded *recTraj=dynamic_cast<Recorded*>(tr);
        setTrajectoryType(RECORDED);

        for (int i = 0; i < g_defaultCloudParams.myPosition.size(); i = i + 1) { // positions
            int l_x;
            int l_y;
            double l_delay;
            l_x = g_defaultCloudParams.myPosition[i]->x;
            l_y = g_defaultCloudParams.myPosition[i]->y;
            l_delay = g_defaultCloudParams.myPosition[i]->delay;
            trajectoryAddPositionDelayed(l_x, l_y, l_delay);
        }
        setTrajectory(tr);
        recTraj->setRecording(false);
        startTrajectory();
        }
        break;
    default :
        break;
    }

    updateTriggerPosition(x, y);
    updateTriggerOrigin(x, y);
    zoneExtent = g_defaultCloudParams.triggerZoneExtent;

    // select on instantiation
    isSelected = true;

    // pulse frequency
    freq = 1.0f;

    // visualization stuff
    minSelRad = minZone - 5;
    maxSelRad = minZone;
    lambda = 0.997;
    selRad = minSelRad;
    targetRad = maxSelRad;
    lastDrawTime=0;
}

void TriggerVis::draw()
{
    double t_sec = 0;
    double dt = 0;

    t_sec = GTime::instance().sec - startTime;

    dt = t_sec - lastDrawTime;
    lastDrawTime = t_sec;

    //computing trajectory
    pt2d pos = {0.,0.};

    if (this->getIsMoving() || restartingTrajectory) {
        pos = this->myTrajectory->computeTrajectory(dt);
        updateTriggerPosition(pos.x,pos.y);
        myTrigger->computeCloudsIn();
        myTrigger->computeCloudsOut();
        myTrigger->computeTriggersIn();
        myTrigger->computeTriggersOut();
    }

    glPushMatrix();
    glTranslatef((GLfloat)gcX, (GLfloat)gcY, 0.0);

    // Trigger representation
    if (isSelected)
        if (recordingTrajectory)
            glColor4f(1.0, 0.1, 0.0, 0.35);
        else
            glColor4f(0.1, 0.7, 0.6, 0.35);
    else
        glColor4f(0.0, 0.4, 0.7, 0.8);

    selRad = minSelRad + 0.5 * (maxSelRad - minSelRad) * sin(2 * PI * (freq * t_sec + 0.125));
    gluDisk(gluNewQuadric(), maxSelRad + zoneExtent, maxSelRad + zoneExtent + 1.0, 128, 2);

    glPopMatrix();
    glPushMatrix();

    // origin
    glTranslatef((GLfloat)origin_gcX, (GLfloat)origin_gcY, 0.0);

    // Trigger origin representation

    if (isSelected)
        if (recordingTrajectory || recordTrajectoryAsked)
            glColor4f(1.0, 0.1, 0.0, 0.35);
        else
            glColor4f(0.1, 0.7, 0.6, 0.35);
    else
        glColor4f(0.0, 0.4, 0.7, 0.8);

    if (myTrigger->getActiveState())
        gluPartialDisk(gluNewQuadric(), maxSelRad - 3, maxSelRad + 2, 128, 2, t_sec * 100, 300);
    else
        gluPartialDisk(gluNewQuadric(), maxSelRad - 3, maxSelRad + 2, 128, 2, 0, 400);

    glPopMatrix();
    glPushMatrix();

    glLineWidth(2.0f);

    glBegin(GL_LINE_STRIP);
    glVertex2f(origin_gcX, origin_gcY);
    glVertex2f(gcX, gcY);
    glEnd();

    glPopMatrix();

}

void TriggerVis::updateTriggerPosition(float newX, float newY)
{
    float xDiff = newX - gcX;
    float yDiff = newY - gcY;
    gcX = newX;
    gcY = newY;
}

void TriggerVis::updateTriggerOrigin(float newOriginX, float newOriginY)
{
    origin_gcX = newOriginX;
    origin_gcY = newOriginY;
    if (myTrajectory != nullptr)
        myTrajectory->setOrigin(newOriginX, newOriginY);
    changed_gcX = true;
    changed_gcY = true;
}

void TriggerVis::updateTriggerTrajectoryPosition(float newX, float newY)
{
    float xDiff = newX - gcX;
    float yDiff = newY - gcY;
    origin_gcX = newX - gcX;
    origin_gcY = newX - gcY;
}

float TriggerVis::getX()
{
    return gcX;
}

float TriggerVis::getY()
{
    return gcY;
}

float TriggerVis::getOriginX()
{
    return origin_gcX;
}

float TriggerVis::getOriginY()
{
    return origin_gcY;
}

void TriggerVis::setX(int newX)
{
    updateTriggerOrigin(newX, gcY);
}

void TriggerVis::setY(int newY)
{
    updateTriggerOrigin(gcX, newY);
}

void TriggerVis::setOriginX(int newX)
{
    updateTriggerOrigin(newX, origin_gcY);
}

void TriggerVis::setOriginY(int newY)
{
    updateTriggerOrigin(origin_gcX, newY);
}

bool TriggerVis::changedGcX()
{
    return changed_gcX;
}

bool TriggerVis::changedGcY()
{
    return changed_gcY;
}

Trajectory *TriggerVis::getTrajectory()
{
    return this->myTrajectory;
}

bool TriggerVis::getIsMoving()
{
    return isMoving;
}

void TriggerVis::setTrajectory(Trajectory *tr)
{
    if ((isMoving || myTrajectory != nullptr))
    {
        delete myTrajectory;
    }
    myTrajectory = tr;
    if(tr == nullptr){
        stopTrajectory();
    }
}

void TriggerVis::stopTrajectory()
{
    isMoving = false;
}

void TriggerVis::startTrajectory()
{
    isMoving = true;
}

void TriggerVis::restartTrajectory()
{
    if (myTrajectory != nullptr) {
        myTrajectory->restart();
        restartingTrajectory = true;
        draw();
        restartingTrajectory = false;
    }
}

void TriggerVis::setRecordingTrajectory(bool l_state)
{
    Recorded *recTraj=dynamic_cast<Recorded*>(getTrajectory());
    recTraj->setRecording(l_state);
    recordingTrajectory = l_state;
}

bool TriggerVis::getRecordingTrajectory()
{
    return recordingTrajectory;
}

void TriggerVis::setRecordTrajectoryAsked(bool l_state)
{
    recordTrajectoryAsked = l_state;
}

bool TriggerVis::getRecordTrajectoryAsked()
{
    return recordTrajectoryAsked;
}

bool TriggerVis::hasTrajectory()
{
    return myTrajectory != nullptr;
}

void TriggerVis::setState(bool l_state)
{
    isOn = l_state;
}

void TriggerVis::setSelectState(bool state)
{
    isSelected = state;
}

bool TriggerVis::select(float x, float y)
{
    float xdiff = x - origin_gcX;
    float ydiff = y - origin_gcY;

    if (sqrt(xdiff * xdiff + ydiff * ydiff) < maxSelRad)
        return true;
    else
        return false;
}

float TriggerVis::getZoneExtent()
{
    return zoneExtent;
}

void TriggerVis::setZoneExtent(float mouseX, float mouseY)
{
    //cout << " trigger seytzone extent" << endl;
    if ((mouseX - gcX) > (mouseY - gcY))
        zoneExtent = fabs(mouseX - gcX);
    else
        zoneExtent = fabs(mouseY - gcY);
    if (zoneExtent < 2.0f)
        zoneExtent = 0.0f;
    //cout << "zoneextent = " << zoneExtent << endl;
}

bool TriggerVis::changedZoneExtent()
{
    return changed_zoneExtent;
}

void TriggerVis::setFixedZoneExtent(float X, float Y)
{
    if (X > Y)
        zoneExtent = X;
    else
        zoneExtent = Y;
    changed_zoneExtent = true;
}

void TriggerVis::describe(ostream &out)
{
    out << "- X : " << getX() << "\n";
    out << "- Y : " << getY() << "\n";
    out << "- extent : " << getZoneExtent() << "\n";
}

void TriggerVis::registerTrigger(Trigger *triggerToRegister)
{
    myTrigger = triggerToRegister;
}

void TriggerVis::changesDone(bool done)
{
    changed_gcX= done;
    changed_gcY = done;
    changed_zoneExtent = done;
}

bool TriggerVis::getIsPlayed()
{
    return isPlayed;
}

void TriggerVis::setIsPlayed(bool l_isPlayed)
{
    isPlayed = l_isPlayed;
    if (myTrajectory != nullptr)
        if ((l_isPlayed) && (myTrigger->getActiveRestartTrajectory())) {
            stopTrajectory();
            restartTrajectory();
            startTrajectory();
        }
}

void TriggerVis::setTrajectoryType(int l_trajectoryType)
{
    trajectoryType = l_trajectoryType;
}

int TriggerVis::getTrajectoryType()
{
    return trajectoryType;
}

void TriggerVis::trajectoryAddPosition(int l_x, int l_y)
{
    Recorded *recTraj=dynamic_cast<Recorded*>(getTrajectory());
    recTraj->addPosition(l_x, l_y);
}

void TriggerVis::trajectoryAddPositionDelayed(int l_x, int l_y, double l_delay)
{
    Recorded *recTraj=dynamic_cast<Recorded*>(getTrajectory());
    recTraj->addPositionDelayed(l_x, l_y, l_delay);
}

void TriggerVis::trajectoryChangeSpeed(double newValue)
{
    getTrajectory()->setSpeed(newValue);
}

void TriggerVis::trajectoryChangeProgress(double newValue)
{
    getTrajectory()->setProgress(newValue);
}

void TriggerVis::trajectoryChangeRadius(double newValue)
{
    getTrajectory()->setRadius(newValue);
}

void TriggerVis::trajectoryChangeAngle(double newValue)
{
    getTrajectory()->setAngle(newValue);
}

void TriggerVis::trajectoryChangeStretch(double newValue)
{
    getTrajectory()->setStretch(newValue);
}

void TriggerVis::trajectoryChangeRadiusInt(double newValue)
{
    getTrajectory()->setRadiusInt(newValue);
}

void TriggerVis::trajectoryChangeExpansion(double newValue)
{
    getTrajectory()->setExpansion(newValue);
}
