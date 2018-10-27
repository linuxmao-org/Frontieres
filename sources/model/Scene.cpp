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
#include "Cloud.h"
#include "Grain.h"
#include "AudioFileSet.h"
#include "I18n.h"
#include "visual/SoundRect.h"
#include "dsp/Window.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QFile>
#include <QDebug>

// TODO arrange this later
#include "visual/CloudVis.h"

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
    : m_audioFiles(new AudioFileSet)
{
    initDefaultCloudParams();
}
//-----------------------------------------------------------------------------
// window to chose scene
//-----------------------------------------------------------------------------
std::string Scene::askNameScene(FileDirection direction)
{
    // choise file name and test extension
    QString filterExtensionScene = _Q("", "Scene files (*%1)").arg(g_extensionScene);
    QString pathScene = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    QFileDialog dlg(nullptr, QString(), pathScene, filterExtensionScene);
    dlg.setDefaultSuffix(g_extensionScene);
    if (direction == FileDirection::Save) {
        dlg.setWindowTitle(_Q("", "Save scene"));
        dlg.setAcceptMode(QFileDialog::AcceptSave);
    }
    else {
        dlg.setWindowTitle(_Q("", "Load scene"));
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
void Scene::clear()
{
    m_audioPaths.clear();
    m_sounds.clear();
    m_clouds.clear();
    m_audioFiles.reset(new AudioFileSet);
    m_selectedCloud = -1;
    m_selectedSound = -1;
    m_selectionIndex = 0;
    m_selectionIndices.clear();
}
//-----------------------------------------------------------------------------
// window to chose cloud
//-----------------------------------------------------------------------------
std::string Scene::askNameCloud(FileDirection direction)
{
    // choise file name and test extension
    QString g_extensionCloud = "cld";
    QString filterExtensionCloud = _Q("", "Cloud files (*%1)").arg(g_extensionCloud);
    QString pathScene = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    QFileDialog dlg(nullptr, QString(), pathScene, filterExtensionCloud);
    dlg.setDefaultSuffix(g_extensionCloud);
    if (direction == FileDirection::Save) {
        dlg.setWindowTitle(_Q("", "Save cloud"));
        dlg.setAcceptMode(QFileDialog::AcceptSave);
    }
    else {
        dlg.setWindowTitle(_Q("", "Load cloud"));
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
    QString sceneFileName = sceneFile.fileName();
    QDir sceneFileDir = QFileInfo(sceneFileName).dir();

    if (!sceneFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(sceneFile.readAll(), &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    QJsonObject docRoot = doc.object();

    QJsonValue valAudioPath = docRoot["audio-path"];

    // path
    m_audioPaths.clear();
    if (valAudioPath.isArray()) {
        for (QJsonValue v : valAudioPath.toArray())
            m_audioPaths.push_back(v.toString().toStdString());
    }
    else if (valAudioPath.isString()) {
        m_audioPaths.push_back(valAudioPath.toString().toStdString());
    }
    for (std::string &path : m_audioPaths) {
        // transform paths relative to the scene file
        QString qpath = QString::fromStdString(path);
        if (QDir::isRelativePath(qpath))
            path = sceneFileDir.filePath(qpath).toStdString();
        cout << "scene path : " << path << "\n";
    }

    QJsonArray docSounds = docRoot["sounds"].toArray();
    m_sounds.clear();
    m_sounds.reserve(docSounds.size());
    for (const QJsonValue &jsonElement : docSounds) { // samples
        QJsonObject objSound = jsonElement.toObject();
        SceneSound *sound = new SceneSound;
        m_sounds.emplace_back(sound);

        std::string name = objSound["name"].toString().toStdString(); // sample
        sound->name = name;
        cout << name << "\n";

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
    }

    QJsonArray docGrains = docRoot["clouds"].toArray();
    m_clouds.clear();
    m_clouds.reserve(docGrains.size());
    for (const QJsonValue &jsonElement : docGrains) { // samples
        QJsonObject objGrain = jsonElement.toObject();
        SceneCloud *cloud = new SceneCloud;
        m_clouds.emplace_back(cloud);

        int cloudId = objGrain["id"].toInt();
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
        int cloudNumGrains = objGrain["num-grains"].toInt();
        int cloudActiveState = objGrain["active-state"].toBool();
        double cloudX = objGrain["x"].toDouble();
        double cloudY = objGrain["y"].toDouble();
        double cloudXRandExtent = objGrain["x-rand-extent"].toDouble();
        double cloudYRandExtent = objGrain["y-rand-extent"].toDouble();
        cout << "cloud " << m_clouds.size() << " :" << "\n";
        cout << "id " << cloudId << " :" << "\n";
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
        cout << "grains = " << cloudNumGrains << "\n";
        cout << "active = " << cloudActiveState << "\n";
        cout << "X = " << cloudX << "\n";
        cout << "Y = " << cloudY << "\n";
        cout << "X extent = " << cloudXRandExtent << "\n";
        cout << "Y extent = " << cloudYRandExtent << "\n";
        // create audio
        Cloud *cloudToLoad = new Cloud(&m_sounds, cloudNumGrains);
        cloud->cloud.reset(cloudToLoad);
        // create visualization
        CloudVis *cloudVisToLoad = new CloudVis(cloudX, cloudY, cloudNumGrains, &m_sounds);
        cloud->view.reset(cloudVisToLoad);
        // register visualization with audio
        cloudVisToLoad->setSelectState(true);
        cloudToLoad->registerVis(cloudVisToLoad);

        cloudToLoad->setId(cloudId);
        cloudToLoad->setDurationMs(cloudDuration);
        cloudToLoad->setOverlap(cloudOverlap);
        cloudToLoad->setPitch(cloudPitch);
        cloudToLoad->setPitchLFOFreq(cloudPitchLFOFreq);
        cloudToLoad->setPitchLFOAmount(cloudPitchLFOAmount);
        cloudToLoad->setDirection(cloudDirection);
        cloudToLoad->setWindowType(cloudWindowType);
        cloudToLoad->setSpatialMode(cloudSpatialMode,cloudSpatialChanel);
        cloudToLoad->setVolumeDb(cloudVolumeDb);
        cloudToLoad->setActiveState(cloudActiveState);
        cloudVisToLoad->setFixedXRandExtent(cloudXRandExtent);
        cloudVisToLoad->setFixedYRandExtent(cloudYRandExtent);
        cloudVisToLoad->setSelectState(false);
    }

    return true;
}

bool Scene::save(QFile &sceneFile)
{
    QString sceneFileName = sceneFile.fileName();
    QDir sceneFileDir = QFileInfo(sceneFileName).dir();

    if (!sceneFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    m_selectedCloud = -1;
    m_selectedSound = -1;
    m_selectionIndex = 0;
    m_selectionIndices.clear();

    QJsonObject docRoot;

    // audio path
    cout << "record scene " << sceneFile.fileName().toStdString() << "\n";

    QJsonArray docPaths;
    for (const std::string &path : m_audioPaths) {
        QString qpath = QString::fromStdString(path);
        // save the paths under scene directory as relative
        QString relative = sceneFileDir.relativeFilePath(qpath);
        docPaths.append(relative.startsWith("../") ? qpath : relative);
    }
    docRoot["audio-path"] = docPaths;

    // samples
    QJsonArray docSounds;
    cout << m_sounds.size() << "samples : " << "\n";
    for (int i = 0, n = m_sounds.size(); i < n; ++i) {
        SceneSound *sound = m_sounds[i].get();
        SoundRect *sv = sound->view.get();

        std::ostream &out = std::cout;
        out << "Audio file " << i << ":\n";
        out << "- name : " << sound->name << "\n";
        out << "Sound rect " << i << ":\n";
        sv->describe(out);

        QJsonObject objSound;
        objSound["name"] = QString::fromStdString(sound->name);
        objSound["orientation"] = (int)sv->getOrientation();
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
        Cloud *cloudToSave = cloud->cloud.get();
        CloudVis *cloudVisToSave = cloud->view.get();

        std::ostream &out = std::cout;
        out << "Cloud " << i << ":";
        cloudToSave->describe(out);
        out << "Cloud Vis " << i << ":";
        cloudVisToSave->describe(out);

        QJsonObject objGrain;
        objGrain["id"] = (int)cloudToSave->getId();
        objGrain["duration"] = cloudToSave->getDurationMs();
        objGrain["overlap"] = cloudToSave->getOverlap();
        objGrain["pitch"] = cloudToSave->getPitch();
        objGrain["pitch-lfo-freq"] = cloudToSave->getPitchLFOFreq();
        objGrain["pitch-lfo-amount"] = cloudToSave->getPitchLFOAmount();
        objGrain["direction"] = cloudToSave->getDirection();
        objGrain["window-type"] = cloudToSave->getWindowType();
        objGrain["spatial-mode"] = cloudToSave->getSpatialMode();
        objGrain["spatial-channel"] = cloudToSave->getSpatialChannel();
        objGrain["volume"] = cloudToSave->getVolumeDb();
        objGrain["num-grains"] = (int)cloudToSave->getNumGrains();
        objGrain["active-state"] = cloudToSave->getActiveState();
        objGrain["x"] = cloudVisToSave->getX();
        objGrain["y"] = cloudVisToSave->getY();
        objGrain["x-rand-extent"] = cloudVisToSave->getXRandExtent();
        objGrain["y-rand-extent"] = cloudVisToSave->getYRandExtent();
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

bool Scene::loadCloudDefault(QFile &cloudFile)
{
    QString cloudFileName = cloudFile.fileName();
    QDir cloudFileDir = QFileInfo(cloudFileName).dir();

    if (!cloudFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(cloudFile.readAll(), &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    QJsonObject docRoot = doc.object();

    QJsonObject objGrain = docRoot["cloud"].toObject();
    g_defaultCloudParams.duration = objGrain["duration"].toDouble();
    g_defaultCloudParams.overlap = objGrain["overlap"].toDouble();
    g_defaultCloudParams.pitch = objGrain["pitch"].toDouble();
    g_defaultCloudParams.pitchLFOFreq = objGrain["pitch-lfo-freq"].toDouble();
    g_defaultCloudParams.pitchLFOAmount = objGrain["pitch-lfo-amount"].toDouble();
    g_defaultCloudParams.dirMode = objGrain["direction"].toInt();
    g_defaultCloudParams.windowType = objGrain["window-type"].toInt();
    g_defaultCloudParams.spatialMode = objGrain["spatial-mode"].toInt();
    g_defaultCloudParams.chanelLocation = objGrain["spatial-channel"].toInt();
    g_defaultCloudParams.volumeDB = objGrain["volume"].toDouble();
    g_defaultCloudParams.numGrains = objGrain["num-grains"].toInt();
    g_defaultCloudParams.activateState = objGrain["active-state"].toBool();
    g_defaultCloudParams.xRandExtent = objGrain["x-rand-extent"].toDouble();
    g_defaultCloudParams.yRandExtent = objGrain["y-rand-extent"].toDouble();

    cout << "duration = " << g_defaultCloudParams.duration << "\n";
    cout << "overlap = " << g_defaultCloudParams.overlap << "\n";
    cout << "pitch = " << g_defaultCloudParams.pitch << "\n";
    cout << "pitchLFOFreq = " << g_defaultCloudParams.pitchLFOFreq << "\n";
    cout << "pitchLFOAmount = " << g_defaultCloudParams.pitchLFOAmount << "\n";
    cout << "direction = " << g_defaultCloudParams.dirMode << "\n";
    cout << "window type = " << g_defaultCloudParams.windowType << "\n";
    cout << "spatial mode = " << g_defaultCloudParams.spatialMode << "\n";
    cout << "spatial channel = " << g_defaultCloudParams.chanelLocation << "\n";
    cout << "volume = " << g_defaultCloudParams.volumeDB << "\n";
    cout << "grains = " << g_defaultCloudParams.numGrains << "\n";
    cout << "active = " << g_defaultCloudParams.activateState << "\n";
    cout << "xRandExtent = " << g_defaultCloudParams.xRandExtent << "\n";
    cout << "yRandExtent = " << g_defaultCloudParams.yRandExtent << "\n";

    return true;

}

bool Scene::saveCloud(QFile &cloudFile, SceneCloud *selectedCloudSave)
{
    QString cloudFileName = cloudFile.fileName();
    QDir cloudFileDir = QFileInfo(cloudFileName).dir();

    if (!cloudFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QJsonObject docRoot;

    // audio path
    cout << "record cloud " << cloudFile.fileName().toStdString() << "\n";

    QJsonArray docPaths;

    // graincloud

    Cloud *cloudToSave = selectedCloudSave->cloud.get();
    CloudVis *cloudVisToSave = selectedCloudSave->view.get();

    std::ostream &out = std::cout;
    out << "Grain Cloud " << cloudToSave->getId() << "\n";
    cloudToSave->describe(out);

    QJsonObject objGrain;
    objGrain["duration"] = cloudToSave->getDurationMs();
    objGrain["overlap"] = cloudToSave->getOverlap();
    objGrain["pitch"] = cloudToSave->getPitch();
    objGrain["pitch-lfo-freq"] = cloudToSave->getPitchLFOFreq();
    objGrain["pitch-lfo-amount"] = cloudToSave->getPitchLFOAmount();
    objGrain["direction"] = cloudToSave->getDirection();
    objGrain["window-type"] = cloudToSave->getWindowType();
    objGrain["spatial-mode"] = cloudToSave->getSpatialMode();
    objGrain["spatial-channel"] = cloudToSave->getSpatialChannel();
    objGrain["volume"] = cloudToSave->getVolumeDb();
    objGrain["num-grains"] = (int)cloudToSave->getNumGrains();
    objGrain["active-state"] = cloudToSave->getActiveState();
    objGrain["x-rand-extent"] = cloudVisToSave->getXRandExtent();
    objGrain["y-rand-extent"] = cloudVisToSave->getYRandExtent();

    docRoot["cloud"] = objGrain;

    QJsonDocument document;
    document.setObject(docRoot);
    cloudFile.write(document.toJson());
    if (!cloudFile.flush())
        return false;

    return true;
}

bool Scene::loadSampleSet(bool interactive)
{
    // a new sound collection
    std::unique_ptr<AudioFileSet> audioFiles(new AudioFileSet);
    // total count of sounds needed
    unsigned numSounds = m_sounds.size();
    // count of sounds left to load
    unsigned numSoundsLeft = numSounds;
    // list of sounds associated with scene
    std::vector<AudioFile *> soundFileAssoc(numSounds);

    // the scene's current audio paths
    std::vector<std::string> audioPaths = m_audioPaths;
    // a list of additional candidate paths, added interactively by user
    std::vector<std::string> candidateAudioPaths;
    // a temporary list of search paths to try in order
    std::vector<const std::string *> effectiveAudioPaths;

    while (numSoundsLeft > 0) {
        // attempt to load many missing sounds as possible
        for (size_t i = 0; i < numSounds; ++i) {
            if (soundFileAssoc[i])
                continue;  // already loaded

            // construct the current search path
            effectiveAudioPaths.clear();
            for (const std::string &path : candidateAudioPaths) {
                // has to be the candidates at the front (logic follows below)
                effectiveAudioPaths.push_back(&path);
            }
            for (const std::string &path : audioPaths)
                effectiveAudioPaths.push_back(&path);
            effectiveAudioPaths.push_back(&g_audioPathUser);
            effectiveAudioPaths.push_back(&g_audioPathSystem);

            // locate this file and load
            AudioFile *af = nullptr;
            QString qname = QString::fromStdString(m_sounds[i]->name);
            for (unsigned i = 0, n = effectiveAudioPaths.size(); !af && i < n; ++i) {
                QDir dir(QString::fromStdString(*effectiveAudioPaths[i]));
                QString curpath = dir.filePath(qname);
                af = audioFiles->loadFile(curpath.toStdString());

                // successful load from a candidate path?
                if (af && i < candidateAudioPaths.size()) {
                    // move the candidate path into the recognized audio path (front)
                    audioPaths.insert(audioPaths.begin(), *effectiveAudioPaths[i]);
                    candidateAudioPaths.erase(candidateAudioPaths.begin() + i);
                }
            }

            if (af) {
                soundFileAssoc[i] = af;
                --numSoundsLeft;
            }
        }

        if (numSoundsLeft > 0) {  // failure to load some sounds
            if (!interactive)
                return false;

            // ask the user to fix
            unsigned indexMissing = 0;
            while (soundFileAssoc[indexMissing]) ++indexMissing;

            QString qnameMissing = QString::fromStdString(m_sounds[indexMissing]->name);

            QMessageBox mbox(
                QMessageBox::Warning, _Q("", "Load sounds"),
                _Q("", "Could not find the sound file \"%1\".").arg(qnameMissing));

            mbox.addButton(_Q("", "Add a different sound directory"), QMessageBox::AcceptRole);
            mbox.addButton(_Q("", "Discard missing sounds"), QMessageBox::DestructiveRole);
            mbox.addButton(QMessageBox::Cancel);
            mbox.exec();

            QAbstractButton *clicked = mbox.clickedButton();
            QMessageBox::ButtonRole role = clicked ? mbox.buttonRole(clicked) : QMessageBox::RejectRole;

            if (role == QMessageBox::DestructiveRole) {
                // ignore missing sounds and proceed
                break;
            }
            else if (role == QMessageBox::AcceptRole) {
                // ask for a directory to add to search paths
                QString candidateDir = QFileDialog::getExistingDirectory(
                    nullptr, _Q("", "Add sound directory"),
                    QString(), QFileDialog::ShowDirsOnly);
                // add into candidates
                if (!candidateDir.isEmpty())
                    candidateAudioPaths.insert(candidateAudioPaths.begin(), candidateDir.toStdString());
            }
            else {
                // cancel
                return false;
            }
        }
    }

    // load all sounds into model
    m_audioFiles = std::move(audioFiles);
    for (unsigned i = 0, j = 0, n = soundFileAssoc.size(); i < n; ++i) {
        AudioFile *assoc = soundFileAssoc[i];
        if (!assoc) {
            // could not load sound for element? remove
            m_sounds.erase(m_sounds.begin() + j);
        }
        else {
            // associate sound with its element
            SceneSound &sceneSound = *m_sounds[j++];
            sceneSound.sample = assoc;
            sceneSound.view->associateSound(assoc->wave, assoc->frames, assoc->channels, assoc->name);
        }
    }

    for (unsigned i = 0, n = m_clouds.size(); i < n; ++i) {
        Cloud &cloudToUpdate = *m_clouds[i]->cloud;
        cloudToUpdate.updateSoundSet();
    }

    // update the audio path, adding user's candidates which worked
    m_audioPaths = audioPaths;
    return true;
}
// init default cloud params
void Scene::initDefaultCloudParams()
{
    cout << "entree init default" << endl;
    g_defaultCloudParams.duration = 500.0;
    g_defaultCloudParams.overlap = 1.0f;
    g_defaultCloudParams.pitch = 1.0f;
    g_defaultCloudParams.pitchLFOFreq = 0.01f;
    g_defaultCloudParams.pitchLFOAmount = 0.00f;
    g_defaultCloudParams.windowType = HANNING;
    g_defaultCloudParams.spatialMode= UNITY;
    g_defaultCloudParams.chanelLocation = -1;
    g_defaultCloudParams.numGrains = 8;
    g_defaultCloudParams.activateState = true;
    g_defaultCloudParams.xRandExtent = 3.0f;
    g_defaultCloudParams.yRandExtent = 3.0f;
}

AudioFile *Scene::loadNewSample(const std::string &path)
{
    AudioFile *af = m_audioFiles->loadFile(path);
    if (!af) {
        // cannot load
        return nullptr;
    }
    return af;
}

bool Scene::removeSoundAt(unsigned index)
{
    if (index >= m_sounds.size())
        return false;

    AudioFile *af = m_sounds[index]->sample;
    m_sounds.erase(m_sounds.begin() + index);

    for (unsigned i = 0, n = m_clouds.size(); i < n; ++i) {
        Cloud &cloudToUpdate = *m_clouds[i]->cloud;
        cloudToUpdate.updateSoundSet();
    }

    removeSampleIfNotUsed(af);
    return true;
}

void Scene::removeSampleIfNotUsed(AudioFile *sample)
{
    bool used = false;
    for (unsigned i = 0, n = m_sounds.size(); !used && i < n; ++i)
        used = m_sounds[i]->sample == sample;
    if (!used) {
        std::cout << "Sample is not used, delete\n";
        m_audioFiles->removeSample(sample);
    }
    else {
        std::cout << "Sample is used, keep\n";
    }
}

void Scene::addAudioPath(const std::string &path)
{
    QDir dir(QString::fromStdString(path));
    dir.makeAbsolute();

    for (const std::string &otherPath : m_audioPaths) {
        // check if already present
        if (dir == QDir(QString::fromStdString(otherPath)))
            return;
    }

    m_audioPaths.push_back(path);
    return;
}

void Scene::addSoundRect(AudioFile *sample)
{
    SceneSound *sound = new SceneSound;
    m_sounds.emplace_back(sound);
    sound->name = sample->name;
    sound->sample = sample;

    SoundRect *sv = new SoundRect;
    sound->view.reset(sv);
    sv->associateSound(sample->wave, sample->frames, sample->channels, sample->name);

    for (unsigned i = 0, n = m_clouds.size(); i < n; ++i) {
        Cloud &cloudToUpdate = *m_clouds[i]->cloud;
        cloudToUpdate.updateSoundSet();
    }
}

void Scene::addNewCloud(int numGrains)
{
    // create audio
    SceneCloud *cloud = new SceneCloud;
    m_clouds.emplace_back(cloud);
    cloud->cloud.reset(new Cloud(&m_sounds, numGrains));
    // create visualization
    cloud->view.reset(new CloudVis(mouseX, mouseY, numGrains, &m_sounds));
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
// get num of a cloud in current scene
int Scene::getNumCloud(SceneCloud *cloudCurrent)
{
    for (int i = 0, n = m_clouds.size(); i < n; i++) {
        Cloud *cloudForNum = cloudCurrent->cloud.get();
        Cloud *cloudToExplore = m_clouds[i]->cloud.get();
        if (cloudForNum->getId() == cloudToExplore->getId())
            return i;
    }
    return -1;
}


// destructor
SceneSound::~SceneSound()
{
}

// destructor
SceneCloud::~SceneCloud()
{
}
