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


//
//  AudioFileSet.h
//  Borderlands
//
//  Created by Christopher Carlson on 11/21/11.
//


#ifndef AUDIOFILESET_H
#define AUDIOFILESET_H

#include <vector>
#include <string>
#include "sndfile.h"
#include "dirent.h"
#include <iostream>
#include "theglobals.h"
using namespace std;


// basic encapsulation of an audio file
struct AudioFile {

    // constructor
    AudioFile(string myName, string thePath, unsigned int numChan,
              unsigned long numFrames, unsigned int srate, SAMPLE *theWave)
    {
        cout << numFrames << endl;
        this->name = myName;
        this->path = thePath;
        this->frames = numFrames;
        this->channels = numChan;
        this->sampleRate = srate;
        this->wave = theWave;
    }
    // destructor
    ~AudioFile()
    {
        if (wave != NULL) {
            delete[] wave;
        }
    }

    void resampleTo(unsigned int newRate);

    string name;
    string path;
    SAMPLE *wave;
    unsigned long frames;
    unsigned int channels;
    unsigned int sampleRate;
};


class AudioFileSet {

public:
    virtual ~AudioFileSet();

    // constructor
    AudioFileSet();

    // read in all audio files contained in
    int loadFileSet(string path);

    // return the audio vector- note, the intension is for the files to be
    // read only.  if write access is needed in the future - thread safety will
    // need to be considered
    vector<AudioFile *> *getFileVector();


private:
    vector<AudioFile *> *fileSet;
};


#endif
