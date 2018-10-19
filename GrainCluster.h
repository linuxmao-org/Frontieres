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
//  GrainCluster.h
//  Frontières
//
//  Created by Christopher Carlson on 11/15/11.
//

#ifndef GRAIN_CLUSTER_H
#define GRAIN_CLUSTER_H

#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <mutex>
#include <cstdlib>
#include <time.h>
#include <ctime>
#include <Stk.h>

#include "GrainVoice.h"
#include "theglobals.h"
#include "Window.h"
#include "SoundRect.h"

// direction modes
enum { FORWARD, BACKWARD, RANDOM_DIR };

// spatialization modes
enum {
    UNITY,
    STEREO,
    AROUND
};  // eventually include channel list specification and VBAP?

using namespace std;


// forward declarations
class GrainCluster;
class GrainClusterVis;

// ids
static unsigned int clusterId = 0;


// class interface
class GrainCluster {

public:
    // destructor
    virtual ~GrainCluster();

    // constructor
    GrainCluster(vector<AudioFile *> *soundSet, float theNumVoices);

    // compute next buffer of audio (accumulate from grains)
    void nextBuffer(double *accumBuff, unsigned int numFrames);

    // CLUSTER PARAMETER accessors/mutators
    // set duration for all grains
    void setDurationMs(float theDur);
    float getDurationMs();


    // overlap
    void setOverlap(float targetOverlap);
    float getOverlap();

    // pitch
    void setPitch(float targetPitch);
    float getPitch();

    // pitch lfo methods
    void setPitchLFOFreq(float pfreq);
    float getPitchLFOFreq();
    void setPitchLFOAmount(float lfoamt);
    float getPitchLFOAmount();

    // direction
    void setDirection(int dirMode);
    int getDirection();

    // add/remove grain voice
    void addGrain();
    void removeGrain();

    // set window type
    void setWindowType(int windowType);
    int getWindowType();


    // spatialization methods (see enum for theMode.  channel number is optional and has default arg);
    void setSpatialMode(int theMode, int channelNumber);
    int getSpatialMode();
    int getSpatialChannel();

    // volume
    void setVolumeDb(float theVolDB);
    float getVolumeDb();


    // get unique id of grain cluster
    unsigned int getId();

    // register visualization
    void registerVis(GrainClusterVis *myVis);

    // turn on/off
    void toggleActive();
    void setActiveState(bool activateState);
    bool getActiveState();


    // return number of voices
    unsigned int getNumVoices();

    // print information
    void describe(std::ostream &out);

protected:
    // update internal trigger point
    void updateBangTime();

    // spatialization - get new channel multiplier buffer to pass to grain voice instance
    void updateSpatialization();

private:
    unsigned int myId;  // unique id

    bool isActive;  // on/off state
    bool awaitingPlay;  // triggered but not ready to play?
    bool addFlag, removeFlag;  // add/remove requests submitted?
    unsigned long local_time;  // internal clock
    double startTime;  // instantiation time
    double bang_time;  // trigger time for next grain
    unsigned int nextGrain;  // grain voice index

    // spatialization vars
    int currentAroundChan;
    int stereoSide;
    int side;


    // thread safety
    std::mutex myLock;

    // registered visualization
    GrainClusterVis *myVis;

    // spatialization
    double *channelMults;
    int spatialMode;
    int channelLocation;

    // volume
    float volumeDb, normedVol;

    // vector of grains
    vector<GrainVoice *> myGrains;

    // number of grains in this cluster
    unsigned int numVoices;

    // cluster params
    float overlap, overlapNorm, pitch, duration, pitchLFOFreq, pitchLFOAmount;
    int myDirMode, windowType;

    // audio files
    vector<AudioFile *> *theSounds;
};


// VISUALIZATION/CONTROLLER
class GrainClusterVis {
public:
    // destructor
    ~GrainClusterVis();

    // constructor (takes center position (x,y), number of voices, sound rectangles)
    GrainClusterVis(float x, float y, unsigned int numVoices, vector<SoundRect *> *rects);

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

    // grain voice visualizations
    vector<GrainVis *> myGrainsV;
    // registered sound rectangles
    vector<SoundRect *> *theLandscape;
};


#endif
