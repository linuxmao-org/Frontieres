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

#include "Frontieres.h"
#include <QFile>
#include <QString>
#include <iostream>
#include <QIODevice>
#include <QTextStream>
#include <iostream>
#include <QStandardPaths>
class SoundRect;
class GrainCluster;
class GrainClusterVis;
struct AudioFile;
class QtFont3D;

using namespace std;

extern string g_audioPath;
extern string g_audioPathDefault;
extern QString g_extensionScene;

class Scene
{
public:
    // destructor
    virtual ~Scene();

    // constructor
    Scene();

    // window to choose a scene file
    string askNameScene(FileDirection direction);

    bool load(QFile &sceneFile);
    bool save(QFile &sceneFile);
};

#endif // SCENE_H
