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
#include "visual/CloudVis.h"
#include "dsp/Window.h"
#include "utility/GTime.h"

extern unsigned int samp_rate;


// Destructor
Cloud::~Cloud()
{
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
    channelLocation = g_defaultCloudParams.chanelLocation;
    //myDirMode = RANDOM_DIR;
    myDirMode = g_defaultCloudParams.dirMode;

    // populate grain cloud
    for (int i = 0; i < numGrains; i++) {
        myGrains.push_back(new Grain(theSamples, duration, pitch));
    }

    // set volume of cloud to unity
    //setVolumeDb(0.0);
    setVolumeDb(g_defaultCloudParams.volumeDB);

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
    isActive = g_defaultCloudParams.activateState;
}
// register controller for communication with view
void Cloud::registerVis(CloudVis *vis)
{
    myVis = vis;
    myVis->setDuration(duration);
}

// turn on/off
void Cloud::toggleActive()
{
    isActive = !isActive;
}

void Cloud::setActiveState(bool activateState)
{
    isActive = activateState;
}

bool Cloud::getActiveState()
{
    return isActive;
}


// set window type
void Cloud::setWindowType(int winType)
{
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
}

int Cloud::getWindowType()
{
    return windowType;
}


void Cloud::addGrain()
{
    addFlag = true;
    myVis->addGrain();
}

void Cloud::removeGrain()
{
    removeFlag = true;
    myVis->removeGrain();
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
}

float Cloud::getOverlap()
{
    return overlapNorm;
}

// duration
void Cloud::setDurationMs(float theDur)
{
    if (theDur >= 1.0f) {
        duration = theDur;
        for (int i = 0; i < myGrains.size(); i++)
            myGrains[i]->setDurationMs(duration);

        updateBangTime();

        // notify visualization
        if (myVis)
            myVis->setDuration(duration);
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
    if (targetPitch < 0.0001) {
        targetPitch = 0.0001;
    }
    pitch = targetPitch;
    for (int i = 0; i < myGrains.size(); i++)
        myGrains[i]->setPitch(targetPitch);
}

float Cloud::getPitch()
{
    return pitch;
}


//-----------------------------------------------------------------
// Cloud volume
//-----------------------------------------------------------------
void Cloud::setVolumeDb(float volDb)
{
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
}

float Cloud::getVolumeDb()
{
    return volumeDb;
}


// direction mode
void Cloud::setDirection(int dirMode)
{
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
}


// return grain direction int (see enum.  currently, 0 = forward, 1 = back, 2 = random)
int Cloud::getDirection()
{
    return myDirMode;
}

// return duration in ms
float Cloud::getDurationMs()
{
    return duration;
}


// return number of grains in this cloud
unsigned int Cloud::getNumGrains()
{
    return myGrains.size();
}

// update after a change of sample set
void Cloud::updateSampleSet()
{
    for (Grain *grain : myGrains)
        grain->updateSampleSet();
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
    out << "- spatial chanel : " << getSpatialChannel() << "\n";
    out << "- volume DB : " << getVolumeDb() << "\n";
    out << "- number of grains : " << getNumGrains() << "\n";
    out << "- active : " << getActiveState() << "\n";
}


// compute audio
void Cloud::nextBuffer(double *accumBuff, unsigned int numFrames)
{

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

    if (isActive == true) {


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
                    if (myVis)
                        myVis->getTriggerPos(nextGrain, playPositions, playVols, duration);
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
            for (int k = 0; k < myGrains.size(); k++) {
                myGrains[k]->nextBuffer(accumBuff, frameSkip, nextFrame, k);
            }
        }
    }
}


// pitch lfo methods
void Cloud::setPitchLFOFreq(float pfreq)
{
    pitchLFOFreq = fabs(pfreq);
}

void Cloud::setPitchLFOAmount(float lfoamt)
{
    if (lfoamt < 0.0) {
        lfoamt = 0.0f;
    }
    pitchLFOAmount = lfoamt;
}

float Cloud::getPitchLFOFreq()
{
    return pitchLFOFreq;
}

float Cloud::getPitchLFOAmount()
{
    return pitchLFOAmount;
}


// spatialization methods
void Cloud::setSpatialMode(int theMode, int channelNumber = -1)
{
    spatialMode = theMode % 3;
    if (spatialMode < 0) {
        spatialMode = 2;
    }
    // for positioning in a single audio channel. - not used currently
    // eventually swap out for azimuth instead of single channel
    if (channelNumber >= 0)
        channelLocation = channelNumber;
}

int Cloud::getSpatialMode()
{
    return spatialMode;
}
int Cloud::getSpatialChannel()
{
    return channelLocation;
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
