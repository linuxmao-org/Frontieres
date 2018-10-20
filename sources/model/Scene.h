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
#include <mutex>
struct SceneSound;
struct SceneCloud;
struct AudioFile;
struct SoundRect;
struct GrainCluster;
struct GrainClusterVis;
class QFile;

typedef std::vector<std::unique_ptr<SceneSound>> VecSceneSound;
typedef std::vector<std::unique_ptr<SceneCloud>> VecSceneCloud;

class Scene
{
public:
    // destructor
    ~Scene();

    // constructor
    Scene();

    // window to choose a scene file
    static std::string askNameScene(FileDirection direction);

    bool load(QFile &sceneFile);
    bool save(QFile &sceneFile);
    void addSampleSet(AudioFile *samples[], unsigned count);
    void addNewCloud(int numVoices);

    SceneSound *selectedSound();
    SceneCloud *selectedCloud();

    // handle deselections
    void deselect(int shapeType);

    // data contents
    std::string m_audioPath;
    VecSceneSound m_sounds;
    VecSceneSound m_soundsNotFound;
    VecSceneCloud m_clouds;

    // selection helper vars
    int m_selectedCloud = -1;
    int m_selectedSound = -1;
    int m_selectionIndex = 0;
    std::vector<int> m_selectionIndices;

    // mutex
    std::mutex m_mutex;
};

struct SceneSound {
    std::string name;
    AudioFile *sample = nullptr;
    std::unique_ptr<SoundRect> view;

    ~SceneSound();
};

struct SceneCloud {
    std::unique_ptr<GrainCluster> cloud;
    std::unique_ptr<GrainClusterVis> view;

    ~SceneCloud();
};

#endif // SCENE_H
