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
struct SceneTrigger;
struct ScenePhrase;
struct SceneScale;
struct Sample;
struct SampleVis;
struct Cloud;
struct CloudVis;
struct Trigger;
struct TriggerVis;
struct Phrase;
struct Scale;
class SampleSet;
class QFile;

typedef std::vector<std::unique_ptr<SceneSample>> VecSceneSample;
typedef std::vector<std::unique_ptr<SceneCloud>> VecSceneCloud;
typedef std::vector<std::unique_ptr<SceneTrigger>> VecSceneTrigger;
typedef std::vector<std::unique_ptr<ScenePhrase>> VecScenePhrase;
typedef std::vector<std::unique_ptr<SceneScale>> VecSceneScale;

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
    static std::string askNameTrajectory(FileDirection direction);
    void clear();

    bool load(QFile &sceneFile);
    bool loadCloudDefault(QFile &cloudFile);
    bool save(QFile &sceneFile);
    bool saveCloud(QFile &cloudFile, SceneCloud *selectedCloudSave);
    bool saveCloudRecordedTrajectory(QFile &trajectoryFile, SceneCloud *selectedCloudSave);
    bool loadCloudRecordedTrajectory(QFile &trajectoryFile, SceneCloud *selectedCloudLoad);
    bool saveTriggerRecordedTrajectory(QFile &trajectoryFile, SceneTrigger *selectedTriggerSave);
    bool loadTriggerRecordedTrajectory(QFile &trajectoryFile, SceneTrigger *selectedTriggerLoad);

    bool loadSampleSet(bool interactive);
    Sample *loadNewSample(const std::string &path);
    Sample *loadNewInput(const int l_input);
    bool removeSampleAt(unsigned index);
    void removeSampleIfNotUsed(Sample *sampleToRemove);
    void addAudioPath(const std::string &path);

    void addSampleVis(Sample *sampleToAdd);
    void addNewCloud(int numGrains);
    void addNewTrigger();
    void addNewPhrase();
    void addNewScale();

    // init default cloud params
    void initDefaultCloudParams();

    int getNumCloud(SceneCloud *cloudCurrent);
    int getNumTrigger(SceneTrigger *triggerCurrent);

    void changeParamEnvelopeVolume ( SceneCloud *selectedCloud);

    SceneSample *selectedSample();
    SceneCloud *selectedCloud();
    SceneTrigger *selectedTrigger();

    SceneTrigger *findTriggerById(unsigned id);
    SceneCloud *findCloudById(unsigned id);
    ScenePhrase *findPhraseById(unsigned id);
    SceneScale *findScaleById(unsigned id);

    unsigned long getPhrasesSize();
    unsigned long getScalesSize();

    // handle deselections
    void deselect(int shapeType);

    // data contents
    std::vector<std::string> m_audioPaths;
    VecSceneSample m_samples;
    VecSceneCloud m_clouds;
    VecSceneTrigger m_triggers;
    VecScenePhrase m_phrases;
    VecSceneScale m_scales;

    // Samples
    std::unique_ptr<SampleSet> m_sampleSet;

    // selection helper vars
    int m_selectedCloud = -1;
    int m_selectedSample = -1;
    int m_selectedTrigger = -1;
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
    int myInputNumber = 0;
    Sample *sample = nullptr;
    std::unique_ptr<SampleVis> view;

    ~SceneSample();
};

struct SceneCloud {
    std::unique_ptr<Cloud> cloud;
    std::unique_ptr<CloudVis> view;

    ~SceneCloud();
};

struct SceneTrigger {
    std::unique_ptr<Trigger> trigger;
    std::unique_ptr<TriggerVis> view;

    ~SceneTrigger();
};

struct ScenePhrase {
    std::unique_ptr<Phrase> phrase;
};

struct SceneScale {
    std::unique_ptr<Scale> scale;
};


#endif // SCENE_H
