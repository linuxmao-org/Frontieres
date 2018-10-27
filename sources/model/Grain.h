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
//  Grain.h
//  Frontières
//
//  Created by Christopher Carlson on 11/13/11.
//

#ifndef GRAIN_H
#define GRAIN_H

#include <vector>
#include <memory>
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
class AudioFile;
class Grain;
class GrainVis;
class SceneSound;

typedef std::vector<std::unique_ptr<SceneSound>> VecSceneSound;


// AUDIO CLASS
class Grain {

public:
    // destructor
    virtual ~Grain();

    // constructor
    Grain(VecSceneSound *soundSet, float durationMs, float thePitch);

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

    // update after a change of sound set
    void updateSoundSet();

protected:
    // makes temp  params permanent
    void updateParams();

private:
    // pointer to all audio file buffers
    VecSceneSound *theSounds;
    // status
    bool playingState;
    // param update required flag
    bool newParam;

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
    vector<int> activeSounds;

    // window type
    unsigned int windowType, queuedWindowType;

    // window reading params
    double winInc;
    double winReader;

    // pointer to audio window (hanning, triangle, etc.)
    double *window;


    // array of position values (in frames, not samples)
    //-1 means not in current soundfile
    std::unique_ptr<double[]> playPositions;
    std::unique_ptr<double[]> playVols;
};

#endif
