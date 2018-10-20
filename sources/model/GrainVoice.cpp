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
//  GrainVoice.cpp
//  Frontières
//
//  Created by Christopher Carlson on 11/20/11.
//

#include "model/GrainVoice.h"
#include "model/AudioFileSet.h"
#include "model/Scene.h"
#include "dsp/Window.h"
#include "theglobals.h"

extern unsigned int samp_rate;

//-------------------AUDIO----------------------------------------------------//

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
GrainVoice::~GrainVoice()
{
    if (playPositions != NULL)
        delete[] playPositions;

    if (playVols != NULL)
        delete[] playVols;

    if (chanMults)
        delete[] chanMults;

    if (queuedChanMults)
        delete[] queuedChanMults;
}


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

GrainVoice::GrainVoice(VecSceneSound *soundSet, float durationMs, float thePitch)
{


    // store pointer to external vector of sound files
    theSounds = soundSet;

    // get number of loaded sounds
    unsigned numSounds = (unsigned)soundSet->size();

    // set play positions to -1 for all
    // note - will have to handle files being added at runtime later if it becomes a feature
    if (numSounds > 0) {
        playPositions = new double[numSounds];
        playVols = new double[numSounds];
        // initialize - (-1 signifies that sound should not be played)
        for (int i = 0; i < soundSet->size(); i++) {
            playPositions[i] = -1.0;
            playVols[i] = 0.0;
        }
    }
    else {
        playPositions = NULL;
    }

    // playing status init
    playingState = false;


    // direction
    if (randf() < 0.5)
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
    queuedChanMults = new double[MY_CHANNELS];
    chanMults = new double[MY_CHANNELS];
    // set panning values - all channels active by default
    for (int i = 0; i < MY_CHANNELS; i++) {
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
    winDurationSamps = ceil(duration * ::samp_rate * (double)0.001);
    winInc = (double)WINDOW_LEN / winDurationSamps;
}


//-----------------------------------------------------------------------------
// Turn on grain.
// input args = position and volume vectors in sound rect space
// returns whether or not grain is awaiting play.
// parent cloud will wait to play this voice if the voice is still
// this should not be an issue unless the overlap value is erroneous
//-----------------------------------------------------------------------------
bool GrainVoice::playMe(double *startPositions, double *startVols)
{

    if (playingState == false) {
        // next buffer call will play
        playingState = true;

        // grab queued params if changed
        if (newParam == true)
            updateParams();

        // convert relative start positions to sample locations

        activeSounds.clear();

        VecSceneSound &sounds = *theSounds;
        for (int i = 0, n = sounds.size(); i < n; i++) {
            if (startPositions[i] != -1) {
                activeSounds.push_back(i);
                playPositions[i] =
                    floor(startPositions[i] * (sounds[i]->sample->frames - 1));
                playVols[i] = startVols[i];
            }
        }

        // initialize window reader index
        winReader = 0;
        return false;
    }
    else {
        // debug
        // cout << "Grain triggered too soon..." << endl;
        return true;
    }
}


//-----------------------------------------------------------------------------
// Find out if grain is currently on
//-----------------------------------------------------------------------------
bool GrainVoice::isPlaying()
{
    return playingState;
}


//-----------------------------------------------------------------------------
// Set channel multipliers
//-----------------------------------------------------------------------------
void GrainVoice::setChannelMultipliers(double *multipliers)
{
    for (int i = 0; i < MY_CHANNELS; i++) {
        queuedChanMults[i] = multipliers[i];
    }
    newParam = true;
}


//-----------------------------------------------------------------------------
// Set channel multipliers
//-----------------------------------------------------------------------------
void GrainVoice::setVolume(float theVolNormed)
{
    float pVol = fabs(theVolNormed);
    queuedLocalAtten = pVol;
}

//-----------------------------------------------------------------------------
// Get channel multipliers
//-----------------------------------------------------------------------------
float GrainVoice::getVolume()
{
    return localAtten;
}

//-----------------------------------------------------------------------------
// Set duration (effective on next trigger)
//-----------------------------------------------------------------------------
void GrainVoice::setDurationMs(float dur)
{
    // get absolute value
    queuedDuration = fabs(dur);
    if (queuedDuration != duration)
        newParam = true;
}


//-----------------------------------------------------------------------------
// Set pitch (effective on next trigger)
//-----------------------------------------------------------------------------
void GrainVoice::setPitch(float newPitch)
{
    // get absolute value
    queuedPitch = newPitch;
    if (queuedPitch != pitch)
        newParam = true;
}

float GrainVoice::getPitch()
{
    if (queuedPitch != pitch)
        return queuedPitch;
    else
        return pitch;
}


//-----------------------------------------------------------------------------
// Update params
//-----------------------------------------------------------------------------
void GrainVoice::updateParams()
{
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
    for (int i = 0; i < MY_CHANNELS; i++) {
        chanMults[i] = queuedChanMults[i];
    }

    // all params have been updated
    newParam = false;
}


//-----------------------------------------------------------------------------
// Set window type (effective on next trigger)
//-----------------------------------------------------------------------------

void GrainVoice::setWindow(unsigned int theType)
{
    queuedWindowType = theType;
    if (queuedWindowType != windowType)
        newParam = true;
}


//-----------------------------------------------------------------------------
// Set direction (effective on next trigger)
//-----------------------------------------------------------------------------

void GrainVoice::setDirection(float thedir)
{
    queuedDirection = thedir;
    if (queuedDirection != direction)
        newParam = true;
}


//-----------------------------------------------------------------------------
// Compute next sub buffer of audio
//-----------------------------------------------------------------------------


void GrainVoice::nextBuffer(double *accumBuff, unsigned int numFrames,
                            unsigned int bufferOffset, int name)
{

    // fill stereo accumulation buffer.  note, buffer output must be interlaced
    // ch1,ch2,ch1,ch2, etc... and playPositions are in frames, NOT SAMPLES.

    // only go through this ordeal if grain is active
    if (playingState == true) {
        // initialize local vars
        VecSceneSound &sounds = *theSounds;

        // linear interp coeff
        double nu = 0.0;

        // next window value
        double nextMult = 0.0;

        // ref idx for left bound of interp
        double flooredIdx = 0;

        // next file index
        int nextSound = -1;

        // waveform params
        double *wave = NULL;
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
        for (int i = 0; i < numFrames; i++) {

            // Window multiplier - Get next val from window and check to see if we've reached the end
            if (winReader > (WINDOW_LEN - 1)) {
                nextMult = (double)0.0;
                winReader = 0;
                playingState = false;
                return;
            }
            else {
                // interpolated read from window buffer
                flooredIdx = floor(winReader);
                nu = winReader - flooredIdx;  // interp coeff
                // interpolated read (lin)
                nextMult = ((double)1.0 - nu) * window[(unsigned long)flooredIdx] +
                           nu * window[(unsigned long)flooredIdx + 1];
                // increment reader
                winReader += winInc;
            }


            // reinit sound accumulators for mono and stereo files to prepare for this frame
            monoWaveVal = 0.0;
            stereoLeftVal = 0.0;
            stereoRightVal = 0.0;

            // Get next audio frame data (accumulate from each sound under grain)
            //-- REMEMBER - playPositions are in frames, not samples
            for (int j = 0; j < activeSounds.size(); j++) {

                nextSound = activeSounds[j];
                pos = playPositions[nextSound];  // get start position
                atten = playVols[nextSound];  // get volume relative to rect


                // if sound is in play,sample it
                if (pos > 0) {
                    // sound vars
                    AudioFile *af = sounds[nextSound]->sample;
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
                             accumBuff[(bufferOffset + i)*MY_CHANNELS] += nextAmp;
                            //copy to other channels for mono
                            for (int k = 1; k < MY_CHANNELS; k++){
                                accumBuff[(bufferOffset + i) * MY_CHANNELS + k] += nextAmp;
                            }
                             */

                            // advance after each stereo frame (do calc twice for mono)
                            playPositions[nextSound] += playInc;
                        }
                        else {
                            // not playing anymore
                            playPositions[nextSound] = -1.0;
                        }
                        break;
                    case 2:  // stereo

                        // make sure we are still in sound
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
                            accumBuff[(bufferOffset + i)*MY_CHANNELS] += (((double)
                            1.0 - nu)*wave[(unsigned long)flooredIdx*2] + nu * wave[(unsigned long)(flooredIdx + 1)*2])*nextMult*atten;

                            //right channel
                            accumBuff[(bufferOffset + i) * MY_CHANNELS + 1] +=
                            (((double) 1.0 - nu)*wave[(unsigned long)flooredIdx*2 + 1] + nu * wave[(unsigned long)(flooredIdx + 1)*2 + 1])*nextMult*atten;
                             */

                            // advance after each stereo frame (do calc twice for mono)
                            playPositions[nextSound] += playInc;
                        }
                        else {
                            // not playing anymore
                            playPositions[nextSound] = -1.0;
                        }
                        break;
                        // don't handle numbers of channels > 2
                    default:
                        break;
                    }  // end switch channels

                }  // end position check

            }  // end accumulation for current frame

            // spatialize output
            for (int k = 0; k < MY_CHANNELS; k++) {
                // preserve stereo waveform L/R for now and just sample alternate channels in "AROUND" case (see GrainCluster.cpp updateSpatialization routine)
                if ((k % 2) == 0) {
                    accumBuff[(bufferOffset + i) * MY_CHANNELS + k] +=
                        (stereoLeftVal + monoWaveVal) * chanMults[k] * localAtten;
                }
                else {
                    accumBuff[(bufferOffset + i) * MY_CHANNELS + k] +=
                        (stereoRightVal + monoWaveVal) * chanMults[k] * localAtten;
                }

                // clip if needed
                if (accumBuff[(bufferOffset + i) * MY_CHANNELS + k] > 1.0) {
                    accumBuff[(bufferOffset + i) * MY_CHANNELS + k] = 1.0;
                }
                else {
                    if (accumBuff[(bufferOffset + i) * MY_CHANNELS + k] < -1.0) {
                        accumBuff[(bufferOffset + i) * MY_CHANNELS + k] = -1.0;
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
