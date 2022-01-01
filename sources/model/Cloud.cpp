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
extern ValueMin g_cloudValueMin;
extern ValueMax g_cloudValueMax;
extern CloudParams g_defaultCloudParams;

// ids
static unsigned int cloudId = 0;

CloudMidi::~CloudMidi()
{
    delete envelopeVolume;
    delete[] envelopeVolumeBuff;
    delete[] intermediateBuff;
    delete shadeBuff;
    for (int i = 0; i < myGrains.size(); i++) {
        delete myGrains[i];
    }

}

CloudMidi::CloudMidi(VecSceneSample *sampleSet, float theNumGrains, float theDuration, float thePitch, int theWindowType)
{
    envelopeVolume = new Env;
    envelopeVolumeBuff = new float[g_buffSize];
    intermediateBuff = new double[g_buffSize * theOutChannelCount];
    shadeBuff = new float[g_buffSize];
    envelopeAction.store(0);
    isActive = false;
    addFlag = false;
    removeFlag = false;
    duration = theDuration;
    // trigger idx
    nextGrain = 0;
    // populate grain cloud
    for (int i = 0; i < theNumGrains; i++) {
        myGrains.push_back(new Grain(sampleSet, theDuration, thePitch));
        myGrains[i]->setWindow(theWindowType);
    }
}

void CloudMidi::registerCloudVis(CloudVis *cloudVisToRegister)
{
    myCloudVis = cloudVisToRegister;
    //myCloudVis->setDuration(duration);
}

// Destructor
Cloud::~Cloud()
{
    delete envelopeVolume;
    delete[] envelopeVolumeBuff;
    delete[] intermediateBuff;
    delete[] shadeBuff;
    for (int i = 0; i < myGrains.size(); i++) {
        delete myGrains[i];
    }

    if (channelMults)
        delete[] channelMults;
    for (int i = 0; i < g_maxMidiVoices; i++){
        delete playedCloudMidi[i];
    }
}


// Constructor
Cloud::Cloud(VecSceneSample *sampleSet, float theNumGrains)
{
    //cout << "creation cloud" << endl;
    unsigned channelCount = theOutChannelCount;

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

    // default trajectory type
    trajectoryType = g_defaultCloudParams.trajectoryType;
    changed_trajectoryType = true;

    // initialize pitch LFO
    //pitchLFOFreq = 0.01f;
    pitchLFOFreq = g_defaultCloudParams.pitchLFOFreq;
    //pitchLFOAmount = 0.0f;
    pitchLFOAmount = g_defaultCloudParams.pitchLFOAmount;
    // initialize channel multiplier array
    channelMults = new double[channelCount];
    for (int i = 0; i < channelCount; i++) {
        channelMults[i] = 0.999f;
    }

    currentAroundChan = 1;
    stereoSide = 0;
    side = 1;


    //spatialMode = UNITY;
    spatialMode = g_defaultCloudParams.spatialMode;
    //channelLocation = -1;
    myOutputFirstNumber = g_defaultCloudParams.outputFirst;
    myOutputLastNumber = g_defaultCloudParams.outputLast;
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
    intermediateBuff = new double[g_buffSize * channelCount];
    shadeBuff = new float[g_buffSize];

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
    //isActive = g_defaultCloudParams.activateState;

    setActiveState(g_defaultCloudParams.activateState);
    myPhrase.setActiveState(g_defaultCloudParams.activateState);

    //midi polyphony
    midiChannel = g_defaultCloudParams.midiChannel;
    midiNote = g_defaultCloudParams.midiNote;

    //cout << "creation cloud, avant midiclouds" << endl;

    for (int i = 0; i < g_maxMidiVoices; i++){
        CloudMidi *l_CloudMidi = new CloudMidi(sampleSet, numGrains, duration, pitch, windowType);
        playedCloudMidi[i] = l_CloudMidi;
    }

    trajectoryType = STATIC;
    //cout << "fin creation cloud" << endl;

}
// register controller for communication with view
void Cloud::registerCloudVis(CloudVis *cloudVisToRegister)
{
    myCloudVis = cloudVisToRegister;
    myCloudVis->setDuration(duration);
    for (int i = 0; i < g_maxMidiVoices; i++){
        //cout  << "association cloud midi " << i << endl;
        CloudVis *cloudVisToRegister = myCloudVis->getMidiCloudVis(i);
        playedCloudMidi[i]->registerCloudVis(cloudVisToRegister);
    }
}

// turn on/off
void Cloud::toggleActive()
{
    setActiveState(!isActive);
    myCloudVis->setIsPlayed(isActive);
}

void Cloud::setActiveState(bool activateState)
{
    if (activateState) {
       envelopeAction.store(TriggerEnvelope);
       toStop = true;

       // trigger idx
       nextGrain = 0;

       // intialize timer
       local_time = 0;

       // playback bool to make sure we precisely time grain triggers
       awaitingPlay = false;

       myGrains[0]->setWindowFirstTime(true);

       for (int i = 0; i < myGrains.size(); i++) {
           myGrains[i]->setPlayingState(false);
           // cout << "windowtype " << windowType << endl;
           myGrains[i]->updateSampleSet();
//           myGrains[i]->setWindowFirstTime(true);
       }

//       toStop = true;

    }
    else
       envelopeAction.store(ReleaseEnvelope);
    if (myPhrase.getRestart())
        myPhrase.setActiveState(activateState);
    isActive = activateState;
}

void Cloud::setActiveMidiState(bool activateMidiState, int l_midiNote, int l_midiVelo)
{
    //cleanMidiClouds();
    if (activateMidiState){
        // midi note on
        cout << "midi on, note = " << l_midiNote << endl;
        // calculate pitch
        int ecartNotes = l_midiNote - midiNote;
        float musicalPitch = (12*log2f(pitch));
        float newMusicalPitch = musicalPitch + ecartNotes;
        float newPitch = (powf(2, (float) (newMusicalPitch / 12)));
        // create note on top
        playedCloudMidi[l_midiNote]->pitch = newPitch;
        playedCloudMidi[l_midiNote]->velocity = l_midiVelo;
        playedCloudMidi[l_midiNote]->envelopeVolume->setParam(envelopeVolume->getParam());
        playedCloudMidi[l_midiNote]->envelopeAction.store(TriggerEnvelope);
        playedCloudMidi[l_midiNote]->isActive = true;
        playedCloudMidi[l_midiNote]->toStop = true;
        // visual
        myCloudVis->activateMidiVis(l_midiNote, true);
        playedCloudMidi[l_midiNote]->myCloudVis->restartTrajectory();
    }
    else {
        // midi note off
        // cout << "midi off, note = " << l_midiNote << endl;
        playedCloudMidi[l_midiNote]->envelopeAction.store(ReleaseEnvelope);
    }
}

bool Cloud::getActiveState()
{
    return isActive;
}


// set window type
void Cloud::setWindowType(int winType)
{
    if (locked) {
        showMessageLocked();
        return;
    }
    int numWins = Window::Instance().numWindows();
    windowType = winType % numWins;

    if (windowType < 0) {
        windowType = Window::Instance().numWindows() - 1;
    }
    if (windowType == RANDOM_WIN) {
        for (unsigned long i = 0; i < myGrains.size(); i++) {
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
    for (int i = 0; i < g_maxMidiVoices; i++){
        for (int j = 0; j < playedCloudMidi[i]->myGrains.size(); j++)
            playedCloudMidi[i]->myGrains[j]->setWindow(windowType);
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
    if (myGrains.size() == g_cloudValueMax.numGrains)
        return;
    // addFlag = true;
    myCloudVis->addGrain();

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

    for (int ii = 0; ii < g_maxMidiVoices; ii++){
        playedCloudMidi[ii]->myGrains.push_back(new Grain(theSamples, duration, playedCloudMidi[ii]->pitch));
        size_t iidx = playedCloudMidi[ii]->myGrains.size() - 1;

        playedCloudMidi[ii]->myGrains[iidx]->setWindow(windowType);
        switch (myDirMode) {
        case FORWARD:
            playedCloudMidi[ii]->myGrains[iidx]->setDirection(1.0);
            break;
        case BACKWARD:
            playedCloudMidi[ii]->myGrains[iidx]->setDirection(-1.0);
            break;
        case RANDOM_DIR:
            if (randf() > 0.5)
                playedCloudMidi[ii]->myGrains[iidx]->setDirection(1.0);
            else
                playedCloudMidi[ii]->myGrains[iidx]->setDirection(-1.0);
            break;

        default:
            break;
        }
        playedCloudMidi[ii]->myGrains[idx]->setVolume(normedVol);

        /*if (playedCloudMidi[i]->isActive)
            playedCloudMidi[i]->addFlag = true;
        else
            playedCloudMidi[i]->myGrains.push_back(new Grain(theSamples, duration, pitch));*/
    }
    changed_numGrains = true;
    // debug std::cout << "fin add grain, numgrains =" << numGrains<< std::endl;
    // debug std::cout << "fin add grain, myGrains.size()="<<myGrains.size() << std::endl;
}

void Cloud::removeGrain()
{
    if (myGrains.size() == g_cloudValueMin.numGrains)
        return;
    //removeFlag = true;

    std::unique_lock<std::mutex> lock(::currentSceneMutex); // protection
    myCloudVis->removeGrain();

    delete myGrains.back();
    //delete myGrains[myGrains.size() - 1];
    myGrains.pop_back();
    nextGrain = 0;
    if (nextGrain >= myGrains.size() - 1) {
        nextGrain = 0;
    }

   for (int i = 0; i < g_maxMidiVoices; i++) {
        delete playedCloudMidi[i]->myGrains.back();
        playedCloudMidi[i]->myGrains.pop_back();
        /*if (playedCloudMidi[i]->isActive){
            playedCloudMidi[i]->removeFlag = true;
        }
        else {
            delete playedCloudMidi[i]->myGrains[playedCloudMidi[i]->myGrains.size()-1];
            playedCloudMidi[i]->myGrains.pop_back();
        }*/
    }

    lock.unlock(); // end protection

    numGrains -= 1;
    //debug std::cout << "fin remove grain, numgrains =" << numGrains<< std::endl;
    //debug std::cout << "fin remove grain, myGrains.size()="<<myGrains.size() << std::endl;
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

void Cloud::setName(QString newName)
{
    myName = newName;
}

QString Cloud::getName()
{
    return myName;
}

// overlap (input on 0 to 1 scale)
void Cloud::setOverlap(float target)
{
    if (locked) {
        showMessageLocked();
        return;
    }
    if (target <= g_cloudValueMax.overlap && target >= g_cloudValueMin.overlap) {
        overlapNorm = target;
        float num = (float)myGrains.size();

        overlap = exp(log(num) * target);

        updateBangTime();
        changed_overlap = true;
    }
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
    if (locked) {
        showMessageLocked();
        return;
    }
    if (theDur >= g_cloudValueMin.duration && theDur <=g_cloudValueMax.duration) {
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
    // debug std::cout << "entree cloud::setpitch, pitch ="<< targetPitch<<std::endl;
    if (locked) {
        showMessageLocked();
        return;
    }
    if (targetPitch >= g_cloudValueMin.pitch && targetPitch <= g_cloudValueMax.pitch) {
        pitch = targetPitch;
        for (int i = 0; i < myGrains.size(); i++)
            myGrains[i]->setPitch(targetPitch);// + ctrlInterval);
        changed_pitch = true;
    }
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
    if (locked) {
        showMessageLocked();
        return;
    }
    // max = 6 db, min = -60 db
    if (volDb <= g_cloudValueMax.volumeDB && volDb >= g_cloudValueMin.volumeDB) {
       volumeDb = volDb;

       // convert to 0-1 representation
        normedVol = pow(10.0, volDb * 0.05);

        for (int i = 0; i < myGrains.size(); i++)
            myGrains[i]->setVolume(normedVol);
        changed_volumeDB = true;
    }
}

float Cloud::getVolumeDb()
{
    return volumeDb;
}

bool Cloud::changedVolumeDb()
{
    return changed_volumeDB;
}

void Cloud::setTrajectoryType(int l_TrajectoryType)
{
    if (locked) {
        showMessageLocked();
        return;
    }
    trajectoryType = l_TrajectoryType;
    myCloudVis->setTrajectoryType(l_TrajectoryType);
    changed_trajectoryType = true;
}

int Cloud::getTrajectoryType()
{
    return trajectoryType;
}

bool Cloud::changedTrajectoryType()
{
    return changed_trajectoryType;
}


// direction mode
void Cloud::setDirection(int dirMode)
{
    if (locked) {
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
    if (locked) {
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

bool Cloud::getGrainsRandom()
{
    return grainsRandom;
}

void Cloud::setGrainsRandom(bool l_random)
{
    grainsRandom = l_random;
}

// update after a change of sample set
void Cloud::updateSampleSet()
{
    for (Grain *grain : myGrains)
        grain->updateSampleSet();
}

void Cloud::setEnvelopeVolumeParam (ParamEnv envelopeVolumeParamToSet)
{
    if (locked) {
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
    if (locked) {
        showMessageLocked();
        return;
    }
    midiChannel = newMidiChannel;
    changed_midiChannel = true;
}

void Cloud::setMidiNote(int newMidiNote)
{
    if (locked) {
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

void Cloud::cleanMidiClouds()
{
    for (int i = 0; i < g_maxMidiVoices; i++){
        if (playedCloudMidi[i]->envelopeVolume->state() == Env::State::Off) {
            playedCloudMidi[i]->isActive = false;
        }
    }
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
    changed_trajectoryType = done;
    changed_ActiveRestartTrajectory = done;
    changed_ctrlInterval = done;
    changed_ctrlShade = done;
}

void Cloud::showMessageLocked()
{
    cout << "cloud locked, no change" << endl;
}

void Cloud::showParameters()
{
    cout << "cloud parameters :" << endl;
    cout << "id:" << myId << endl;
    cout << "overlap = " << overlap << endl;
    cout << "pitch = " << pitch << endl;
    cout << "duration = " << duration <<endl;
    cout << "pitchLFOFreq = " << pitchLFOFreq << endl;
    cout << "pitchLFOAmount = " << pitchLFOAmount << endl;
    cout << "direction = " << myDirMode << endl;
    cout << "windowType = " << windowType << endl;
    cout << "first output = " << myOutputFirstNumber << endl;
    cout << "last output = " << myOutputLastNumber << endl;
    cout << "spatial mode = " << spatialMode << endl;
    cout << "volume DB = " << volumeDb << endl;
    cout << "midinote = " << midiNote << endl;
    cout << "locked = " << locked << endl;
    cout << "x = " << myCloudVis->getX() << endl;
    cout << "y = " << myCloudVis->getY() << endl;
    cout << "x extent = " << myCloudVis->getXRandExtent() << endl;
    cout << "y extent = " << myCloudVis->getYRandExtent() << endl;

    cout << "trajectoryType = " << myCloudVis->getTrajectoryType() << endl;
    switch (myCloudVis->getTrajectoryType())  {
    case RECORDED: {
        Recorded *tr_show = dynamic_cast<Recorded*>(myCloudVis->getTrajectory());
        cout << "positions : " << tr_show->lastPosition() << endl;
        for (int i = 1; i < tr_show->lastPosition(); i = i + 1) {
            cout << "position " << i << ", x = " << tr_show->getPosition(i).x
                 << ", y = " << tr_show->getPosition(i).y << ", delay = " << ", x = " << tr_show->getPosition(i).delay << endl;
        }
    }
        break;
    default:
        break;
    }
}

void Cloud::setActiveRestartTrajectory(bool l_choice)
{
    if (activeRestartTrajectory != l_choice)
        changed_ActiveRestartTrajectory = true;
    activeRestartTrajectory = l_choice;
}

bool Cloud::changedActiveRestartTrajectory()
{
    return changed_ActiveRestartTrajectory;
}

void Cloud::setCtrlInterval(float l_ctrInterval, bool fromOSC)
{
    ctrlInterval = l_ctrInterval;
    changed_ctrlInterval = fromOSC;
 //   for (int i = 0; i < myGrains.size(); i++) {
   //     myGrains[i]->setPitch(pitch);}// + ctrlInterval);}
   //}
  //      myGrains[i]->setPlayingState(false);}
    //nextGrain = 0;

}

float Cloud::getCtrlInterval()
{
    return ctrlInterval;
}

bool Cloud::changedCtrlInterval()
{
    return changed_ctrlInterval;
}

void Cloud::setCtrlShade(float l_ctrShade, bool fromOSC)
{
    //cout << "entre setcontrolshade, " << l_ctrShade << endl;
    ctrlShade = l_ctrShade;
    changed_ctrlShade = fromOSC;
}

float Cloud::getCtrlShade()
{
    return ctrlShade;
}

bool Cloud::changedCtrlShade()
{
    return changed_ctrlShade;
}

void Cloud::setCtrlX(int l_x)
{
    ctrlX = l_x;
}

int Cloud::getCtrlX()
{
    return ctrlX;
}

void Cloud::setCtrlY(int l_y)
{
    ctrlY = l_y;
}

int Cloud::getCtrlY()
{
    return ctrlY;
}

void Cloud::setCtrlAutoUpdate(bool n_ctrlAutoUpdate)
{
    ctrlAutoUpdate = n_ctrlAutoUpdate;
}

bool Cloud::getCtrlAutoUpdate()
{
    return ctrlAutoUpdate;
}

Scale *Cloud::getScale()
{
    return myPhrase.getScale();
}

bool Cloud::scaleAttraction()
{
    return myPhrase.scaleAttraction();
}

void Cloud::setScaleAttraction(bool n_state)
{
    myPhrase.setScaleAttraction(n_state);
}

void Cloud::insertScalePosition(ScalePosition n_scalePosition)
{
    myPhrase.insertScalePosition(n_scalePosition);
}

void Cloud::phraseActualise()
{
    float l_interval = myPhrase.getInterval();
    if (!myPhrase.getEndedState()) {
        if (myPhrase.getRelease()) {
            //cout << "getrelease true" << endl;
            myPhrase.setRestart(false);
            setActiveState(false);
        }
        if (scaleAttraction()) {
            if (scaleAttraction()) {
                double l_nearestPosition = myPhrase.getScale()->nearest(l_interval, -1000000, 1000000);
                l_interval = l_nearestPosition;
            }
        }

        setCtrlInterval(l_interval, false);
        setCtrlShade(myPhrase.getShade(), false);

        if (myPhrase.getRelease()) {
            //cout << "getrelease true" << endl;
//           myNode->setActiveState(false);
           myPhrase.setRestart(false);
           setActiveState(false);
        }
        if (myPhrase.getAttack()) {
            //cout << "getattack true" << endl;
//           myNode->setActiveState(false);
           myPhrase.setRestart(false);
           setActiveState(true);
        }
/*       else {
            //cout << "getrelease false" << endl;
           //myNode->setActiveState(true);
           myPhrase.setRestart(false);
           //if (!cloudRef->getActiveState())

           setActiveState(false);
           setActiveState(true);
        }*/

        setActualiseByPhrase(true);
    }
    else {
        if (myPhrase.getRelease()) {
            //cout << "getrelease true" << endl;
//           myNode->setActiveState(false);
           myPhrase.setRestart(false);
           setActiveState(false);
        }
        //cout << "ended state" << endl;
    }
    //cout << "interval = " << l_interval << endl;
}

unsigned long Cloud::getPhraseIntervalSize()
{
    return myPhrase.getMyControlIntervalSize();
}

unsigned long Cloud::getPhraseShadeSize()
{
    return myPhrase.getMyControlShadeSize();
}

Phrase *Cloud::getPhrase()
{
    return &myPhrase;
}

void Cloud::setActualiseByPhrase(bool n_state)
{
    actualiseByPhrase = n_state;
}

bool Cloud::getActualiseByPhrase()
{
    return actualiseByPhrase;
}

void Cloud::phraseRestart()
{
    myPhrase.setRestart(false);
    myPhrase.setActiveState(true);
}

void Cloud::setSilence(bool l_silence)
{
    silence = l_silence;
}

bool Cloud::getSilence()
{
    return silence;
}

void Cloud::setNewWay(bool n_NewWay)
{
    newWay = n_NewWay;
}

bool Cloud::getActiveRestartTrajectory()
{
    return activeRestartTrajectory;
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
void Cloud::nextBuffer(BUFFERPREC *accumBuff, unsigned int numFrames)
{
    if (newWay) {
        nextNewBuffer(accumBuff,numFrames);
        return;
    }
    unsigned channelCount = theOutChannelCount;

    // debug std::cout<<"entree nextbuffer cloud"<<std::endl;
    int l_envelopeAction = this->envelopeAction.exchange(0);

    switch (l_envelopeAction) {
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

    phraseActualise();

    if ((envelopeVolume->state() != Env::State::Off) && (!silence)) {// && (!getPhrase()->getSilenceState())) {
    //if (isActive) {
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
                //cout << "bang " << nextGrain << endl;
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
                        myCloudVis->getCloudPos(nextGrain, playPositions, playVols, duration);
                }

                // get next pitch (using LFO) -  eventually generalize to an applyLFOs method (if LFO control will be exerted over multiple params)
                //cout << "interval = " << ctrlInterval << endl;
                if ((pitchLFOAmount > 0.0f) && (pitchLFOFreq > 0.0f)) {
                    //float nextPitch = fabsf(pitch + pitchLFOAmount * sinf(2 * PI * pitchLFOFreq * GTime::instance().sec));
                    float pitchPow = pow(2, ((pitch + ctrlInterval) / 12));
                    float nextPitchPow = fabsf(pitchPow + ctrlInterval + pitchLFOAmount * sinf(2 * PI * pitchLFOFreq * GTime::instance().sec));
                    float nextPitch = 12 * log2(nextPitchPow);
                    myGrains[nextGrain]->setPitch(nextPitch);
                }
                else
                    myGrains[nextGrain]->setPitch(pitch + ctrlInterval);

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
            //if (!awaitingPlay) {
                // advance time
                local_time += frameSkip;

                // sample offset (1 sample at a time for now)
                nextFrame = j * frameSkip;
                // iterate over all grains
                //for (int k = 0; k < myGrains.size(); k++) {
                //  myGrains[k]->nextBuffer(accumBuff, frameSkip, nextFrame, k);
                //}
                // iterate over all grains
                memset(intermediateBuff, 0, numFrames * channelCount * sizeof(intermediateBuff[0]));
                for (int k = 0; k < myGrains.size(); k++) {
                    myGrains[k]->nextBuffer(intermediateBuff, frameSkip, nextFrame, k);
                }
                for (int i = 0; i < numFrames; ++i) {
                    //for (int j = 0; j < channelCount; ++j)
                    for (int l = myOutputFirstNumber; l <= myOutputLastNumber; ++l)
                        accumBuff[i * channelCount + l] += intermediateBuff[i * channelCount + l] * envelopeVolumeBuff[i] * ctrlShade;// * ((float) midiVelocity / 127);
                }
            //}
        }
    }
    else {
        if (toStop) {
            for (int i=0;i<myGrains.size();i=i+1)
                myGrains[i]->setPlayingState(false);

            toStop = false;
        }
        //cout << "wait" << endl;
    }
    // midi playing
    //debug : std::cout << "actualyPlayedMidi=" <<actualyPlayedMidi<< std::endl;
    for (int ii = 0; ii < g_maxMidiVoices; ii++){
        //debug: std::cout <<"ii="<<ii<<"playedCloudMidi[ii]->isActive="<<playedCloudMidi[ii]->isActive<<std::endl;
        if (playedCloudMidi[ii]->isActive){
            //debug: std::cout << "midi " << ii << endl;
            int midiEnvelopeAction = playedCloudMidi[ii]->envelopeAction.exchange(0);
            //debug: std::cout << "midiEnvelopeAction=" <<midiEnvelopeAction<< std::endl;
            //debug: std::cout << "envelope state=" << (int) playedCloudMidi[ii]->envelopeVolume->state() << std::endl;
            switch (midiEnvelopeAction) {
                case TriggerEnvelope:
                    //debug: std::cout << "trigger" << std::endl;
                    playedCloudMidi[ii]->envelopeVolume->trigger();
                    break;
                case ReleaseEnvelope:
                    //debug: std::cout << "release" << std::endl;
                    playedCloudMidi[ii]->envelopeVolume->release();
                    break;
                default:
                    //debug: std::cout << "autre ?" << std::endl;
                    break;
            }
            playedCloudMidi[ii]->envelopeVolume->generate(playedCloudMidi[ii]->envelopeVolumeBuff, numFrames);
            //debug : std::cout << "midi poly, actualyPlayedMidi= "<<actualyPlayedMidi<<",ii=" << ii <<std::endl;
            if (playedCloudMidi[ii]->envelopeVolume->state() != Env::State::Off) {
                //debug : std::cout << "midi poly, play"<<std::endl;
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
                                myCloudVis->getMidiCloudVis(ii)->getCloudPos(playedCloudMidi[ii]->nextGrain, playPositions, playVols, duration);
                        }

                        // get next pitch (using LFO) -  eventually generalize to an applyLFOs method (if LFO control will be exerted over multiple params)
                        if ((pitchLFOAmount > 0.0f) && (pitchLFOFreq > 0.0f)) {
                            //float nextPitch = fabsf(playedCloudMidi[ii]->pitch + pitchLFOAmount * sinf(2 * PI * pitchLFOFreq * GTime::instance().sec));
                            float pitchPow = pow(2, ((pitch + ii - midiNote) / 12));
                            float nextPitchPow = fabsf(pitchPow + pitchLFOAmount * sinf(2 * PI * pitchLFOFreq * GTime::instance().sec));
                            float nextPitch = 12 * log2(nextPitchPow);
                            playedCloudMidi[ii]->myGrains[playedCloudMidi[ii]->nextGrain]->setPitch(nextPitch);
                        }
                        else
                            //playedCloudMidi[ii]->myGrains[nextGrain]->setPitch(playedCloudMidi[ii]->pitch);
                            playedCloudMidi[ii]->myGrains[playedCloudMidi[ii]->nextGrain]->setPitch(pitch + ii - midiNote);


                        // update spatialization/get new channel multiplier set
                        updateSpatialization();
                        playedCloudMidi[ii]->myGrains[playedCloudMidi[ii]->nextGrain]->setChannelMultipliers(channelMults);

                        // trigger grain
                        awaitingPlay = playedCloudMidi[ii]->myGrains[playedCloudMidi[ii]->nextGrain]->playMe(playPositions, playVols);

                        // only advance if next grain is playable.  otherwise, cycle
                        // through again to wait for playback
                        if (!awaitingPlay) {
                            // queue next grain for trigger
                            playedCloudMidi[ii]->nextGrain++;
                            // wrap grain idx
                            if (playedCloudMidi[ii]->nextGrain >= playedCloudMidi[ii]->myGrains.size())
                                playedCloudMidi[ii]->nextGrain = 0;
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
                    memset(playedCloudMidi[ii]->intermediateBuff, 0, numFrames * channelCount * sizeof(playedCloudMidi[ii]->intermediateBuff[0]));
                    //debug : std::cout << "playedCloudMidi[ii]->myGrains.size()="<< playedCloudMidi[ii]->myGrains.size()<<std::endl;
                    for (int k = 0; k < playedCloudMidi[ii]->myGrains.size(); k++) {
                        playedCloudMidi[ii]->myGrains[k]->nextBuffer(playedCloudMidi[ii]->intermediateBuff, frameSkip, nextFrame, k);
                    }
                    //debug : std::cout<<"boucle temp, ii="<<ii<<",playedCloudMidi[ii]->midiNote="<<playedCloudMidi[ii]->midiNote<<",playedCloudMidi[ii]->velocity="<<playedCloudMidi[ii]->velocity<<std::endl;
                    for (int i = 0; i < numFrames; ++i) {
                        //for (int j = 0; j < channelCount; ++j)
                        for (int j = myOutputFirstNumber; j <= myOutputLastNumber; ++j)
                            accumBuff[i * channelCount + j] += playedCloudMidi[ii]->intermediateBuff[i * channelCount + j]
                                                              * playedCloudMidi[ii]->envelopeVolumeBuff[i]
                                                              * (((float) playedCloudMidi[ii]->velocity + 127) * g_cloudValueMin.midiVelocityBoost / 100) / 127;
                    }
                }
            }
            else {
                if (playedCloudMidi[ii]->toStop) {
                    for (int i=0;i<playedCloudMidi[ii]->myGrains.size();i=i+1)
                        playedCloudMidi[ii]->myGrains[i]->setPlayingState(false);

                    playedCloudMidi[ii]->toStop = false;
                }
                //cout << "wait" << endl;

                myCloudVis->activateMidiVis(ii, false);
                playedCloudMidi[ii]->isActive = false;
            }
        }

    }
    //debug: std::cout<<"sortie nextbuffer cloud"<<std::endl;
}

void Cloud::nextNewBuffer(BUFFERPREC *accumBuff, unsigned int numFrames)
{
    unsigned channelCount = theOutChannelCount;
    int l_envelopeAction = this->envelopeAction.exchange(0);

    switch (l_envelopeAction) {
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

    if (envelopeVolume->state() != Env::State::Off) {
        if (firstGrain < (myGrains.size() - 1)) {
            secondGrain = firstGrain + 1;
        }
        else {
            secondGrain = 0;
        }
        unsigned int cptNumFrames = 0;
        while (cptNumFrames < numFrames) {

        }
        for (int i = 0; i < numFrames; ++i) {
            if (!firstGrainCalculated) {
                memset(firstGrainBuff, 0, numFrames * channelCount * sizeof(firstGrainBuff[0]));
                myGrains[firstGrain]->nextNewBuffer(intermediateBuff, numFrames, i, firstGrain);
            }
            if (!secondGrainCalculated) {
                memset(secondGrainBuff, 0, numFrames * channelCount * sizeof(secondGrainBuff[0]));
                myGrains[secondGrain]->nextNewBuffer(intermediateBuff, numFrames, i, secondGrain);
            }
        }
    }
}


// pitch lfo methods
void Cloud::setPitchLFOFreq(float pfreq)
{
    if (locked) {
        showMessageLocked();
        return;
    }
    if (pfreq >= g_cloudValueMin.pitchLFOFreq && pfreq <= g_cloudValueMax.pitchLFOFreq) {
        pitchLFOFreq = fabsf(pfreq);
        changed_pitchLFOFreq = true;
    }
}

void Cloud::setPitchLFOAmount(float lfoamt)
{
    if (locked) {
        showMessageLocked();
        return;
    }
    if (lfoamt >= g_cloudValueMin.pitchLFOAmount && lfoamt <= g_cloudValueMax.pitchLFOAmount) {
        pitchLFOAmount = lfoamt;
        changed_pitchLFOAmount = true;
    }
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
    if (locked) {
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

void Cloud::setOutputFirst(int myOutput)
{
    myOutputFirstNumber = myOutput;
    updateSpatialization();
    changed_spatialMode = true;
}

void Cloud::setOutputLast(int myOutput)
{
    myOutputLastNumber = myOutput;
    updateSpatialization();
    changed_spatialMode = true;
}

int Cloud::getOutputFirst()
{
    return myOutputFirstNumber;
}

int Cloud::getOutputLast()
{
    return myOutputLastNumber;
}

// spatialization logic
void Cloud::updateSpatialization()
{
    unsigned channelCount = theOutChannelCount;

    // currently assumes orientation L: 0,2,4,...  R: 1,3,5, etc (interleaved)
    switch (spatialMode) {
    case UNITY:
        for (int i = 0; i < channelCount; i++) {
            if (i >= myOutputFirstNumber && i <= myOutputLastNumber)
                channelMults[i] = 0.999f;
            else
                channelMults[i] = 0.0f;
        }
        break;
    case STEREO:

        if (stereoSide == 0) {  // left
            for (int i = 0; i < channelCount; i++) {
                channelMults[i] = 0.0f;
                if ((i % 2) == 0 && (i >= myOutputFirstNumber && i <= myOutputLastNumber))
                    channelMults[i] = 0.999f;
                else
                    channelMults[i] = 0.0f;
            }
            stereoSide = 1;
        }
        else {  // right
            for (int i = 0; i < channelCount; i++) {
                channelMults[i] = 0.0f;
                if ((i % 2) == 0)
                    channelMults[i] = 0.0f;
                else
                    if (i >= myOutputFirstNumber && i <= myOutputLastNumber)
                        channelMults[i] = 0.999f;
            }
            stereoSide = 0;
        }
        break;
    case AROUND:
        for (int i = 0; i < channelCount; i++) {
            channelMults[i] = 0;
        }

        // 1 3 5 7 6 4 2 0

        channelMults[currentAroundChan] = 0.999;
        currentAroundChan += side * 2;
//        if ((currentAroundChan > channelCount) || (currentAroundChan < 0)) {
        if ((currentAroundChan > myOutputLastNumber) || (currentAroundChan < myOutputFirstNumber)) {
            side = -1 * side;
            currentAroundChan += side * 3;
        }
        // currentAroundChan = currentAroundChan % channelCount;
        break;

    default:
        break;
    }
}

CloudMidi *Cloud::getMidiCloud(int l_numNote)
{
    return playedCloudMidi[l_numNote];
}

