//------------------------------------------------------------------------------
// BORDERLANDS:  An interactive granular sampler.  
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



//-----------------------------------------------------------------------------
// name: MyRtAudio.cpp
// implementation of wrapper class for RtAudio
// author: Chris Carlson
//   date: fall 2011
//-----------------------------------------------------------------------------

#include "MyRtAudio.h"
using namespace std;


//destructor
MyRtAudio::~MyRtAudio()
{
    //cleanup   
    delete audio;
    //cerr << "rtaudio cleanup reached " << endl;
}

MyRtAudio::MyRtAudio(unsigned int numIns, unsigned int numOuts, unsigned int * bufferSize,  RtAudioFormat format,bool showWarnings)
{
    //configure RtAudio
    //create pointer to RtAudio object
    audio = NULL;
    //create the object
    try {
        audio = new RtAudio();
    } catch (RtError & err) {
        err.printMessage();
        exit(1);
    }
    
    //io
    numInputs = numIns;
    numOutputs = numOuts;
    
    //check audio devices
    if ( audio->getDeviceCount() < 1)
    {
        //no audio available
        cout << "No Audio Devices Found!" << endl;
        exit( 1 );
    }
    const RtAudio::DeviceInfo &info = audio->getDeviceInfo( audio->getDefaultOutputDevice() );

    
    //allow RtAudio to print msgs to stderr
    audio->showWarnings( showWarnings );
    
    //store pointer to bufferSize
    myBufferSize = bufferSize;
    
    //set sample rate;
    mySRate = info.preferredSampleRate;
    
    //set format
    myFormat = format;
    

}


//set the audio callback and start the audio stream
void MyRtAudio::openStream( RtAudioCallback callback){
    
    //create stream options
    RtAudio::StreamOptions options;
    options.streamName = "Borderlands";
    
    RtAudio::StreamParameters iParams, oParams;
    //i/o params
    iParams.deviceId = audio->getDefaultInputDevice();
    iParams.nChannels = numInputs;
    iParams.firstChannel = 0;
    oParams.deviceId = audio->getDefaultOutputDevice();
    oParams.nChannels = numOutputs;
    oParams.firstChannel = 0;
    
    //open stream
    audio->openStream( &oParams, &iParams, RTAUDIO_FLOAT64, mySRate, myBufferSize, callback, NULL, &options); 

        
}

//report the current sample rate
unsigned int MyRtAudio::getSampleRate()
{
    return mySRate;
}


//report the current buffer size
unsigned int MyRtAudio::getBufferSize(){
        
    cerr << "Buffer size defined by RtAudio: " << (*myBufferSize) << endl;
    return (*myBufferSize);

}           


//start rtaudio stream
void MyRtAudio::startStream(){
    //start audio stream
    audio->startStream();
    
}

//stop rtaudio stream
void MyRtAudio::stopStream(){
    //try to stop audio stream
    try {
        audio->stopStream();
    } catch( RtError & err) {
        err.printMessage();
    }
}

//close audio stream
void MyRtAudio::closeStream(){
    audio->closeStream();
}


//report the stream latency
void MyRtAudio::reportStreamLatency(){
    //report latency
    cout << "Stream Latency: " << audio->getStreamLatency() << " frames" << endl;    
}














