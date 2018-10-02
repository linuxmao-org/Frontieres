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
//  GrainCluster.cpp
//  Frontières
//
//  Created by Christopher Carlson on 11/23/11.
//

#include "GrainCluster.h"
#include "MyGLApplication.h"
#include "MyGLWindow.h"

extern unsigned int samp_rate;


// Destructor
GrainCluster::~GrainCluster()
{
    if (myGrains != NULL) {
        for (int i = 0; i < myGrains->size(); i++) {
            delete myGrains->at(i);
        }
        delete myGrains;
    }

    if (myVis)
        delete myVis;
    if (myLock)
        delete myLock;
    if (channelMults)
        delete[] channelMults;
}


// Constructor
GrainCluster::GrainCluster(vector<AudioFile *> *soundSet, float theNumVoices)
{
    // initialize mutext
    myLock = new Mutex();
    // cluster id
    myId = ++clusterId;

    // playback bool to make sure we precisely time grain triggers
    awaitingPlay = false;

    // number of voices
    numVoices = theNumVoices;
    // initialize random number generator (for random motion)
    srand(time(NULL));

    // initialize interfacing flags
    addFlag = false;
    removeFlag = false;

    // keep pointer to the sound set
    theSounds = soundSet;

    // trigger idx
    nextGrain = 0;

    // intialize timer
    local_time = 0;

    // default duration (ms)
    duration = 500.0;

    // default pitch
    pitch = 1.0f;

    // default window type
    windowType = HANNING;

    // initialize pitch LFO
    pitchLFOFreq = 0.01f;
    pitchLFOAmount = 0.0f;

    // initialize channel multiplier array
    channelMults = new double[MY_CHANNELS];
    for (int i = 0; i < MY_CHANNELS; i++) {
        channelMults[i] = 0.999f;
    }

    currentAroundChan = 1;
    stereoSide = 0;
    side = 1;


    spatialMode = UNITY;
    channelLocation = -1;

    myDirMode = RANDOM_DIR;

    // create grain voice vector
    myGrains = new vector<GrainVoice *>;

    // populate grain cloud
    for (int i = 0; i < numVoices; i++) {
        myGrains->push_back(new GrainVoice(theSounds, duration, pitch));
    }

    // set volume of cloud to unity
    setVolumeDb(0.0);

    // set overlap (default to full overlap)
    setOverlap(1.0f);

    //    //direction
    //    setDirection(RANDOM_DIR);

    // initialize trigger time (samples)
    bang_time = duration * ::samp_rate * (double)0.001 / overlap;

    // load grains
    for (int i = 0; i < myGrains->size(); i++) {
        myGrains->at(i)->setDurationMs(duration);
    }

    // state - (user can remove cloud from "play" for editing)
    isActive = true;
}


// register controller for communication with view
void GrainCluster::registerVis(GrainClusterVis *vis)
{
    myVis = vis;
    myVis->setDuration(duration);
}

// turn on/off
void GrainCluster::toggleActive()
{
    isActive = !isActive;
}

bool GrainCluster::getActiveState()
{
    return isActive;
}


// set window type
void GrainCluster::setWindowType(int winType)
{
    int numWins = Window::Instance().numWindows();
    windowType = winType % numWins;

    if (windowType < 0) {
        windowType = Window::Instance().numWindows() - 1;
    }
    if (windowType == RANDOM_WIN) {
        for (int i = 0; i < myGrains->size(); i++) {
            myGrains->at(i)->setWindow(
                (int)floor(randf() * Window::Instance().numWindows() - 1));
        }
    }
    else {

        for (int i = 0; i < myGrains->size(); i++) {
            // cout << "windowtype " << windowType << endl;
            myGrains->at(i)->setWindow(windowType);
        }
    }
}

int GrainCluster::getWindowType()
{
    return windowType;
}


void GrainCluster::addGrain()
{
    addFlag = true;
    myVis->addGrain();
}

void GrainCluster::removeGrain()
{
    removeFlag = true;
    myVis->removeGrain();
}


// return id for grain cluster
unsigned int GrainCluster::getId()
{
    return myId;
}


// overlap (input on 0 to 1 scale)
void GrainCluster::setOverlap(float target)
{
    if (target > 1.0f)
        target = 1.0f;
    else if (target < 0.0f)
        target = 0.0f;
    overlapNorm = target;
    // oops wrong!//overlap = ((float)(myGrains->size()))*0.25f*exp(log(2.0f)*target);

    float num = (float)myGrains->size();

    overlap = exp(log(num) * target);

    //  cout<<"overlap set" << overlap << endl;
    updateBangTime();
}

float GrainCluster::getOverlap()
{
    return overlapNorm;
}

// duration
void GrainCluster::setDurationMs(float theDur)
{
    if (theDur >= 1.0f) {
        duration = theDur;
        for (int i = 0; i < myGrains->size(); i++)
            myGrains->at(i)->setDurationMs(duration);

        updateBangTime();

        // notify visualization
        if (myVis)
            myVis->setDuration(duration);
    }
}

// update internal grain trigger time
void GrainCluster::updateBangTime()
{
    bang_time = duration * ::samp_rate * (double)0.001 / overlap;
    // cout << "duration: " << duration << ", new bang time " << bang_time << endl;
}


// pitch
void GrainCluster::setPitch(float targetPitch)
{
    if (targetPitch < 0.0001) {
        targetPitch = 0.0001;
    }
    pitch = targetPitch;
    for (int i = 0; i < myGrains->size(); i++)
        myGrains->at(i)->setPitch(targetPitch);
}

float GrainCluster::getPitch()
{
    return pitch;
}


//-----------------------------------------------------------------
// Cluster volume
//-----------------------------------------------------------------
void GrainCluster::setVolumeDb(float volDb)
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

    for (int i = 0; i < myGrains->size(); i++)
        myGrains->at(i)->setVolume(normedVol);
}

float GrainCluster::getVolumeDb()
{
    return volumeDb;
}


// direction mode
void GrainCluster::setDirection(int dirMode)
{
    myDirMode = dirMode % 3;
    if (myDirMode < 0) {
        myDirMode = 2;
    }
    // cout << "dirmode num" << myDirMode << endl;
    switch (myDirMode) {
    case FORWARD:
        //      cout << "set for" << endl;
        for (int i = 0; i < myGrains->size(); i++)
            myGrains->at(i)->setDirection(1.0);
        break;
    case BACKWARD:
        //    cout << "set back" << endl;
        for (int i = 0; i < myGrains->size(); i++)
            myGrains->at(i)->setDirection(-1.0);
        break;
    case RANDOM_DIR:
        for (int i = 0; i < myGrains->size(); i++) {
            if (randf() > 0.5)
                myGrains->at(i)->setDirection(1.0);
            else
                myGrains->at(i)->setDirection(-1.0);
        }

    default:
        break;
    }
}


// return grain direction int (see enum.  currently, 0 = forward, 1 = back, 2 = random)
int GrainCluster::getDirection()
{
    return myDirMode;
}

// return duration in ms
float GrainCluster::getDurationMs()
{
    return duration;
}


// return number of voices in this cloud
unsigned int GrainCluster::getNumVoices()
{
    return myGrains->size();
}


// compute audio
void GrainCluster::nextBuffer(double *accumBuff, unsigned int numFrames)
{

    if (addFlag == true) {
        addFlag = false;
        myGrains->push_back(new GrainVoice(theSounds, duration, pitch));
        int idx = myGrains->size() - 1;
        myGrains->at(idx)->setWindow(windowType);
        switch (myDirMode) {
        case FORWARD:
            myGrains->at(idx)->setDirection(1.0);
            break;
        case BACKWARD:
            myGrains->at(idx)->setDirection(-1.0);
            break;
        case RANDOM_DIR:
            if (randf() > 0.5)
                myGrains->at(idx)->setDirection(1.0);
            else
                myGrains->at(idx)->setDirection(-1.0);
            break;

        default:
            break;
        }

        myGrains->at(idx)->setVolume(normedVol);
        numVoices += 1;
        setOverlap(overlapNorm);
    }

    if (removeFlag == true) {
        myLock->lock();
        if (myGrains->size() > 1) {
            if (nextGrain >= myGrains->size() - 1) {
                nextGrain = 0;
            }
            myGrains->pop_back();
            setOverlap(overlapNorm);
        }
        removeFlag = false;
        myLock->unlock();
    }

    if (isActive == true) {


        // initialize play positions array
        double playPositions[theSounds->size()];
        double playVols[theSounds->size()];

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
                    for (int i = 0; i < theSounds->size(); i++) {
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
                    myGrains->at(nextGrain)->setPitch(nextPitch);
                }


                // update spatialization/get new channel multiplier set
                updateSpatialization();
                myGrains->at(nextGrain)->setChannelMultipliers(channelMults);

                // trigger grain
                awaitingPlay = myGrains->at(nextGrain)->playMe(playPositions, playVols);

                // only advance if next grain is playable.  otherwise, cycle
                // through again to wait for playback
                if (!awaitingPlay) {
                    // queue next grain for trigger
                    nextGrain++;
                    // wrap grain idx
                    if (nextGrain >= myGrains->size())
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
            for (int k = 0; k < myGrains->size(); k++) {
                myGrains->at(k)->nextBuffer(accumBuff, frameSkip, nextFrame, k);
            }
        }
    }
}


// pitch lfo methods
void GrainCluster::setPitchLFOFreq(float pfreq)
{
    pitchLFOFreq = fabs(pfreq);
}

void GrainCluster::setPitchLFOAmount(float lfoamt)
{
    if (lfoamt < 0.0) {
        lfoamt = 0.0f;
    }
    pitchLFOAmount = lfoamt;
}

float GrainCluster::getPitchLFOFreq()
{
    return pitchLFOFreq;
}

float GrainCluster::getPitchLFOAmount()
{
    return pitchLFOAmount;
}


// spatialization methods
void GrainCluster::setSpatialMode(int theMode, int channelNumber = -1)
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

int GrainCluster::getSpatialMode()
{
    return spatialMode;
}
int GrainCluster::getSpatialChannel()
{
    return channelLocation;
}

// spatialization logic
void GrainCluster::updateSpatialization()
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


//-----------------------------------------------------------------------------------------------
// GRAPHICS
//-----------------------------------------------------------------------------------------------

GrainClusterVis::~GrainClusterVis()
{
    if (myGrainsV)
        delete myGrainsV;
}

GrainClusterVis::GrainClusterVis(float x, float y, unsigned int numVoices,
                                 vector<SoundRect *> *rects)
{
    // get screen width and height
    MyGLScreen *screen = theApplication->GLwindow()->screen();
    screenWidth = screen->width();
    screenHeight = screen->height();

    startTime = GTime::instance().sec;
    // cout << "cluster started at : " << startTime << " sec " << endl;
    gcX = x;
    gcY = y;

    //    cout << "cluster x" << gcX << endl;
    //    cout << "cluster y" << gcY  << endl;


    // randomness params
    xRandExtent = 3.0;
    yRandExtent = 3.0;

    // init add and remove flags to false
    addFlag = false;
    removeFlag = false;

    // select on instantiation
    isSelected = true;

    // pulse frequency
    freq = 1.0f;

    // pointer to landscape visualization objects
    theLandscape = rects;

    myGrainsV = new vector<GrainVis *>;

    for (int i = 0; i < numVoices; i++) {
        myGrainsV->push_back(new GrainVis(gcX, gcY));
    }

    numGrains = numVoices;


    // visualization stuff
    minSelRad = 15.0f;
    maxSelRad = 19.0f;
    lambda = 0.997;
    selRad = minSelRad;
    targetRad = maxSelRad;
}

void GrainClusterVis::setDuration(float dur)
{
    freq = 1000.0 / dur;
}

// return cluster x
float GrainClusterVis::getX()
{
    return gcX;
}
// return cluster y
float GrainClusterVis::getY()
{
    return gcY;
}

void GrainClusterVis::draw()
{


    double t_sec = GTime::instance().sec - startTime;
    // cout << t_sec << endl;

    // if ((g_time -last_gtime) > 50){
    glPushMatrix();
    glTranslatef((GLfloat)gcX, (GLfloat)gcY, 0.0);
    // Grain cluster representation
    if (isSelected)
        glColor4f(0.1, 0.7, 0.6, 0.35);
    else
        glColor4f(0.0, 0.4, 0.7, 0.3);

    selRad = minSelRad + 0.5 * (maxSelRad - minSelRad) *
                             sin(2 * PI * (freq * t_sec + 0.125));
    gluDisk(gluNewQuadric(), selRad, selRad + 5.0, 128, 2);
    glPopMatrix();

    // update grain motion;
    // Individual voices

    // disc version (lower quality, but works on graphics cards that don't support GL_POINT_SMOOTH)
    //
    //    for (int i = 0; i < numGrains; i++){
    //        glPushMatrix();
    //        myGrainsV->at(i)->draw(mode);
    //        glPopMatrix();
    //    }

    // end disc version

    // point version (preferred)
    glPushMatrix();
    // update grain motion;
    // Individual voices
    for (int i = 0; i < numGrains; i++) {
        myGrainsV->at(i)->draw();
    }
    glPopMatrix();

    // end point version
}


// get trigger position/volume relative to sound rects for single grain voice
void GrainClusterVis::getTriggerPos(unsigned int idx, double *playPos,
                                    double *playVol, float theDur)
{
    bool trigger = false;
    SoundRect *theRect = NULL;
    if (idx < myGrainsV->size()) {
        GrainVis *theGrain = myGrainsV->at(idx);
        // TODO: motion models
        // updateGrainPosition(idx,gcX + randf()*50.0 + randf()*(-50.0),gcY + randf()*50.0 + randf()*(-50.0));
        updateGrainPosition(idx, gcX + (randf() * xRandExtent - randf() * xRandExtent),
                            gcY + (randf() * yRandExtent - randf() * yRandExtent));
        for (int i = 0; i < theLandscape->size(); i++) {
            theRect = theLandscape->at(i);
            bool tempTrig = false;
            tempTrig = theRect->getNormedPosition(playPos, playVol, theGrain->getX(),
                                                  theGrain->getY(), i);
            if (tempTrig == true)
                trigger = true;
            // cout << "playvol: " << *playPos << ", playpos: " << *playVol << endl;
        }
        if (trigger == true) {
            theGrain->trigger(theDur);
        }
    }
}


// rand cluster size
void GrainClusterVis::setXRandExtent(float mouseX)
{
    xRandExtent = fabs(mouseX - gcX);
    if (xRandExtent < 2.0f)
        xRandExtent = 0.0f;
}

void GrainClusterVis::setYRandExtent(float mouseY)
{
    yRandExtent = fabs(mouseY - gcY);
    if (yRandExtent < 2.0f)
        yRandExtent = 0.0f;
}
void GrainClusterVis::setRandExtent(float mouseX, float mouseY)
{
    setXRandExtent(mouseX);
    setYRandExtent(mouseY);
}
float GrainClusterVis::getXRandExtent()
{
    return xRandExtent;
}
float GrainClusterVis::getYRandExtent()
{
    return yRandExtent;
}

//
void GrainClusterVis::updateCloudPosition(float x, float y)
{
    float xDiff = x - gcX;
    float yDiff = y - gcY;
    gcX = x;
    gcY = y;
    for (int i = 0; i < myGrainsV->size(); i++) {
        float newGrainX = myGrainsV->at(i)->getX() + xDiff;
        float newGrainY = myGrainsV->at(i)->getY() + yDiff;
        myGrainsV->at(i)->moveTo(newGrainX, newGrainY);
    }
}

void GrainClusterVis::updateGrainPosition(int idx, float x, float y)
{
    if (idx < numGrains)
        myGrainsV->at(idx)->moveTo(x, y);
}


// check mouse selection
bool GrainClusterVis::select(float x, float y)
{
    float xdiff = x - gcX;
    float ydiff = y - gcY;

    if (sqrt(xdiff * xdiff + ydiff * ydiff) < maxSelRad)
        return true;
    else
        return false;
}

void GrainClusterVis::setSelectState(bool selectState)
{
    isSelected = selectState;
}

void GrainClusterVis::addGrain()
{
    //    addFlag = true;
    myGrainsV->push_back(new GrainVis(gcX, gcY));
    numGrains = myGrainsV->size();
}

// remove a grain from the cloud (visualization only)
void GrainClusterVis::removeGrain()
{
    //    removeFlag = true;
    if (numGrains > 1) {
        // delete object
        myGrainsV->pop_back();
        numGrains = myGrainsV->size();
    }
}
