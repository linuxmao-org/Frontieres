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
//  Grain.cpp
//  Frontières
//
//  Created by Christopher Carlson on 11/20/11.
//

#include "model/Grain.h"
#include "model/Sample.h"
#include "model/Scene.h"
#include "visual/SampleVis.h"
#include "dsp/Window.h"
#include "theglobals.h"
extern unsigned int samp_rate;

//-------------------AUDIO----------------------------------------------------//

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Grain::~Grain()
{
    if (chanMults)
        delete[] chanMults;

    if (queuedChanMults)
        delete[] queuedChanMults;
}


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

Grain::Grain(VecSceneSample *sampleSet, float durationMs, float thePitch)
{
    int channelCount = theOutChannelCount;

    // store pointer to external vector of sample files
    theSamples = sampleSet;

    // initialize with the current sample set
    updateSampleSet();

    // playing status init
    playingState = false;


    // direction
    if (randf() < float(0.5))
        direction = 1.0;
    else
        direction = -1.0;
    queuedDirection = direction;

    // set default windowType
    windowType = HANNING;
    queuedWindowType = windowType;

    // window type is hanning
    window = Window::Instance().getWindow(windowType);

    // grain volume
    localAtten = 1.0;
    queuedLocalAtten = localAtten;

    // grain duration (ms)
    duration = durationMs;
    queuedDuration = durationMs;

    // grain playback rate
    pitch = thePitch;
    queuedPitch = pitch;


    // spatialization
    queuedChanMults = new double[channelCount];
    chanMults = new double[channelCount];
    // set panning values - all channels active by default
    for (int i = 0; i < channelCount; i++) {
        chanMults[i] = 1.0;
        queuedChanMults[i] = 1.0;
    }


    // new input flag (no new inputs on instantiation)
    newParam = false;

    // set playhead increment
    playInc = pitch * direction;
    // initialize window reading params
    winReader = 0.0;  // 0 idx

    // get duration in samples (fractional)
    winDurationSamps = ceil(duration * ::samp_rate * (float)0.001);
    winInc = (double)WINDOW_LEN / winDurationSamps;
}


//-----------------------------------------------------------------------------
// Turn on grain.
// input args = position and volume vectors in sample visual space
// returns whether or not grain is awaiting play.
// parent cloud will wait to play this grain if the grain is still
// this should not be an issue unless the overlap value is erroneous
//-----------------------------------------------------------------------------
bool Grain::playMe(double *startPositions, double *startVols)
{

    if (playingState == false) {
        //cout << "Grain triggered" << endl;

        //if (true) {
        // next buffer call will play
        playingState = true;

        // grab queued params if changed
        if (newParam == true)
            updateParams();

        // convert relative start positions to sample locations

        activeSamples.clear();

        VecSceneSample &sceneSamples = *theSamples;
        double l_playPos = 0;
        int ptrWave = 0;
        for (int i = 0, n = sceneSamples.size(); i < n; i++) {
            if (startPositions[i] != -1) {
                activeSamples.push_back(i);
                //playPositions[i] =
                //    floor(startPositions[i] * (sceneSamples[i]->sample->frames - 1));
                l_playPos = floor(startPositions[i] * (sceneSamples[i]->sample->frames - 1)) + sceneSamples[i]->sample->ptrLagWave;
                if (l_playPos >= (sceneSamples[i]->sample->frames - 1))
                    l_playPos = l_playPos - ((sceneSamples[i]->sample->frames));

                playPositions[i] = l_playPos;


                playVols[i] = startVols[i];
            }
        }

        // initialize window reader index
        winReader = 0;
        return false;
    }
    else {
        // debug
        cout << "Grain triggered too soon..." << endl;
        return true;
    }
}


//-----------------------------------------------------------------------------
// Find out if grain is currently on
//-----------------------------------------------------------------------------
bool Grain::isPlaying()
{
    return playingState;
}

void Grain::setPlayingState(bool n_playingState)
{
    playingState = n_playingState;
}


//-----------------------------------------------------------------------------
// Set channel multipliers
//-----------------------------------------------------------------------------
void Grain::setChannelMultipliers(double *multipliers)
{
    unsigned channelCount = theOutChannelCount;
    for (int i = 0; i < channelCount; i++) {
        queuedChanMults[i] = multipliers[i];
    }
    newParam = true;
}


//-----------------------------------------------------------------------------
// Set channel multipliers
//-----------------------------------------------------------------------------
void Grain::setVolume(float theVolNormed)
{
    float pVol = fabs(theVolNormed);
    queuedLocalAtten = pVol;
}

//-----------------------------------------------------------------------------
// Get channel multipliers
//-----------------------------------------------------------------------------
float Grain::getVolume()
{
    return localAtten;
}

//-----------------------------------------------------------------------------
// Set duration (effective on next trigger)
//-----------------------------------------------------------------------------
void Grain::setDurationMs(float dur)
{
    // get absolute value
    queuedDuration = fabs(dur);
    if (queuedDuration != duration)
        newParam = true;
}


//-----------------------------------------------------------------------------
// Set pitch (effective on next trigger)
//-----------------------------------------------------------------------------
void Grain::setPitch(float newPitch)
{
    // get absolute value
    queuedPitch = pow(2, (float) (newPitch / 12));
//    if (queuedPitch != pitch)
        newParam = true;
}

float Grain::getPitch()
{
    if (queuedPitch != pitch)
        return queuedPitch;
    else
        return pitch;
}


//-----------------------------------------------------------------------------
// Update params
//-----------------------------------------------------------------------------
void Grain::updateParams()
{
    unsigned channelCount = theOutChannelCount;

    // update parameter set

    localAtten = queuedLocalAtten;

    // playback rate
    pitch = queuedPitch;

    // grain duration in ms
    duration = queuedDuration;

    // direction of playback
    direction = queuedDirection;

    // playhead increment
    playInc = direction * pitch;

    // window
    windowType = queuedWindowType;

    // switch window
    window = Window::Instance().getWindow(windowType);

    // double value, but eliminate fractional component -
    winDurationSamps = ceil(duration * ::samp_rate * (double)0.001);

    // how far should we advance through windowing function each sample
    winInc = (double)WINDOW_LEN / winDurationSamps;

    // spatialization - get new channel multipliers
    for (int i = 0; i < channelCount; i++) {
        chanMults[i] = queuedChanMults[i];
    }

    // all params have been updated
    newParam = false;
}


//-----------------------------------------------------------------------------
// Set window type (effective on next trigger)
//-----------------------------------------------------------------------------

void Grain::setWindow(unsigned int theType)
{
    queuedWindowType = theType;
    if (queuedWindowType != windowType)
        newParam = true;
}

void Grain::setWindowFirstTime(bool n_firstTime)
{
    windowFirstTime = n_firstTime;
}

//-----------------------------------------------------------------------------
// Update after a change of sample set
//-----------------------------------------------------------------------------
void Grain::updateSampleSet()
{
    // get number of loaded samples
    unsigned numSamples = (unsigned)theSamples->size();

    // set play positions to -1 for all
    playPositions.reset(new double[numSamples]);
    playVols.reset(new double[numSamples]);
    // initialize - (-1 signifies that sample should not be played)
    for (int i = 0; i < numSamples; i++) {
        playPositions[i] = -1.0;
        playVols[i] = 0.0;
    }

    activeSamples.clear();
    activeSamples.reserve(numSamples);
}

void Grain::setFristGrain(bool n_firstGrain)
{
    firstGrain = n_firstGrain;
}

bool Grain::getNewPlayingState()
{
    return newPlayingState;
}

void Grain::setNewPlayingState(bool n_newPlayingState)
{
    if (n_newPlayingState) {
        cptPlaying = 0;
    }
    newPlayingState = n_newPlayingState;
}

unsigned int Grain::getCptPlaying()
{
    return cptPlaying;
}

void Grain::setCptPlaying(unsigned int n_CptPlaying)
{
    cptPlaying = n_CptPlaying;
}

void Grain::incCptPlaying()
{
    if (cptPlaying < maxCptPlaying){
        cptPlaying = cptPlaying + 1;
    }
    else {
        setNewPlayingState(false);
    }
}

void Grain::setNewPlayPos(int n_playX, int n_playY)
{
    if (newPlayingState) {
        nextX = n_playX;
        nextY = n_playY;
    }
    else {
        playX = n_playX;
        playY = n_playY;
    }
}


//-----------------------------------------------------------------------------
// Set direction (effective on next trigger)
//-----------------------------------------------------------------------------

void Grain::setDirection(float thedir)
{
    queuedDirection = thedir;
    if (queuedDirection != direction)
        newParam = true;
}


//-----------------------------------------------------------------------------
// Compute next sub buffer of audio
//-----------------------------------------------------------------------------


void Grain::nextBuffer(double *accumBuff, unsigned int numFrames,
                            unsigned int bufferOffset, int name)
{
    unsigned channelCount = theOutChannelCount;

    // fill stereo accumulation buffer.  note, buffer output must be interlaced
    // ch1,ch2,ch1,ch2, etc... and playPositions are in frames, NOT SAMPLES.

    // only go through this ordeal if grain is active
    if (playingState == true) {
        // initialize local vars
        VecSceneSample &sceneSamples = *theSamples;

        // linear interp coeff
        double nu = 0.0;

        // next window value
        double nextMult = 0.0;

        // ref idx for left bound of interp
        double flooredIdx = 0;

        // next file index
        int nextSample = -1;

        // waveform params
        BUFFERPREC *wave = NULL;
        int channels = 0;
        unsigned int frames = 0;

        // file reader position
        double pos = -1.0;

        // attenuation value
        double atten = 0.0;

        // output values
        double nextAmp = 0.0;
        double monoWaveVal = 0.0;
        double stereoLeftVal = 0.0;
        double stereoRightVal = 0.0;


        // iterate over requested number of samples
        //cout << "dbut cout de nu" << endl;
        for (unsigned int i = 0; i < numFrames; i++) {

            // Window multiplier - Get next val from window and check to see if we've reached the end
            if (winReader > (WINDOW_LEN - 1)) {
                nextMult = double(0.0);
                winReader = 0;
                playingState = false;
                return;
            }
            else {
                // interpolated read from window buffer
                flooredIdx = floor(winReader);
                nu = winReader - flooredIdx;  // interp coeff
                // set beginning of window to max for the first grain to respect enveloppe attack
                if (windowFirstTime) {
                    nextMult = 1;
                    if (i > (numFrames / 2))
                        windowFirstTime = false;
                }
                else {
                    // interpolated read (lin)
                    nextMult = (double(1.0) - nu) * window[(unsigned long)flooredIdx] +
                               nu * window[(unsigned long)flooredIdx + 1];
                }
                // increment reader
                winReader += winInc;
            }


            // reinit sample accumulators for mono and stereo files to prepare for this frame
            monoWaveVal = 0.0;
            stereoLeftVal = 0.0;
            stereoRightVal = 0.0;

            // Get next audio frame data (accumulate from each sample under grain)
            //-- REMEMBER - playPositions are in frames, not samples
            for (int j = 0; j < activeSamples.size(); j++) {

                nextSample = activeSamples[j];
                pos = playPositions[nextSample];  // get start position
                atten = playVols[nextSample];  // get volume relative to rect


                // if sample is in play,sample it
                if (pos > 0) {
                    // Sample vars
                    Sample *af = sceneSamples[nextSample]->sample;
                    wave = af->wave;
                    channels = af->channels;
                    frames = af->frames;

                    // get info for interpolation based on frame location
                    flooredIdx = floor(pos);
                    nu = pos - flooredIdx;

                    // handle mono and stereo files separately.
                    switch (channels) {
                    case 1:

                        // get next linearly interpolated sample val and make sure we are still inside
                        if ((flooredIdx >= 0) && ((flooredIdx + 1) < (frames - 1))) {
                            nextAmp = (((double)1.0 - nu) * wave[(unsigned long)flooredIdx] +
                                       nu * wave[(unsigned long)flooredIdx + 1]) *
                                      nextMult * atten;

                            // accumulate mono frame
                            monoWaveVal += nextAmp;

                            /*//old
                             accumBuff[(bufferOffset + i)*channelCount] += nextAmp;
                            //copy to other channels for mono
                            for (int k = 1; k < channelCount; k++){
                                accumBuff[(bufferOffset + i) * channelCount + k] += nextAmp;
                            }
                             */

                            // advance after each stereo frame (do calc twice for mono)
                            playPositions[nextSample] += playInc;
                        }
                        else {
                            // not playing anymore
                            playPositions[nextSample] = -1.0;
                        }
                        break;
                    case 2:  // stereo

                        // make sure we are still in sample
                        if ((flooredIdx >= 0) && ((flooredIdx + 1) < (frames - 1))) {


                            // left channel
                            stereoLeftVal +=
                                (((double)1.0 - nu) * wave[(unsigned long)flooredIdx * 2] +
                                 nu * wave[(unsigned long)(flooredIdx + 1) * 2]) *
                                nextMult * atten;
                            // right channel
                            stereoRightVal +=
                                (((double)1.0 - nu) * wave[(unsigned long)flooredIdx * 2 + 1] +
                                 nu * wave[(unsigned long)(flooredIdx + 1) * 2 + 1]) *
                                nextMult * atten;

                            /*//old
                            //left channel
                            accumBuff[(bufferOffset + i)*channelCount] += (((double)
                            1.0 - nu)*wave[(unsigned long)flooredIdx*2] + nu * wave[(unsigned long)(flooredIdx + 1)*2])*nextMult*atten;

                            //right channel
                            accumBuff[(bufferOffset + i) * channelCount + 1] +=
                            (((double) 1.0 - nu)*wave[(unsigned long)flooredIdx*2 + 1] + nu * wave[(unsigned long)(flooredIdx + 1)*2 + 1])*nextMult*atten;
                             */

                            // advance after each stereo frame (do calc twice for mono)
                            playPositions[nextSample] += playInc;
                        }
                        else {
                            // not playing anymore
                            playPositions[nextSample] = -1.0;
                        }
                        break;
                        // don't handle numbers of channels > 2
                    default:
                        break;
                    }  // end switch channels

                }  // end position check

            }  // end accumulation for current frame

            // spatialize output
            for (int k = 0; k < channelCount; k++) {
                // preserve stereo waveform L/R for now and just sample alternate channels in "AROUND" case (see Cloud.cpp updateSpatialization routine)
                if ((k % 2) == 0) {
                    accumBuff[(bufferOffset + i) * channelCount + k] +=
                        (stereoLeftVal + monoWaveVal) * chanMults[k] * localAtten;
                }
                else {
                    accumBuff[(bufferOffset + i) * channelCount + k] +=
                        (stereoRightVal + monoWaveVal) * chanMults[k] * localAtten;
                }

                // clip if needed
                if (accumBuff[(bufferOffset + i) * channelCount + k] > 1.0) {
                    accumBuff[(bufferOffset + i) * channelCount + k] = 1.0;
                }
                else {
                    if (accumBuff[(bufferOffset + i) * channelCount + k] < -1.0) {
                        accumBuff[(bufferOffset + i) * channelCount + k] = -1.0;
                    }
                }
            }
        }
    }
    else {
        // cout << "return - not playing " << endl;
        return;
    }
}

void Grain::nextNewBuffer(double *accumBuff, unsigned int numFrames, int bufferPos, int name)
{
    VecSceneSample &sceneSamples = *theSamples;
    if (newPlayingState) {

    }
    float grainX = playX;
    float grainY = playY;

    for (unsigned long i = 0, n = sceneSamples.size(); i < n; i++) {
        if (sceneSamples[i]->view->insideMe(grainX, grainY)) {

        }

    }

}
