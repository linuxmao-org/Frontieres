//------------------------------------------------------------------------------
// FRONTIÈRES :  An interactive granular sampler.
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
//  AudioFileSet.cpp
//  Frontières
//
//  Created by Christopher Carlson on 11/21/11.
//

#include "AudioFileSet.h"
#include <QFileInfo>
#include <stdexcept>
#include <soxr.h>
#include <math.h>

extern unsigned int samp_rate;

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
AudioFileSet::~AudioFileSet()
{
    // delete each audio file object (and corresponding buffer, etc.)
    for (AudioFile * audioFile : fileSet)
        delete audioFile;
}

//---------------------------------------------------------------------------
// constructor
//---------------------------------------------------------------------------
AudioFileSet::AudioFileSet()
{
}

//---------------------------------------------------------------------------
// Access file set externally (note this is not thread safe)
//---------------------------------------------------------------------------
vector<AudioFile *> *AudioFileSet::getFileVector()
{
    return &this->fileSet;
}


//---------------------------------------------------------------------------
//  Search path and load all audio files into memory.  Convert to mono (L)
//  if needed.
//---------------------------------------------------------------------------
int AudioFileSet::loadFileSet(const std::string &localPath, std::vector<AudioFile *> *loaded)
{
    // read through loop directory and attempt to load audio files into buffers

    // using dirent
    DIR *dir;
    struct dirent *ent;

    // counter var for number of files loaded
    int fileCounter = 0;

    // get directory
    dir = opendir(localPath.c_str());

    // if directory exists - jump on in
    if (dir != NULL) {

        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {

            // get filename
            string theFileName = ent->d_name;

            // skip cd, top directory, other files
            if (!theFileName.empty() && theFileName.front() == '.')
                continue;

            // construct path
            string myPath = localPath + '/' + theFileName;

            // load it
            AudioFile *af = loadFile(myPath.c_str());
            if (!af)
                continue;

            // increment the file counter
            fileCounter++;

            if (loaded)
                loaded->push_back(af);
        }

        // close the directory that we've been navigating
        closedir(dir);
    }
    else {
        /* could not open directory */
        perror("");
        return 1;
    }

    return 0;
}

AudioFile *AudioFileSet::loadFile(const std::string &path)
{
    // temp struct that will hold the details of the file being read (sample rate, num channels. etc.)
    SF_INFO sfinfo;

    // a pointer to the audio file to read
    SNDFILE *infile;

    // isolate the file name part
    std::string theFileName = QFileInfo(QString::fromStdString(path)).fileName().toStdString();

    // check if exists, if so return it
    for (AudioFile *existing : fileSet) {
        if (existing->name == theFileName)
            return existing;
    }

    // open the file for reading and get the header info
    if (!(infile = sf_open(path.c_str(), SFM_READ, &sfinfo))) {
        printf("Not able to open input file %s.\n", theFileName.c_str());
        // Print the error message from libsndfile.
        puts(sf_strerror(NULL));
        // skip to next
        return nullptr;
    }

    // show the current file path (comment this eventually)
    printf("Loading '%s'... \n", theFileName.c_str());

    // explore the file's info
    cout << "  Channels: " << sfinfo.channels << endl;
    cout << "  Frames: " << sfinfo.frames << endl;
    cout << "  Sample Rate: " << sfinfo.samplerate << endl;
    cout << "  Format: " << sfinfo.format << endl;

    // warn about sampling rate incompatibility
    if (sfinfo.samplerate != ::samp_rate) {
        printf("'%s' is sampled at a different rate from the current "
               "sample rate of %i\n",
               theFileName.c_str(), ::samp_rate);
    }

    // MONO CONVERSION SET ASIDE FOR NOW...  number of channels for each file is dealt with
    // by external audio processing algorithms
    // notify user of mono conversion
    //            if (sfinfo.channels > 1){
    //  printf("\nWARNING: '%s' will be converted to mono using left channel \n\n",theFileName.c_str());
    //            }

    // read the contents of the file in chunks of 512 samples
    const int buffSize = 512;

    // we will convert the file to mono - left channel, but store the stereo frames temporarily
    double stereoBuff[buffSize];

    // length of mono buffer
    //            unsigned int monoLength = sfinfo.frames/sfinfo.channels;


    // allocate memory for the new waveform (new audio file entry in the fileSet)
    // length corresponds to the number of frames * number of channels (1 frame contains L, R pair or chans 1,2,3...)
    unsigned long fullSize = sfinfo.frames * sfinfo.channels;

    AudioFile *audioFile = new AudioFile(theFileName, path, sfinfo.channels,
                                         sfinfo.frames, sfinfo.samplerate,
                                         new double[fullSize]());
    fileSet.push_back(audioFile);


    // accumulate the samples
    unsigned long counter = 0;
    bool empty = false;
    do {
        // read the samples as doubles
        sf_count_t count = sf_read_double(infile, &stereoBuff[0], buffSize);
        // break if we reached the end of the file
        // print the sample values to screen
        for (int i = 0; i < buffSize; i++) {
            if (counter < fullSize) {
                // if ((i % sfinfo.channels) == 0){
                audioFile->wave[counter] = stereoBuff[i] * globalAtten;
                counter++;
            }
        }
        if (count == 0) {
            empty = true;
            continue;
        }
    } while (!empty);

    if (sfinfo.samplerate != ::samp_rate) {
        printf("Resample to %i\n", ::samp_rate);
        audioFile->resampleTo(::samp_rate);
    }

    cout << counter << endl;

    // don't forget to close the file
    sf_close(infile);

    return audioFile;
}

void AudioFileSet::removeSample(AudioFile *sample)
{
    unsigned index = 0;
    unsigned count = fileSet.size();

    while (index < count && fileSet[index] != sample)
        ++index;

    if (index == count)
        return;  // not found

    fileSet.erase(fileSet.begin() + index);
    delete sample;
}

void AudioFile::resampleTo(unsigned int newRate)
{
    unsigned channels = this->channels;

    unsigned oldFrames = frames;
    unsigned oldRate = sampleRate;
    SAMPLE *oldWave = wave;

    unsigned newFrames = ceil((double)oldRate * newRate / oldRate);
    SAMPLE *newWave = new SAMPLE[channels * newFrames];

    soxr_io_spec_t io_spec = soxr_io_spec(MY_RESAMPLER_FORMAT_I, MY_RESAMPLER_FORMAT_I);
    soxr_quality_spec_t quality_spec = soxr_quality_spec(SOXR_VHQ, 0);
    soxr_runtime_spec_t runtime_spec = soxr_runtime_spec(2);

    size_t idone = 0;
    size_t odone = 0;
    soxr_error_t err = soxr_oneshot(oldRate, newRate, channels, oldWave,
                                    oldFrames, &idone, newWave, newFrames, &odone,
                                    &io_spec, &quality_spec, &runtime_spec);
    if (err)
        throw std::runtime_error("could not resample: libsoxr error");
    newFrames = odone;

    delete[] wave;
    wave = newWave;
    frames = newFrames;
    sampleRate = newRate;
}

// print information
void AudioFile::describe(std::ostream &out)
{
    out << "- name : " << this->name << "\n";
    out << "- path : " << this->path << "\n";
}
