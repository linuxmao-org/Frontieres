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
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QTranslator>
#include "model/Adsr.h"
#include "dsp/Window.h"

class Grain;
struct SceneSample;

typedef std::vector<std::unique_ptr<SceneSample>> VecSceneSample;

int const g_maxMidiVoices(32);

// direction modes
enum { FORWARD, BACKWARD, RANDOM_DIR };

// spatialization modes
enum {
    UNITY,
    STEREO,
    AROUND
};  // eventually include channel list specification and VBAP?

enum EnvelopeAction { TriggerEnvelope = 1, ReleaseEnvelope = 2 };

using namespace std;


// forward declarations
class Cloud;
class CloudVis;

// midi polyphonic

class CloudMidi {
public:
    // destructor
    ~CloudMidi();

    // constructor
    CloudMidi();
    // cloud parameters duplicated for each midi note
    int midiNote = - 1;
    float pitch;
    bool isActive = false;
    int velocity;
    float *envelopeVolumeBuff;
//    double *intermediateBuff;
    std::atomic<int> envelopeAction;
    Env *envelopeVolume;
};

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
    bool changedDurationMs();


    // overlap
    void setOverlap(float targetOverlap);
    float getOverlap();
    bool changedOverlap();

    // pitch
    void setPitch(float targetPitch);
    float getPitch();
    bool changedPitch();

    // pitch lfo methods
    void setPitchLFOFreq(float pfreq);
    float getPitchLFOFreq();
    bool changedPitchLFOFreq();
    void setPitchLFOAmount(float lfoamt);
    float getPitchLFOAmount();
    bool changedPitchLFOAmount();

    // direction
    void setDirection(int dirMode);
    int getDirection();
    bool changedDirection();

    // add/remove grain
    void addGrain();
    void removeGrain();
    bool changedNumGrains();

    // set window type
    void setWindowType(int windowType);
    int getWindowType();
    bool changedWindowType();


    // spatialization methods (see enum for theMode.  channel number is optional and has default arg);
    void setSpatialMode(int theMode, int channelNumber);
    int getSpatialMode();
    int getSpatialChannel();
    bool changedSpatialMode();

    // volume
    void setVolumeDb(float theVolDB);
    float getVolumeDb();
    bool changedVolumeDb();


    // get unique id of cloud
    unsigned int getId();
    void setId(int cloudId);

    void setName(QString newName);
    QString getName();

    // register visualization
    void registerCloudVis(CloudVis *cloudVisToRegister);

    // turn on/off
    void toggleActive();
    void setActiveState(bool activateState);
    void setActiveMidiState(bool activateMidiState, int l_midiNote, int l_midiVelo);
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

    // midi notes
    void setMidiChannel(int newMidiChannel);
    void setMidiNote(int newMidiNote);
    void setMidiVelocity(int newMidiVelocity);
    int getMidiChannel();
    int getMidiNote();
    int getMidiVelocity();
    bool changedMidiChannel();
    bool changedMidiNote();

    // lock flag
    void setLockedState(bool newLockedState);
    bool getLockedState();
    bool dialogLocked();

    // changes done
    void changesDone(bool done);

    // message when osc or midi want to change locked cloud
    void showMessageLocked();

protected:
    // update internal trigger point
    void updateBangTime();

    // spatialization - get new channel multiplier buffer to pass to grain instance
    void updateSpatialization();

private:
    unsigned int myId;  // unique id

    QString myName;

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
    bool changed_spatialMode = false;
    int channelLocation;

    // volume
    float volumeDb, normedVol;
    bool changed_volumeDB = false;
    float *envelopeVolumeBuff;
    double *intermediateBuff;
    //enum EnvelopeAction { TriggerEnvelope = 1, ReleaseEnvelope = 2 };
    std::atomic<int> envelopeAction;


    // vector of grains
    vector<Grain *> myGrains;
    // number of grains in this cloud
    unsigned int numGrains;
    bool changed_numGrains = false;

    // cloud params
    float overlap, overlapNorm, pitch, duration, pitchLFOFreq, pitchLFOAmount;
    int myDirMode, windowType;
    bool changed_overlap = false;
    bool changed_pitch = false;
    bool changed_duration = false;
    bool changed_pitchLFOFreq = false;
    bool changed_pitchLFOAmount = false;
    bool changed_myDirMode = false;
    bool changed_windowType = false;

    // audio files
    VecSceneSample *theSamples;

    // envelope
    Env *envelopeVolume;

    // midi params
    int midiChannel = 0;
    int midiNote;
    bool changed_midiChannel = false;
    bool changed_midiNote = false;
    int midiVelocity = 127;

    // lock switch
    bool locked = false;

    // midi polyphony
    CloudMidi playedCloudMidi[g_maxMidiVoices];
    int actualyPlayedMidi = 0;
    void deletePlayedCloudMidi(int l_numCloudMidi);

};

#endif
