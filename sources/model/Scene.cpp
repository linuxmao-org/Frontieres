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

#include "Scene.h"
#include "Frontieres.h"
#include "GrainCluster.h"
#include "GrainVoice.h"
#include "AudioFileSet.h"
#include "I18n.h"
#include "visual/SoundRect.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QFile>
#include <QDebug>

// TODO arrange this later
#include "visual/GrainClusterVis.h"

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Scene::~Scene()
{
}
//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Scene::Scene()
{
}
//-----------------------------------------------------------------------------
// window to chose scene
//-----------------------------------------------------------------------------
std::string Scene::askNameScene(FileDirection direction)
{
    // choise file name and test extension
    QString caption = _Q("","Frontieres : name of the scene");
    QString filterExtensionScene = _Q("", "Scene files (*%1)").arg(g_extensionScene);
    QString pathScene = QString::fromStdString(g_audioPath);

    if (g_audioPath == g_audioPathDefault)
        pathScene = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    QFileDialog dlg(nullptr, caption, pathScene, filterExtensionScene);
    dlg.setDefaultSuffix(g_extensionScene);
    if (direction == FileDirection::Save) {
        dlg.setAcceptMode(QFileDialog::AcceptSave);
    }
    else {
        dlg.setAcceptMode(QFileDialog::AcceptOpen);
        dlg.setFileMode(QFileDialog::ExistingFile);
    }

    if (dlg.exec() != QDialog::Accepted)
        return std::string();

    QStringList selection = dlg.selectedFiles();
    if (selection.size() != 1)
        return std::string();

    return selection.front().toStdString();
}

bool Scene::load(QFile &sceneFile)
{
    // TODO structurer ce format de fichier
    // TODO ne pas charger la scène en global

    if (!sceneFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(sceneFile.readAll(), &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    QJsonObject docRoot = doc.object();

    m_audioPath = docRoot["audio-path"].toString().toStdString(); // path
    cout << "scene path : " << m_audioPath << "\n";

    QJsonArray docSounds = docRoot["sounds"].toArray();
    m_sounds.clear();
    m_sounds.reserve(docSounds.size());
    m_soundsNotFound.clear();
    m_soundsNotFound.reserve(docSounds.size());
    for (const QJsonValue &jsonElement : docSounds) { // samples
        QJsonObject objSound = jsonElement.toObject();
        SceneSound *sound = new SceneSound;
        m_sounds.emplace_back(sound);

        std::string name = objSound["name"].toString().toStdString(); // sample
        cout << name << "\n";

        AudioFile *af = nullptr;
        for (int j = 0, n = mySounds->size(); !af && j < n; ++j) {
            AudioFile *cur = (*mySounds)[j];
            if (name == cur->name) // sample matching
                af = cur;
        }
        sound->sample = af;

        SoundRect *sv = new SoundRect();
        sound->view.reset(sv);
        if ((bool)objSound["orientation"].toInt() != sv->getOrientation())
            sv->toggleOrientation();
        double heightSample = objSound["height"].toDouble();
        double widthSample = objSound["width"].toDouble();
        double xSample = objSound["x"].toDouble();
        double ySample = objSound["y"].toDouble();
        sv->setWidthHeight(widthSample, heightSample);
        sv->setXY(xSample, ySample);
        cout << ", heigth : " << heightSample
             << ", width : " << widthSample
             << ", x : " << xSample
             << ", y : " << ySample << "\n";

        if (af) {
            sv->associateSound(af->wave, af->frames, af->channels);
        }
        else {
            cout << " not found" << "\n";
            m_soundsNotFound.push_back(std::move(m_sounds.back()));
            m_sounds.pop_back();
        }
    }

    QJsonArray docGrains = docRoot["clouds"].toArray();
    m_clouds.clear();
    m_clouds.reserve(docGrains.size());
    for (const QJsonValue &jsonElement : docGrains) { // samples
        QJsonObject objGrain = jsonElement.toObject();
        SceneCloud *cloud = new SceneCloud;
        m_clouds.emplace_back(cloud);

        double cloudDuration = objGrain["duration"].toDouble();
        double cloudOverlap = objGrain["overlap"].toDouble();
        double cloudPitch = objGrain["pitch"].toDouble();
        double cloudPitchLFOFreq = objGrain["pitch-lfo-freq"].toDouble();
        double cloudPitchLFOAmount = objGrain["pitch-lfo-amount"].toDouble();
        int cloudDirection = objGrain["direction"].toInt();
        int cloudWindowType = objGrain["window-type"].toInt();
        int cloudSpatialMode = objGrain["spatial-mode"].toInt();
        int cloudSpatialChanel = objGrain["spatial-channel"].toInt();
        double cloudVolumeDb = objGrain["volume"].toDouble();
        int cloudNumVoices = objGrain["num-voices"].toInt();
        int cloudActiveState = objGrain["active-state"].toBool();
        double cloudX = objGrain["x"].toDouble();
        double cloudY = objGrain["y"].toDouble();
        double cloudXRandExtent = objGrain["x-rand-extent"].toDouble();
        double cloudYRandExtent = objGrain["y-rand-extent"].toDouble();
        cout << "cloud " << m_clouds.size() << " :" << "\n";
        cout << "duration = " << cloudDuration << "\n";
        cout << "overlap = " << cloudOverlap << "\n";
        cout << "pitch = " << cloudPitch << "\n";
        cout << "pitchLFOFreq = " << cloudPitchLFOFreq << "\n";
        cout << "pitchLFOAmount = " << cloudPitchLFOAmount << "\n";
        cout << "direction = " << cloudDirection << "\n";
        cout << "window type = " << cloudWindowType << "\n";
        cout << "spatial mode = " << cloudSpatialMode << "\n";
        cout << "spatial channel = " << cloudSpatialChanel << "\n";
        cout << "volume = " << cloudVolumeDb << "\n";
        cout << "voices = " << cloudNumVoices << "\n";
        cout << "active = " << cloudActiveState << "\n";
        cout << "X = " << cloudX << "\n";
        cout << "Y = " << cloudY << "\n";
        cout << "X extent = " << cloudXRandExtent << "\n";
        cout << "Y extent = " << cloudYRandExtent << "\n";
        // create audio
        GrainCluster *gc = new GrainCluster(&m_sounds, cloudNumVoices);
        cloud->cloud.reset(gc);
        // create visualization
        GrainClusterVis *gv = new GrainClusterVis(cloudX, cloudY, cloudNumVoices, &m_sounds);
        cloud->view.reset(gv);
        // register visualization with audio
        gv->setSelectState(true);
        gc->registerVis(gv);

        gc->setDurationMs(cloudDuration);
        gc->setOverlap(cloudOverlap);
        gc->setPitch(cloudPitch);
        gc->setPitchLFOFreq(cloudPitchLFOFreq);
        gc->setPitchLFOAmount(cloudPitchLFOAmount);
        gc->setDirection(cloudDirection);
        gc->setWindowType(cloudWindowType);
        gc->setSpatialMode(cloudSpatialMode,cloudSpatialChanel);
        gc->setVolumeDb(cloudVolumeDb);
        gc->setActiveState(cloudActiveState);
        gv->setFixedXRandExtent(cloudXRandExtent);
        gv->setFixedYRandExtent(cloudYRandExtent);
        gv->setSelectState(false);
    }

    return true;
}

bool Scene::save(QFile &sceneFile)
{
    // TODO structurer ce format de fichier
    // TODO ne pas sauvegarder la scène globale

    if (!sceneFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    m_selectedCloud = -1;
    m_selectedSound = -1;
    m_selectionIndex = 0;
    m_selectionIndices.clear();
    m_audioPath = g_audioPath;

    QJsonObject docRoot;

    // audio path
    cout << "record scene " << sceneFile.fileName().toStdString() << "\n";
    cout << "audio path : " << m_audioPath << "\n";
    docRoot["audio-path"] = QString::fromStdString(m_audioPath);

    // collect a list of sounds (both found and not found)
    std::vector<SceneSound *> allSounds;
    allSounds.reserve(m_sounds.size() + m_soundsNotFound.size());
    for (const auto &sound : m_sounds)
        allSounds.push_back(sound.get());
    for (const auto &sound : m_soundsNotFound)
        allSounds.push_back(sound.get());

    // samples
    QJsonArray docSounds;
    cout << allSounds.size() << "samples : " << "\n";
    for (int i = 0, n = allSounds.size(); i < n; ++i) {
        SceneSound *sound = allSounds[i];
        SoundRect *sv = sound->view.get();

        std::ostream &out = std::cout;
        out << "Audio file " << i << ":\n";
        out << "- name : " << sound->name << "\n";
        out << "Sound rect " << i << ":\n";
        sv->describe(out);

        QJsonObject objSound;
        objSound["name"] = QString::fromStdString(sound->name);
        objSound["orientation"] = QString::number(sv->getOrientation());
        objSound["height"] = sv->getHeight();
        objSound["width"] = sv->getWidth();
        objSound["x"] = sv->getX();
        objSound["y"] = sv->getY();
        docSounds.append(objSound);
    }

    // grainclouds
    QJsonArray docGrains;
    for (int i = 0, n = m_clouds.size(); i < n; i++) {
        SceneCloud *cloud = m_clouds[i].get();
        GrainCluster *gc = cloud->cloud.get();
        GrainClusterVis *gv = cloud->view.get();

        std::ostream &out = std::cout;
        out << "Grain Cluster " << i << ":";
        gc->describe(out);
        out << "Grain Vis " << i << ":";
        gv->describe(out);

        QJsonObject objGrain;
        objGrain["duration"] = gc->getDurationMs();
        objGrain["overlap"] = gc->getOverlap();
        objGrain["pitch"] = gc->getPitch();
        objGrain["pitch-lfo-freq"] = gc->getPitchLFOFreq();
        objGrain["pitch-lfo-amount"] = gc->getPitchLFOAmount();
        objGrain["direction"] = gc->getDirection();
        objGrain["window-type"] = gc->getWindowType();
        objGrain["spatial-mode"] = gc->getSpatialMode();
        objGrain["spatial-channel"] = gc->getSpatialChannel();
        objGrain["volume"] = gc->getVolumeDb();
        objGrain["num-voices"] = (int)gc->getNumVoices();
        objGrain["active-state"] = gc->getActiveState();
        objGrain["x"] = gv->getX();
        objGrain["y"] = gv->getY();
        objGrain["x-rand-extent"] = gv->getXRandExtent();
        objGrain["y-rand-extent"] = gv->getYRandExtent();
        docGrains.append(objGrain);
    }

    docRoot["sounds"] = docSounds;
    docRoot["clouds"] = docGrains;

    QJsonDocument document;
    document.setObject(docRoot);
    sceneFile.write(document.toJson());
    if (!sceneFile.flush())
        return false;

    return true;
}

void Scene::addSampleSet(AudioFile *samples[], unsigned count)
{
    for (unsigned i = 0; i < count; ++i) {
        SceneSound *sound = new SceneSound;
        m_sounds.emplace_back(sound);

        AudioFile *af = samples[i];
        sound->name = af->name;
        sound->sample = af;
        SoundRect *sv = new SoundRect;
        sound->view.reset(sv);
        sv->associateSound(af->wave, af->frames, af->channels);
    }
}

void Scene::addNewCloud(int numVoices)
{
    // create audio
    SceneCloud *cloud = new SceneCloud;
    m_clouds.emplace_back(cloud);
    cloud->cloud.reset(new GrainCluster(&m_sounds, numVoices));
    // create visualization
    cloud->view.reset(new GrainClusterVis(mouseX, mouseY, numVoices, &m_sounds));
    // select new cloud
    cloud->view->setSelectState(true);
    // register visualization with audio
    cloud->cloud->registerVis(cloud->view.get());
}

SceneSound *Scene::selectedSound()
{
    if ((unsigned)m_selectedSound >= m_sounds.size())
        return nullptr;
    return m_sounds[(unsigned)m_selectedSound].get();
}

SceneCloud *Scene::selectedCloud()
{
    if ((unsigned)m_selectedCloud >= m_clouds.size())
        return nullptr;
    return m_clouds[(unsigned)m_selectedCloud].get();
}

// handle deselections
void Scene::deselect(int shapeType)
{
    switch (shapeType) {
    case CLOUD:
        if (SceneCloud *cloud = selectedCloud()) {
            cloud->view->setSelectState(false);
            // reset selected cloud
            m_selectedCloud = -1;
            // cout << "deselecting cloud" <<endl;
        }
        break;
    case RECT:
        if (SceneSound *sound = selectedSound()) {
            // cout << "deselecting rect" << endl;
            sound->view->setSelectState(false);
            m_selectedSound = -1;
        }
        break;
    }
}

// destructor
SceneSound::~SceneSound()
{
}

// destructor
SceneCloud::~SceneCloud()
{
}
