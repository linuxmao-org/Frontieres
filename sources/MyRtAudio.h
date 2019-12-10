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
#include "RtMidi.h"
#include "theglobals.h"
#include <vector>
#include <memory>
#include <cstdio>
#include <cstdlib>

class AbstractAudio {
protected:
    AbstractAudio() {}

public:
    virtual ~AbstractAudio() {}

    // set the audio callback and start the audio stream
    typedef void (*AudioCallback)(
        const BUFFERPREC *in, BUFFERPREC *out, unsigned int numFrames, void *userData);
    virtual bool openStream(AudioCallback callback, void *userData) = 0;

    // connect outputs
    virtual void connectOutputs() = 0;

    // report the current sample rate
    virtual unsigned int getSampleRate() = 0;

    // report the current buffer size
    virtual unsigned int getBufferSize() = 0;

    // get the obtained channel count
    virtual unsigned int getObtainedInputChannels() = 0;

    // get the obtained channel count
    virtual unsigned int getObtainedOutputChannels() = 0;

    // start audio stream
    virtual bool startStream() = 0;

    // stop audio stream
    virtual bool stopStream() = 0;

    // get the stream latency
    virtual int getStreamLatency() = 0;

    // open MIDI input
    typedef void (*ReceiveMidi)(const unsigned char *msg, unsigned length, void *userData);
    virtual int openMidiInput(ReceiveMidi receive, unsigned bufferSize, void *userData) = 0;

private:
    AbstractAudio(const AbstractAudio &) = delete;
    AbstractAudio &operator=(const AbstractAudio &) = delete;
};

//------------------------------------------------------------------------------
class MyRtAudio : public AbstractAudio {

public:
    // constructor - args =
    MyRtAudio(unsigned int numIns, unsigned int numOuts);

    // set the audio callback and start the audio stream
    bool openStream(AudioCallback callback, void *userData) override;
    void connectOutputs() override {}

    unsigned int getSampleRate() override;
    unsigned int getBufferSize() override;
    unsigned int getObtainedInputChannels() override;
    unsigned int getObtainedOutputChannels() override;
    bool startStream() override;
    bool stopStream() override;
    int getStreamLatency() override;
    int openMidiInput(ReceiveMidi receive, unsigned bufferSize, void *userData) override;

private:
    static void errorCallback(RtAudioError::Type type, const std::string &errorText);

    // rtaudio pointer
    std::unique_ptr<RtAudio> audio;
    std::unique_ptr<RtMidiIn> midiIn;
    unsigned int numInputs;
    unsigned int numOutputs;

    // buffer size, sample rate, rt audio format
    // note: buffer size is handled as pointer to unsigned int passed in externally. this allows shared access, but is risky.
    unsigned int myBufferSize;
    unsigned int mySRate;

    static bool hasError;

    AudioCallback audioCallback = nullptr;
    void *audioUserData = nullptr;
    ReceiveMidi receiveMidiIn = nullptr;
    void *receiveMidiInUserData = nullptr;

    static int rtaudioCallback(
        void *outputBuffer, void *, unsigned int numFrames,
        double, RtAudioStreamStatus, void *userData);
    static void midiInCallback(double timeStamp, std::vector<unsigned char> *message, void *userData);
};

//------------------------------------------------------------------------------
#include <jack/jack.h>
#include <jack/midiport.h>

class MyRtJack : public AbstractAudio {

public:
    // constructor - args =
    MyRtJack(unsigned int numIns, unsigned int numOuts);

    // set the audio callback and start the audio stream
    bool openStream(AudioCallback callback, void *userData) override;
    void connectOutputs() override;

    unsigned int getSampleRate() override;
    unsigned int getBufferSize() override;
    unsigned int getObtainedInputChannels() override;
    unsigned int getObtainedOutputChannels() override;
    bool startStream() override;
    bool stopStream() override;
    int getStreamLatency() override;
    int openMidiInput(ReceiveMidi receive, unsigned bufferSize, void *userData) override;

private:
    struct JackClientDeleter { void operator()(jack_client_t *x) const noexcept { jack_client_close(x); } };

    AudioCallback audioCallback = nullptr;
    void *audioUserData = nullptr;
    ReceiveMidi receiveMidiIn = nullptr;
    void *receiveMidiInUserData = nullptr;

    unsigned numInputs = 0;
    unsigned numOutputs = 0;
    std::unique_ptr<float[]> inputBuffer;
    std::unique_ptr<float[]> outputBuffer;

    std::vector<jack_port_t *> inputs;
    std::vector<jack_port_t *> outputs;
    jack_port_t *midiIn = nullptr;

    std::unique_ptr<jack_client_t, JackClientDeleter> client;

    static int jackProcess(jack_nframes_t nframes, void *userData);
};

#endif
