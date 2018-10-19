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

    string askNameScene(int direction);
};

#endif // SCENE_H
