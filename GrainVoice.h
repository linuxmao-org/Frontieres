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
//  GrainVoice.h
//  Frontières
//
//  Created by Christopher Carlson on 11/13/11.
//

#ifndef GRAINVOICE_H
#define GRAINVOICE_H

#include "theglobals.h"
#include "AudioFileSet.h"
#include "Window.h"
#include <vector>
#include <math.h>
#include <time.h>
#include <ctime>
#include <Stk.h>

#ifdef __MACOSX_CORE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif


// forward declarations
class GrainVoice;
class GrainVis;


// AUDIO CLASS
class GrainVoice {

public:
    // destructor
    virtual ~GrainVoice();

    // constructor
    GrainVoice(vector<AudioFile *> *soundSet, float durationMs, float thePitch);

    // dump samples into next buffer
    void nextBuffer(double *accumBuff, unsigned int numFrames,
                    unsigned int bufferPos, int name);


    // set on
    bool playMe(double *startPositions, double *startVols);

    // report state
    bool isPlaying();

    // queue up params for next grain
    void setDurationMs(float dur);

    // set/get playback rate
    void setPitch(float newPitch);

    // get playback rate
    float getPitch();

    // volume
    void setVolume(float theVolNormed);
    float getVolume();

    // set spatialization
    void setChannelMultipliers(double *multipliers);

    // set playback direction
    void setDirection(float thedir);

    // change window type
    void setWindow(unsigned int windowType);


protected:
    // makes temp  params permanent
    void updateParams();

private:
    // pointer to all audio file buffers
    vector<AudioFile *> *theSounds;
    // status
    bool playingState;
    // param update required flag
    bool newParam;

    // numsounds
    unsigned int numSounds;

    // grain parameters
    float duration, queuedDuration;
    double winDurationSamps;
    double pitch, queuedPitch;
    double direction, queuedDirection;
    double playInc;

    // local volume (set by user)
    float localAtten;
    float queuedLocalAtten;

    // panning values
    double *chanMults;
    double *queuedChanMults;

    // audio files being sampled
    vector<int> *activeSounds;

    // window type
    unsigned int windowType, queuedWindowType;

    // window reading params
    double winInc;
    double winReader;

    // pointer to audio window (hanning, triangle, etc.)
    double *window;


    // array of position values (in frames, not samples)
    //-1 means not in current soundfile
    double *playPositions;
    double *playVols;
};


// GRAPHICS CLASS
// handle display, picking.
// register selection listener
class GrainVis {
public:
    // destructor
    ~GrainVis();
    // constructor
    GrainVis(float x, float y);
    void draw();
    // position update function
    void moveTo(float x, float y);
    float getX();
    float getY();
    void trigger(float theDur);

private:
    bool isOn, firstTrigger;
    double startTime;
    double triggerTime;
    float gX, gY;
    float colR, colG, colB, colA;
    float defG, defB;
    float mySize, defSize, onSize;  // GL point size
    float durSec;
};


#endif
