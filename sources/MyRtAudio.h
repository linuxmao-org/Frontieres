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
//  MyRtAudio.h
//  header for MyRtAudio RtAudio wrapper class
//
//
//  Created by Christopher Carlson on 10/29/2011.
//

#ifndef __MYRTAUDIO_H
#define __MYRTAUDIO_H


#include "RtAudio.h"
#include <cstdio>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>

class MyRtAudio {

public:
    // destructor
    virtual ~MyRtAudio();

    // constructor - args =
    MyRtAudio(unsigned int numIns, unsigned int numOuts,
              unsigned int *bufferSize, RtAudioFormat format, bool showWarnings);


    // set the audio callback and start the audio stream
    void openStream(RtAudioCallback callback);

    // report the current sample rate
    unsigned int getSampleRate();

    // report the current buffer size
    unsigned int getBufferSize();

    // start audio stream
    void startStream();

    // stop audio stream
    void stopStream();

    // close audio stream
    void closeStream();

    // report the stream latency
    void reportStreamLatency();


private:
    // rtaudio pointer
    RtAudio *audio;
    unsigned int numInputs;
    unsigned int numOutputs;

    // buffer size, sample rate, rt audio format
    // note: buffer size is handled as pointer to unsigned int passed in externally. this allows shared access, but is risky.
    unsigned int *myBufferSize;
    unsigned int mySRate;
    RtAudioFormat myFormat;
};

#endif
