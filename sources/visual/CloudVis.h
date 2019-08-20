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


#include <vector>
#include <memory>
#include <iostream>
#include "theglobals.h"
#include "Trajectory.h"
#include "Circular.h"
#include "Hypotrochoid.h"
#include "Recorded.h"

class SceneSample;
class GrainVis;
class Cloud;
class CloudMidi;

typedef std::vector<std::unique_ptr<SceneSample>> VecSceneSample;

// VISUALIZATION/CONTROLLER
class CloudVis {
public:
    // destructor
    ~CloudVis();

    // constructor (takes center position (x,y), number of grains, sample visuals)
    CloudVis(float x, float y, unsigned int numGrainsVis, VecSceneSample *rects, bool c_isMidiVis);

    CloudVis(float x, float y, unsigned int numGrainsVis, VecSceneSample *rects,Trajectory *trajectory);
    // render
    void draw();
    // get playback position in registered sample visuals and return to grain cloud
    void getTriggerPos(unsigned int idx, double *playPos, double *playVols, float dur);
    // move grains
    void updateCloudPosition(float newX, float newY);
    void updateCloudOrigin (float newOriginX, float newOriginY);
    void updateCloudTrajectoryPosition (float newX, float newY);
    void updateGrainPosition(int idx, float x, float y);
    void setState(bool l_state);
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
    // get my origin x coordinate
    float getOriginX();
    // get my origin y coordinate
    float getOriginY();
    void setX(int newX);
    void setY(int newY);
    void setOriginX(int newX);
    void setOriginY(int newY);
    bool changedGcX();
    bool changedGcY();


    // randomness params for grain positions
    float getXRandExtent();
    float getYRandExtent();
    void setXRandExtent(float mouseX);
    void setYRandExtent(float mouseY);
    void setRandExtent(float mouseX, float mouseY);
    void setFixedXRandExtent(float X);
    void setFixedYRandExtent(float Y);
    void setFixedRandExtent(float X, float Y);
    bool changedXRandExtent();
    bool changedYRandExtent();

    // set the pulse duration (which determines the frequency of the pulse)
    void setDuration(float dur);

    // print information
    void describe(std::ostream &out);

    // register model
    void registerCloud(Cloud *cloudToRegister);
    void registerCloudMidi(CloudMidi *cloudMidiToRegister);

    // changes done
    void changesDone(bool done);

    Trajectory* getTrajectory();
    bool getIsMoving();
    void setTrajectory(Trajectory *tr);
    void stopTrajectory();
    void startTrajectory();
    void restartTrajectory();
    void setRecordingTrajectory(bool l_state);
    bool getRecordingTrajectory();
    void setRecordTrajectoryAsked(bool l_state);
    bool getRecordTrajectoryAsked();
    bool hasTrajectory();
    bool getIsMidiVis();
    void setIsMidiVis(bool md);
    void activateMidiVis(int l_numNote, bool l_activate);
    bool getStateMidiVis(int l_numNote);
    CloudVis *getMidiCloudVis(int l_numNote);
    bool getIsPlayed();
    void setIsPlayed(bool l_isPlayed);
    void setTrajectoryType(int l_trajectoryType);
    int getTrajectoryType();
    void trajectoryAddPosition(int l_x, int l_y);

protected:
private:

    bool isMoving;
    bool isOn, isSelected;
    bool isMidiVis = false;
    bool isPlayed = false;
    bool addFlag, removeFlag;
    double startTime;
    double lastDrawTime;

    float xRandExtent, yRandExtent;
    bool changed_xRandExtent = false;
    bool changed_yRandExtent = false;

    float freq;
    float gcX, gcY;
    float origin_gcX, origin_gcY;
    bool changed_gcX = false;
    bool changed_gcY = false;
    float selRad, lambda, maxSelRad, minSelRad, targetRad;
    unsigned int numGrains;
    // registered visualization
    Cloud *myCloud;
    CloudMidi *myCloudMidi;
    // grain visualizations
    std::vector<GrainVis *> myGrainsV;
    // registered sample visuals
    VecSceneSample *theLandscape;

    // trajectory of the cloud
    int trajectoryType;
    Trajectory *myTrajectory;
    bool restartingTrajectory = false;
    bool recordingTrajectory = false;
    bool recordTrajectoryAsked = false;

    // midi polyphony
    CloudVis *playedCloudVisMidi[g_maxMidiVoices];
    bool isPlayedCloudVisMidi[g_maxMidiVoices];
};

#endif
