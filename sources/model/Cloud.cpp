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
//  Cloud.cpp
//  Frontières
//
//  Created by Christopher Carlson on 11/23/11.
//

#include "model/Cloud.h"
#include "model/Grain.h"
#include "model/ParamCloud.h"
#include "visual/CloudVis.h"
#include "utility/GTime.h"
#include <QMessageBox>

extern unsigned int samp_rate;
extern unsigned int g_buffSize;
extern CloudParams g_defaultCloudParams;

// ids
static unsigned int cloudId = 0;
// Destructor
Cloud::~Cloud()
{
    delete envelopeVolume;
    delete[] envelopeVolumeBuff;
    delete[] intermediateBuff;
    for (int i = 0; i < myGrains.size(); i++) {
        delete myGrains[i];
    }

    if (channelMults)
        delete[] channelMults;
}


// Constructor
Cloud::Cloud(VecSceneSample *sampleSet, float theNumGrains)
{
    // cloud id
    myId = ++cloudId;

    // playback bool to make sure we precisely time grain triggers
    awaitingPlay = false;

    // number of grains
    numGrains = g_defaultCloudParams.numGrains;
    // initialize random number generator (for random motion)
    srand(time(NULL));

    // initialize interfacing flags
    addFlag = false;
    removeFlag = false;

    // keep pointer to the sample set
    theSamples = sampleSet;

    // trigger idx
    nextGrain = 0;

    // intialize timer
    local_time = 0;

    // default duration (ms)
    //duration = 500.0;
    duration = g_defaultCloudParams.duration;

    // default pitch
    //pitch = 1.0f;
    pitch = g_defaultCloudParams.pitch;

    // default window type
    //windowType = HANNING;
    windowType = g_defaultCloudParams.windowType;

    // initialize pitch LFO
    //pitchLFOFreq = 0.01f;
    pitchLFOFreq = g_defaultCloudParams.pitchLFOFreq;
    //pitchLFOAmount = 0.0f;
    pitchLFOAmount = g_defaultCloudParams.pitchLFOAmount;
    // initialize channel multiplier array
    channelMults = new double[MY_CHANNELS];
    for (int i = 0; i < MY_CHANNELS; i++) {
        channelMults[i] = 0.999f;
    }

    currentAroundChan = 1;
    stereoSide = 0;
    side = 1;


    //spatialMode = UNITY;
    spatialMode = g_defaultCloudParams.spatialMode;
    //channelLocation = -1;
    channelLocation = g_defaultCloudParams.channelLocation;
    //myDirMode = RANDOM_DIR;
    myDirMode = g_defaultCloudParams.dirMode;

    // populate grain cloud
    for (int i = 0; i < numGrains; i++) {
        myGrains.push_back(new Grain(theSamples, duration, pitch));
    }

    // set volume of cloud to unity
    //setVolumeDb(0.0);
    setVolumeDb(g_defaultCloudParams.volumeDB);
    envelopeVolume = new Env;
    envelopeVolume->setParam(g_defaultCloudParams.envelope);
    envelopeVolumeBuff = new float[g_buffSize];
    intermediateBuff = new double[g_buffSize * MY_CHANNELS];

    // initialize the envelope trigger flag
    envelopeAction.store(0);

    // set overlap (default to full overlap)
    //setOverlap(1.0f);
    setOverlap(g_defaultCloudParams.overlap);

    //    //direction
    //    setDirection(RANDOM_DIR);

    // initialize trigger time (samples)
    bang_time = duration * ::samp_rate * (double)0.001 / overlap;

    // load grains
    for (int i = 0; i < myGrains.size(); i++) {
        myGrains[i]->setDurationMs(duration);
    }

    // state - (user can remove cloud from "play" for editing)
    //isActive = true;
    setActiveState(g_defaultCloudParams.activateState);
}
// register controller for communication with view
void Cloud::registerCloudVis(CloudVis *cloudVisToRegister)
{
    myCloudVis = cloudVisToRegister;
    myCloudVis->setDuration(duration);
}

// turn on/off
void Cloud::toggleActive()
{
    setActiveState(!isActive);
}

void Cloud::setActiveState(bool activateState)
{
    if (activateState)
       envelopeAction.store(TriggerEnvelope);
    else
       envelopeAction.store(ReleaseEnvelope);
    isActive = activateState;
}

bool Cloud::getActiveState()
{
    return isActive;
}


// set window type
void Cloud::setWindowType(int winType)
{
    if (locked){
        showMessageLocked();
        return;
    }
    int numWins = Window::Instance().numWindows();
    windowType = winType % numWins;

    if (windowType < 0) {
        windowType = Window::Instance().numWindows() - 1;
    }
    if (windowType == RANDOM_WIN) {
        for (int i = 0; i < myGrains.size(); i++) {
            myGrains[i]->setWindow(
                (int)floor(randf() * Window::Instance().numWindows() - 1));
        }
    }
    else {

        for (int i = 0; i < myGrains.size(); i++) {
            // cout << "windowtype " << windowType << endl;
            myGrains[i]->setWindow(windowType);
        }
    }
    changed_windowType = true;
}

int Cloud::getWindowType()
{
    return windowType;
}

bool Cloud::changedWindowType()
{
    return changed_windowType;
}

void Cloud::addGrain()
{
    addFlag = true;
    myCloudVis->addGrain();
    changed_numGrains = true;
}

void Cloud::removeGrain()
{
    removeFlag = true;
    myCloudVis->removeGrain();
    changed_numGrains = true;
}

bool Cloud::changedNumGrains()
{
    return changed_numGrains;changedDirection();
}

// return id for grain
unsigned int Cloud::getId()
{
    return myId;
}

void Cloud::setId(int cloudId)
{
    myId = cloudId;
}

// overlap (input on 0 to 1 scale)
void Cloud::setOverlap(float target)
{
    if (locked){
        showMessageLocked();
        return;
    }
    if (target > 1.0f)
        target = 1.0f;
    else if (target < 0.0f)
        target = 0.0f;
    overlapNorm = target;
    // oops wrong!//overlap = ((float)(myGrains->size()))*0.25f*exp(log(2.0f)*target);

    float num = (float)myGrains.size();

    overlap = exp(log(num) * target);

    //  cout<<"overlap set" << overlap << endl;
    updateBangTime();
    changed_overlap = true;
}

float Cloud::getOverlap()
{
    return overlapNorm;
}

bool Cloud::changedOverlap()
{
    return changed_overlap;
}

// duration
void Cloud::setDurationMs(float theDur)
{
    if (locked){
        showMessageLocked();
        return;
    }
    if (theDur >= 1.0f) {
        duration = theDur;
        for (int i = 0; i < myGrains.size(); i++)
            myGrains[i]->setDurationMs(duration);

        updateBangTime();

        // notify visualization
        if (myCloudVis)
            myCloudVis->setDuration(duration);
        changed_duration = true;
    }
}

// update internal grain trigger time
void Cloud::updateBangTime()
{
    bang_time = duration * ::samp_rate * (double)0.001 / overlap;
    // cout << "duration: " << duration << ", new bang time " << bang_time << endl;
}


// pitch
void Cloud::setPitch(float targetPitch)
{
    if (locked){
        showMessageLocked();
        return;
    }
    if (targetPitch < 0.0001) {
        targetPitch = 0.0001;
    }
    pitch = targetPitch;
    for (int i = 0; i < myGrains.size(); i++)
        myGrains[i]->setPitch(targetPitch);
    changed_pitch = true;
}

float Cloud::getPitch()
{
    return pitch;
}

bool Cloud::changedPitch()
{
    return changed_pitch;
}


//-----------------------------------------------------------------
// Cloud volume
//-----------------------------------------------------------------
void Cloud::setVolumeDb(float volDb)
{
    if (locked){
        showMessageLocked();
        return;
    }
    // max = 6 db, min = -60 db
    if (volDb > 6.0) {
        volDb = 6.0;
    }

    if (volDb < -60.0) {
        volDb = -60.0;
    }

    volumeDb = volDb;

    // convert to 0-1 representation
    normedVol = pow(10.0, volDb * 0.05);

    for (int i = 0; i < myGrains.size(); i++)
        myGrains[i]->setVolume(normedVol);
    changed_volumeDB = true;
}

float Cloud::getVolumeDb()
{
    return volumeDb;
}

bool Cloud::changedVolumeDb()
{
    return changed_volumeDB;
}


// direction mode
void Cloud::setDirection(int dirMode)
{
    if (locked){
        showMessageLocked();
        return;
    }
    myDirMode = dirMode % 3;
    if (myDirMode < 0) {
        myDirMode = 2;
    }
    // cout << "dirmode num" << myDirMode << endl;
    switch (myDirMode) {
    case FORWARD:
        //      cout << "set for" << endl;
        for (int i = 0; i < myGrains.size(); i++)
            myGrains[i]->setDirection(1.0);
        break;
    case BACKWARD:
        //    cout << "set back" << endl;
        for (int i = 0; i < myGrains.size(); i++)
            myGrains[i]->setDirection(-1.0);
        break;
    case RANDOM_DIR:
        for (int i = 0; i < myGrains.size(); i++) {
            if (randf() > 0.5)
                myGrains[i]->setDirection(1.0);
            else
                myGrains[i]->setDirection(-1.0);
        }

    default:
        break;
    }
    changed_myDirMode = true;
}


// return grain direction int (see enum.  currently, 0 = forward, 1 = back, 2 = random)
int Cloud::getDirection()
{
    return myDirMode;
}

bool Cloud::changedDirection()
{
    return changed_myDirMode;
}

// return duration in ms
float Cloud::getDurationMs()
{
    return duration;
}

bool Cloud::changedDurationMs()
{
    return changed_duration;
}


// return number of grains in this cloud
unsigned int Cloud::getNumGrains()
{
    return myGrains.size();
}

void Cloud::setNumGrains(unsigned int newNumGrains)
{
    if (locked){
        showMessageLocked();
        return;
    }
    if (newNumGrains < myGrains.size())
        for (int i=1; i <= (myGrains.size() - newNumGrains); ++i)
            removeGrain();
    else
        for (int i=1; i <= (newNumGrains - myGrains.size()); ++i)
            addGrain();
}

// update after a change of sample set
void Cloud::updateSampleSet()
{
    for (Grain *grain : myGrains)
        grain->updateSampleSet();
}

void Cloud::setEnvelopeVolumeParam (ParamEnv envelopeVolumeParamToSet)
{
    if (locked){
        showMessageLocked();
        return;
    }
    envelopeVolume->setParam(envelopeVolumeParamToSet);
}

ParamEnv Cloud::getEnvelopeVolumeParam ()
{
    return envelopeVolume->getParam();
}

void Cloud::setMidiChannel(int newMidiChannel)
{
    if (locked){
        showMessageLocked();
        return;
    }
    midiChannel = newMidiChannel;
    changed_midiChannel = true;
}

void Cloud::setMidiNote(int newMidiNote)
{
    if (locked){
        showMessageLocked();
        return;
    }
    midiNote = newMidiNote;
    changed_midiNote = true;
}

void Cloud::setMidiVelocity(int newMidiVelocity)
{
    midiVelocity = newMidiVelocity;
}

int Cloud::getMidiChannel()
{
    return midiChannel;
}

int Cloud::getMidiNote()
{
    return midiNote;
}

int Cloud::getMidiVelocity()
{
    return midiVelocity;
}

bool Cloud::changedMidiChannel()
{
    return changed_midiChannel;
}

bool Cloud::changedMidiNote()
{
    return changed_midiNote;
}

void Cloud::setLockedState(bool newLockedState)
{
    locked = newLockedState;
}

bool Cloud::getLockedState()
{
    return locked;
}

bool Cloud::dialogLocked()
{
    if (!locked)
        return false;

    QMessageBox msgBox;
    msgBox.setText("This cloud is locked, impossible to change parameters.");
    msgBox.setInformativeText("Do you want to unlock the cloud ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    locked = msgBox.exec() == QMessageBox::No;
    return locked;
}

void Cloud::changesDone(bool done)
{
    changed_duration = done;
    changed_midiChannel = done;
    changed_midiNote = done;
    changed_numGrains = done;
    changed_overlap = done;
    changed_pitch = done;
    changed_pitchLFOAmount = done;
    changed_pitchLFOFreq = done;
    changed_volumeDB = done;
    changed_windowType = done;
    changed_myDirMode = done;
    changed_spatialMode = done;
}

void Cloud::showMessageLocked()
{
    // cout << "cloud locked, no change" << endl;
}

// print information
void Cloud::describe(std::ostream &out)
{
    out << "- duration : " << getDurationMs() << "\n";
    out << "- overlap : " << getOverlap() << "\n";
    out << "- pitch : " << getPitch() << "\n";
    out << "- pitch LFO Freq : " << getPitchLFOFreq() << "\n";
    out << "- pitch LFO Amount : " << getPitchLFOAmount() << "\n";
    out << "- direction : " << getDirection() << "\n";
    out << "- window type : " << getWindowType() << "\n";
    out << "- spatial mode : " << getSpatialMode() << "\n";
    out << "- spatial channel : " << getSpatialChannel() << "\n";
    out << "- midi channel : " << getMidiChannel() << "\n";
    out << "- midi note : " << getMidiNote() << "\n";
    out << "- volume DB : " << getVolumeDb() << "\n";
    out << "- volume envelope L1 : " << getEnvelopeVolumeParam().l1 << "\n";
    out << "- volume envelope L2 : " << getEnvelopeVolumeParam().l2 << "\n";
    out << "- volume envelope L3 : " << getEnvelopeVolumeParam().l3 << "\n";
    out << "- volume envelope R1 : " << getEnvelopeVolumeParam().r1 << "\n";
    out << "- volume envelope R2 : " << getEnvelopeVolumeParam().r2 << "\n";
    out << "- volume envelope R3 : " << getEnvelopeVolumeParam().r3 << "\n";
    out << "- volume envelope R4 : " << getEnvelopeVolumeParam().r4 << "\n";
    out << "- volume envelope T1 : " << getEnvelopeVolumeParam().t1 << "\n";
    out << "- volume envelope T2 : " << getEnvelopeVolumeParam().t2 << "\n";
    out << "- volume envelope T3 : " << getEnvelopeVolumeParam().t3 << "\n";
    out << "- volume envelope T4 : " << getEnvelopeVolumeParam().t4 << "\n";
    out << "- number of grains : " << getNumGrains() << "\n";
    out << "- active : " << getActiveState() << "\n";
}

void Cloud::setEnvelopeVolume(Env envelopeVolumeToSet)
{
    envelopeVolume = &envelopeVolumeToSet;
}

Env Cloud::getEnvelopeVolume()
{
    return *envelopeVolume;
}


// compute audio
void Cloud::nextBuffer(double *accumBuff, unsigned int numFrames)
{
    int envelopeAction = this->envelopeAction.exchange(0);

    switch (envelopeAction) {
    case TriggerEnvelope:
        envelopeVolume->trigger();
        break;
    case ReleaseEnvelope:
        envelopeVolume->release();
        break;
    default:
        break;
    }
    envelopeVolume->generate(envelopeVolumeBuff, numFrames);
    if (addFlag == true) {
        addFlag = false;
        myGrains.push_back(new Grain(theSamples, duration, pitch));
        size_t idx = myGrains.size() - 1;
        myGrains[idx]->setWindow(windowType);
        switch (myDirMode) {
        case FORWARD:
            myGrains[idx]->setDirection(1.0);
            break;
        case BACKWARD:
            myGrains[idx]->setDirection(-1.0);
            break;
        case RANDOM_DIR:
            if (randf() > 0.5)
                myGrains[idx]->setDirection(1.0);
            else
                myGrains[idx]->setDirection(-1.0);
            break;

        default:
            break;
        }

        myGrains[idx]->setVolume(normedVol);
        numGrains += 1;
        setOverlap(overlapNorm);
    }

    if (removeFlag == true) {
        if (myGrains.size() > 1) {
            if (nextGrain >= myGrains.size() - 1) {
                nextGrain = 0;
            }
            myGrains.pop_back();
            setOverlap(overlapNorm);
        }
        removeFlag = false;
    }
    // end midi note, come back to velocity max
    if ((envelopeVolume->state() == Env::State::Off) && (midiVelocity != 127))
        setMidiVelocity(127);
    if (envelopeVolume->state() != Env::State::Off) {
        // initialize play positions array
        double playPositions[theSamples->size()];
        double playVols[theSamples->size()];

        // buffer variables
        unsigned int nextFrame = 0;

        // compute sub_buffers for reduced function calls
        int frameSkip = numFrames / 2;


        // fill buffer
        for (int j = 0; j < (numFrames / (frameSkip)); j++) {

            // check for bang
            if ((local_time > bang_time) || (awaitingPlay)) {

                // debug
                // cout << "bang " << nextGrain << endl;
                // reset local
                if (!awaitingPlay) {
                    local_time = 0;
                    // clear play and volume buffs
                    for (int i = 0; i < theSamples->size(); i++) {
                        playPositions[i] = (double)(-1.0);
                        playVols[i] = (double)0.0;
                    }
                    // TODO:  get position vector for grain with idx nextGrain from controller
                    // udate positions vector (currently randomized)q
                    if (myCloudVis)
                        myCloudVis->getTriggerPos(nextGrain, playPositions, playVols, duration);
                }

                // get next pitch (using LFO) -  eventually generalize to an applyLFOs method (if LFO control will be exerted over multiple params)
                if ((pitchLFOAmount > 0.0f) && (pitchLFOFreq > 0.0f)) {
                    float nextPitch =
                        fabs(pitch + pitchLFOAmount * sin(2 * PI * pitchLFOFreq *
                                                          GTime::instance().sec));
                    myGrains[nextGrain]->setPitch(nextPitch);
                }


                // update spatialization/get new channel multiplier set
                updateSpatialization();
                myGrains[nextGrain]->setChannelMultipliers(channelMults);

                // trigger grain
                awaitingPlay = myGrains[nextGrain]->playMe(playPositions, playVols);

                // only advance if next grain is playable.  otherwise, cycle
                // through again to wait for playback
                if (!awaitingPlay) {
                    // queue next grain for trigger
                    nextGrain++;
                    // wrap grain idx
                    if (nextGrain >= myGrains.size())
                        nextGrain = 0;
                }
                else {
                    // debug
                    // cout << "playback delayed "<< endl;
                }
            }
            // advance time
            local_time += frameSkip;

            // sample offset (1 sample at a time for now)
            nextFrame = j * frameSkip;
            // iterate over all grains
            //for (int k = 0; k < myGrains.size(); k++) {
             //  myGrains[k]->nextBuffer(accumBuff, frameSkip, nextFrame, k);
            //}
            // iterate over all grains
            memset(intermediateBuff, 0, numFrames * MY_CHANNELS * sizeof(intermediateBuff[0]));
            for (int k = 0; k < myGrains.size(); k++) {
                myGrains[k]->nextBuffer(intermediateBuff, frameSkip, nextFrame, k);
            }
            for (int i = 0; i < numFrames; ++i) {
                for (int j = 0; j < MY_CHANNELS; ++j)
                    accumBuff[i * MY_CHANNELS + j] += intermediateBuff[i * MY_CHANNELS + j] * envelopeVolumeBuff[i] * ((float) midiVelocity / 127);
            }
        }
    }
}


// pitch lfo methods
void Cloud::setPitchLFOFreq(float pfreq)
{
    if (locked){
        showMessageLocked();
        return;
    }
    pitchLFOFreq = fabs(pfreq);
    changed_pitchLFOFreq = true;
}

void Cloud::setPitchLFOAmount(float lfoamt)
{
    if (locked){
        showMessageLocked();
        return;
    }
    if (lfoamt < 0.0) {
        lfoamt = 0.0f;
    }
    pitchLFOAmount = lfoamt;
    changed_pitchLFOAmount = true;
}

float Cloud::getPitchLFOFreq()
{
    return pitchLFOFreq;
}

bool Cloud::changedPitchLFOFreq()
{
    return changed_pitchLFOFreq;
}

float Cloud::getPitchLFOAmount()
{
    return pitchLFOAmount;
}

bool Cloud::changedPitchLFOAmount()
{
    return changed_pitchLFOAmount;
}


// spatialization methods
void Cloud::setSpatialMode(int theMode, int channelNumber = -1)
{
    if (locked){
        showMessageLocked();
        return;
    }
    spatialMode = theMode % 3;
    if (spatialMode < 0) {
        spatialMode = 2;
    }
    // for positioning in a single audio channel. - not used currently
    // eventually swap out for azimuth instead of single channel
    if (channelNumber >= 0)
        channelLocation = channelNumber;
    changed_spatialMode = true;
}

int Cloud::getSpatialMode()
{
    return spatialMode;
}
int Cloud::getSpatialChannel()
{
    return channelLocation;
}

bool Cloud::changedSpatialMode()
{
    return changed_spatialMode;
}

// spatialization logic
void Cloud::updateSpatialization()
{

    // currently assumes orientation L: 0,2,4,...  R: 1,3,5, etc (interleaved)
    switch (spatialMode) {
    case UNITY:
        for (int i = 0; i < MY_CHANNELS; i++) {
            channelMults[i] = 0.999f;
        }
        break;
    case STEREO:

        if (stereoSide == 0) {  // left
            for (int i = 0; i < MY_CHANNELS; i++) {
                channelMults[i] = 0.0f;
                if ((i % 2) == 0)
                    channelMults[i] = 0.999f;
                else
                    channelMults[i] = 0.0f;
            }
            stereoSide = 1;
        }
        else {  // right
            for (int i = 0; i < MY_CHANNELS; i++) {
                channelMults[i] = 0.0f;
                if ((i % 2) == 0)
                    channelMults[i] = 0.0f;
                else
                    channelMults[i] = 0.999f;
            }
            stereoSide = 0;
        }
        break;
    case AROUND:
        for (int i = 0; i < MY_CHANNELS; i++) {
            channelMults[i] = 0;
        }

        // 1 3 5 7 6 4 2 0

        channelMults[currentAroundChan] = 0.999;
        currentAroundChan += side * 2;
        if ((currentAroundChan > MY_CHANNELS) || (currentAroundChan < 0)) {
            side = -1 * side;
            currentAroundChan += side * 3;
        }
        // currentAroundChan = currentAroundChan % MY_CHANNELS;
        break;

    default:
        break;
    }
}
