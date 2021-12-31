//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2011  Christopher Carlson
// Copyright (C) 2020  Olivier Flatres
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


// forward declarations
class Sample;
class SampleVis;
class Grain;
class GrainVis;
class SceneSample;

typedef std::vector<std::unique_ptr<SceneSample>> VecSceneSample;


// AUDIO CLASS
class Grain {

public:
    // destructor
    virtual ~Grain();

    // constructor
    Grain(VecSceneSample *sampleSet, float durationMs, float thePitch);

    // dump samples into next buffer
    void nextBuffer(double *accumBuff, unsigned int numFrames,
                    unsigned int bufferPos, int name);

    void nextNewBuffer(double *accumBuff, unsigned int numFrames,
                    int bufferPos, int name);

    // set on
    bool playMe(double *startPositions, double *startVols);

    // report state
    bool isPlaying();
    void setPlayingState(bool n_playingState);

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
    void setWindowFirstTime(bool n_firstTime);

    // update after a change of sample set
    void updateSampleSet();

    //
    void updateWave(unsigned int numFrames);

    void setFristGrain(bool n_firstGrain);

    bool getNewPlayingState();
    void setNewPlayingState(bool n_newPlayingState);

    unsigned int getCptPlaying ();
    void setCptPlaying(unsigned int n_CptPlaying);
    void incCptPlaying();


    void setNewPlayPos(int n_playX, int n_playY);

protected:
    // makes temp  params permanent
    void updateParams();

private:
    // pointer to all audio file buffers
    VecSceneSample *theSamples;
    // status
    bool playingState;
    // param update required flag
    bool newParam;

    // grain parameters
    double duration, queuedDuration;
    double winDurationSamps;
    double pitch, queuedPitch, pitchCent;
    double direction, queuedDirection;
    double playInc;

    // local volume (set by user)
    double localAtten;
    double queuedLocalAtten;

    // panning values
    double *chanMults;
    double *queuedChanMults;

    // audio files being sampled
    vector<int> activeSamples;

    // window type
    unsigned int windowType, queuedWindowType;

    // window reading params
    double winInc;
    double winReader;

    // pointer to audio window (hanning, triangle, etc.)
    double *window;
    bool windowFirstTime = false;


    // array of position values (in frames, not samples)
    //-1 means not in current samplefile
    std::unique_ptr<double[]> playPositions;
    std::unique_ptr<double[]> playVols;

    bool firstGrain = false;

    int playX, playY, nextX, nextY;
    bool newPlayingState = false;
    unsigned int cptPlaying = 0;
    unsigned int maxCptPlaying = 0;
    int newPlayingPos = 0;
};

#endif
