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
//  Cloud.h
//  Frontières
//
//  Created by Christopher Carlson on 11/15/11.
//

#ifndef CLOUD_H
#define CLOUD_H

#include "theglobals.h"
#include <Stk.h>
#include <map>
#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <atomic>
#include <cstdlib>
#include <time.h>
#include <ctime>
#include <QFile>
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <QTranslator>
#include "model/Adsr.h"
#include "dsp/Window.h"

class Grain;
struct SceneSample;

typedef std::vector<std::unique_ptr<SceneSample>> VecSceneSample;

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
class Cloud;
class CloudVis;
class CloudDialog;

// class interface
class Cloud {

public:
    // destructor
    virtual ~Cloud();

    // constructor
    Cloud(VecSceneSample *sampleSet, float theNumGrains);

    // compute next buffer of audio (accumulate from grains)
    void nextBuffer(double *accumBuff, unsigned int numFrames);

    // CLOUD PARAMETER accessors/mutators
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

    // add/remove grain
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


    // get unique id of cloud
    unsigned int getId();
    void setId(int cloudId);

    // register visualization
    void registerCloudVis(CloudVis *cloudVisToRegister);

    // turn on/off
    void toggleActive();
    void setActiveState(bool activateState);
    bool getActiveState();


    // number of Grains
    unsigned int getNumGrains();
    void setNumGrains(unsigned int newNumGrains);

    // update after a change of sample set
    void updateSampleSet();

    // print information
    void describe(std::ostream &out);

    // envelope
    void setEnvelopeVolume (Env envelopeVolumeToSet);
    Env getEnvelopeVolume ();
    void setEnvelopeVolumeParam (ParamEnv envelopeVolumeParamToSet);
    ParamEnv getEnvelopeVolumeParam ();

    // params dialog
    void setDialogExist(bool exist);
    bool getDialogExist();
    void showDialog(CloudVis *selectedCloudVis);
    void updateDialog();

    // midi notes
    void setMidiChannel(int newMidiChannel);
    void setMidiNote(int newMidiNote);
    int getMidiChannel();
    int getMidiNote();

protected:
    // update internal trigger point
    void updateBangTime();

    // spatialization - get new channel multiplier buffer to pass to grain instance
    void updateSpatialization();

private:
    unsigned int myId;  // unique id

    bool isActive;  // on/off state
    bool awaitingPlay;  // triggered but not ready to play?
    bool addFlag, removeFlag;  // add/remove requests submitted?
    unsigned long local_time;  // internal clock
    double startTime;  // instantiation time
    double bang_time;  // trigger time for next grain
    unsigned int nextGrain;  // grain index

    // spatialization vars
    int currentAroundChan;
    int stereoSide;
    int side;

    // registered visualization
    CloudVis *myCloudVis;

    // spatialization
    double *channelMults;
    int spatialMode;
    int channelLocation;

    // volume
    float volumeDb, normedVol;
    float *envelopeVolumeBuff;
    double *intermediateBuff;
    enum EnvelopeAction { TriggerEnvelope = 1, ReleaseEnvelope = 2 };
    std::atomic<int> envelopeAction;


    // vector of grains
    vector<Grain *> myGrains;

    // number of grains in this cloud
    unsigned int numGrains;

    // cloud params
    float overlap, overlapNorm, pitch, duration, pitchLFOFreq, pitchLFOAmount;
    int myDirMode, windowType;

    // audio files
    VecSceneSample *theSamples;

    // envelope
    Env *envelopeVolume;

    // params dialog
    bool dialogExist = false;
    CloudDialog *myCloudDialog;

    // midi params
    int midiChannel = 0;
    int midiNote;
};

#endif
