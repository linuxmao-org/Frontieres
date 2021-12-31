//------------------------------------------------------------------------------
// FRONTIÈRES :  An interactive granular sampler.
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
//  Sample.cpp
//  Frontières
//
//  Created by Christopher Carlson on 11/21/11.
//

#include "Sample.h"
#include "visual/SampleVis.h"
#include <minimp3_custom.h>
#include <QFileInfo>
#include <algorithm>
#include <stdexcept>
#include <soxr.h>
#include <math.h>

extern unsigned int samp_rate;
extern int timeInputs;

// ids
static unsigned int sampleId = 0;

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
SampleSet::~SampleSet()
{
    // delete each audio file object (and corresponding buffer, etc.)
    for (Sample * sample : fileSet)
        delete sample;
}

//---------------------------------------------------------------------------
// constructor
//---------------------------------------------------------------------------
SampleSet::SampleSet()
{
}

//---------------------------------------------------------------------------
// Access file set externally (note this is not thread safe)
//---------------------------------------------------------------------------
vector<Sample *> *SampleSet::getFileVector()
{
    return &this->fileSet;
}


//---------------------------------------------------------------------------
//  Search path and load all audio files into memory.  Convert to mono (L)
//  if needed.
//---------------------------------------------------------------------------
int SampleSet::loadFileSet(const std::string &localPath, std::vector<Sample *> *loaded)
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
    if (dir != nullptr) {

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
            Sample *af = loadFile(myPath.c_str());
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

Sample *SampleSet::loadFile(const std::string &path)
{
    // isolate the file name part
    std::string theFileName = QFileInfo(QString::fromStdString(path)).fileName().toStdString();

    // check if exists, if so return it
    for (Sample *existing : fileSet) {
        if (existing->name == theFileName)
            return existing;
    }

    // make the list of available file loaders
    Sample *(SampleSet::*loaders[])(const std::string &, const std::string &) = {
        &SampleSet::loadFileSndfile,
        &SampleSet::loadFileMp3,
    };
    size_t num_loaders = sizeof(loaders) / sizeof(loaders[0]);

    // try with all loaders possible
    Sample *sample = nullptr;
    for (size_t i = 0; i < num_loaders && !sample; ++i)
        sample = (this->*loaders[i])(path, theFileName);

    if (!sample)
        return nullptr;

    fileSet.push_back(sample);

    if (sample->sampleRate != ::samp_rate) {
        printf("Resample from %i to %i\n", sample->sampleRate, ::samp_rate);
        sample->resampleTo(::samp_rate);
    }

    return sample;
}

Sample *SampleSet::loadInput(const int n_input)
{
    //numFrames c'est la durée du bloc à traiter exprimée en échantillons.
    //Tu as la durée du bloc en secondes qui s'exprime T = numFrames / sampleRate.
    //(qu'on appelle aussi la latence)
    string n_filename = "input " + to_string(n_input);
    string n_path = "";
    unsigned long n_frames = ::timeInputs * ::samp_rate;
    unsigned int n_samplerate = ::samp_rate;
    Sample *n_sample = new Sample(n_filename, n_path, 1,
                                n_frames, n_samplerate,
                                new BUFFERPREC[n_frames]());
    fileSet.push_back(n_sample);
    n_sample->isInput = true;
    n_sample->myInputNumber = n_input;
    return n_sample;
}

void SampleSet::removeSample(Sample *sampleToRemove)
{
    unsigned index = 0;
    unsigned count = fileSet.size();

    while (index < count && fileSet[index] != sampleToRemove)
        ++index;

    if (index == count)
        return;  // not found

    fileSet.erase(fileSet.begin() + index);
    delete sampleToRemove;
}

Sample::Sample(string myName, string thePath, unsigned int numChan, unsigned long numFrames, unsigned int srate, BUFFERPREC *theWave)
{
    // cout << numFrames << endl;
    this->name = myName;
    this->path = thePath;
    this->frames = numFrames;
    this->channels = numChan;
    this->sampleRate = srate;
    this->wave = theWave;
    this->isInput = false;
    myId = ++sampleId;
}

Sample::~Sample()
{
    if (wave != NULL) {
        delete[] wave;
    }
}

void Sample::nextBuffer(const BUFFERPREC *accumBuff, unsigned int numFrames)
{
    int offsetWave = ptrLagWave;
    if (isInput) {
        int l_lenghtWave = ::timeInputs * int (::samp_rate);
        for (int i = 0; i < numFrames - 1; i = i + 1) {
            //généralement i * n + c avec c le canal, n le nombre total de canaux, et i dans [0,numFrames-1]

            wave[i + offsetWave] = accumBuff[(i * theInChannelCount) + (myInputNumber - 1)];

            ptrLagWave = ptrLagWave + 1;
            if (ptrLagWave > l_lenghtWave) {
                ptrLagWave = 0;
                offsetWave = offsetWave - l_lenghtWave;
            }
        }
    }
}

void Sample::resampleTo(unsigned int newRate)
{
    unsigned channels = this->channels;

    unsigned oldFrames = frames;
    unsigned oldRate = sampleRate;
    BUFFERPREC *oldWave = wave;

    unsigned newFrames = ceil((double)oldFrames * newRate / oldRate);
    BUFFERPREC *newWave = new BUFFERPREC[channels * newFrames];

    soxr_io_spec_t io_spec = soxr_io_spec(SOXR_FLOAT32_I, SOXR_FLOAT32_I);
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

Sample *SampleSet::loadFileSndfile(const std::string &path, const std::string &filename)
{
    // temp struct that will hold the details of the file being read (sample rate, num channels. etc.)
    SF_INFO sfinfo;

    // a pointer to the audio file to read
    SNDFILE *infile;

    // show the current file path
    fprintf(stderr, "[Snd] Loading '%s'... \n", filename.c_str());

    // open the file for reading and get the header info
    if (!(infile = sf_open(path.c_str(), SFM_READ, &sfinfo))) {
        fprintf(stderr, "[Snd] Not able to open input file '%s'.\n", filename.c_str());
        // Print the error message from libsndfile.
        fprintf(stderr, "[Snd] %s\n", sf_strerror(NULL));
        // skip to next
        return nullptr;
    }

    // explore the file's info
    if (false) {
        cout << "  Channels: " << sfinfo.channels << endl;
        cout << "  Frames: " << sfinfo.frames << endl;
        cout << "  Sample Rate: " << sfinfo.samplerate << endl;
        cout << "  Format: " << sfinfo.format << endl;
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

    Sample *sample = new Sample(filename, path, sfinfo.channels,
                                sfinfo.frames, sfinfo.samplerate,
                                new BUFFERPREC[fullSize]());


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
                sample->wave[counter] = stereoBuff[i] * globalAtten;
                counter++;
            }
        }
        if (count == 0) {
            empty = true;
            continue;
        }
    } while (!empty);

    // don't forget to close the file
    sf_close(infile);

    return sample;
}

Sample *SampleSet::loadFileMp3(const std::string &path, const std::string &filename)
{
    mp3dec_t mp3d;
    mp3dec_file_info_t info;

    // show the current file path
    fprintf(stderr, "[Mp3] Loading '%s'... \n", filename.c_str());

    if (mp3dec_load(&mp3d, path.c_str(), &info, nullptr, nullptr)) {
        fprintf(stderr, "[Mp3] Not able to open input file '%s'.\n", filename.c_str());
        return nullptr;
    }

    size_t channels = info.channels;
    size_t frames = info.samples / info.channels;
    size_t samples = channels * frames;

    BUFFERPREC *wave = new BUFFERPREC[samples];
    Sample *sample = new Sample(filename, path, channels, frames, info.hz, wave);

    std::copy(info.buffer, info.buffer + samples, wave);
    free(info.buffer);

    return sample;
}

// print information
void Sample::describe(std::ostream &out)
{
    out << "- name : " << this->name << "\n";
    out << "- path : " << this->path << "\n";
}

void Sample::registerSampleVis(SampleVis *sampleVisToRegister)
{
    mySampleVis = sampleVisToRegister;
}
