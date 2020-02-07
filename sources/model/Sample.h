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
//  Sample.h
//  Frontières
//
//  Created by Christopher Carlson on 11/21/11.
//


#ifndef SAMPLE_H
#define SAMPLE_H

#include <vector>
#include <string>
#include "sndfile.h"
#include "dirent.h"
#include <iostream>
#include "theglobals.h"
using namespace std;

// forward declarations
class SampleVis;

// basic encapsulation of an audio file
struct Sample {

    // constructor
    Sample(string myName, string thePath, unsigned int numChan,
              unsigned long numFrames, unsigned int srate, BUFFERPREC *theWave);
    // destructor
    ~Sample();

    // operations on input audio buffer
    void nextBuffer(const BUFFERPREC *accumBuff, unsigned int numFrames);

    void resampleTo(unsigned int newRate);

    // print information
    void describe(std::ostream &out);

    void registerSampleVis(SampleVis *sampleVisToRegister);

    SampleVis *mySampleVis = nullptr;

    unsigned int myId;  // unique id

    string name;
    string path;
    BUFFERPREC *wave;
    unsigned long frames;
    unsigned int channels;
    unsigned int sampleRate;
    bool isInput = false;
    bool inputToRecord = false;
    int myInputNumber = -1;
    int ptrLagWave = 0; // for circular waves to use input waves
};


class SampleSet {

public:
    virtual ~SampleSet();

    // constructor
    SampleSet();

    // read in all audio files contained in
    int loadFileSet(const std::string &path, std::vector<Sample *> *loaded = nullptr);

    // load a single file in the collection, or return if the same name already exists
    Sample *loadFile(const std::string &path);

    // load from input
    Sample *loadInput(const int n_input);

    // remove a single sample from the set
    void removeSample(Sample *sampleToRemove);

    // return the audio vector- note, the intension is for the files to be
    // read only.  if write access is needed in the future - thread safety will
    // need to be considered
    vector<Sample *> *getFileVector();


private:
    vector<Sample *> fileSet;

    // try to load a single file with Sndfile
    Sample *loadFileSndfile(const std::string &path, const std::string &filename);

    // try to load a single file with minimp3
    Sample *loadFileMp3(const std::string &path, const std::string &filename);
};


#endif
