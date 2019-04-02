//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     https://github.com/linuxmao-org/Frontieres
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
// Copyright (C) 2018  Olivier Flatres
// Copyright (C) 2018  Jean Pierre Cimalando
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

#ifndef SCENE_H
#define SCENE_H

#include "theglobals.h"
#include <string>
#include <vector>
#include <memory>
#include "model/MidiBank.h"
#include "model/MidiInstrum.h"

struct SceneSample;
struct SceneCloud;
struct Sample;
struct SampleVis;
struct Cloud;
struct CloudVis;
class SampleSet;
class QFile;

typedef std::vector<std::unique_ptr<SceneSample>> VecSceneSample;
typedef std::vector<std::unique_ptr<SceneCloud>> VecSceneCloud;

extern unsigned int samp_rate;

class Scene
{
public:
    // destructor
    ~Scene();

    // constructor
    Scene();

    // window to choose a scene file
    static std::string askNameScene(FileDirection direction);
    static std::string askNameCloud(FileDirection direction);
    void clear();

    bool load(QFile &sceneFile);
    bool loadCloudDefault(QFile &cloudFile);
    bool save(QFile &sceneFile);
    bool saveCloud(QFile &cloudFile, SceneCloud *selectedCloudSave);

    bool loadSampleSet(bool interactive);
    Sample *loadNewSample(const std::string &path);
    bool removeSampleAt(unsigned index);
    void removeSampleIfNotUsed(Sample *sampleToRemove);
    void addAudioPath(const std::string &path);

    void addSampleVis(Sample *sampleToAdd);
    void addNewCloud(int numGrains);
    void addNewMovingCloud(int numGrains);

    // init default cloud params
    void initDefaultCloudParams();

    int getNumCloud(SceneCloud *cloudCurrent);

    void changeParamEnvelopeVolume ( SceneCloud *selectedCloud);

    SceneSample *selectedSample();
    SceneCloud *selectedCloud();

    SceneCloud *findCloudById(unsigned id);

    // handle deselections
    void deselect(int shapeType);

    // data contents
    std::vector<std::string> m_audioPaths;
    VecSceneSample m_samples;
    VecSceneCloud m_clouds;

    // Samples
    std::unique_ptr<SampleSet> m_sampleSet;

    // selection helper vars
    int m_selectedCloud = -1;
    int m_selectedSample = -1;
    int m_selectionIndex = 0;
    std::vector<int> m_selectionIndices;

    // midi notes
    void midiNoteOn (int midiChannelToPlay, int midiNoteToPlay, int midiVeloToPlay);
    void midiNoteOff (int midiChannelToStop, int midiNoteToStop);

    // midi combinations bank and channels
    MidiBank m_midiBank;
    MidiInstrument m_midiInstrument;
};

struct SceneSample {
    std::string name;
    Sample *sample = nullptr;
    std::unique_ptr<SampleVis> view;

    ~SceneSample();
};

struct SceneCloud {
    std::unique_ptr<Cloud> cloud;
    std::unique_ptr<CloudVis> view;

    ~SceneCloud();
};

#endif // SCENE_H
