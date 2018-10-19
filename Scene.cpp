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
#include "SoundRect.h"
#include "GrainCluster.h"
#include "GrainVoice.h"
#include "AudioFileSet.h"
#include "I18n.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

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
string Scene::askNameScene(FileDirection direction)
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

    QString lineScene = doc["audio-path"].toString(); // path
    cout << "scene path : " << lineScene.toStdString() << "\n";
    for (const QJsonValue &jsonElement : doc["sounds"].toArray()) { // samples
        QJsonObject objSound = jsonElement.toObject();
        int nSample = -1;
        lineScene = objSound["name"].toString(); // sample
        cout << lineScene.toStdString();
        for (int j = 0; j < mySounds->size(); ++j) {
            if (mySounds->at(j)->name.c_str() == lineScene) { // sample matching
                nSample = j;
                j = mySounds->size();
            }
        }

        if (nSample != -1) {
            SoundRect *sv = soundViews->at(nSample);
            if ((bool)objSound["orientation"].toInt() != sv->getOrientation())
                soundViews->at(nSample)->toggleOrientation();
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
        else
            cout << " not found" << "\n";
    }

    int cloudCurrent = 0;
    for (const QJsonValue &jsonElement : doc["clouds"].toArray()) { // samples
        QJsonObject objGrain = jsonElement.toObject();
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
        cout << "cloud " << (cloudCurrent + 1) << " :" << "\n";
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
        GrainCluster *gc = new GrainCluster(mySounds, cloudNumVoices);
        grainCloud->push_back(gc);
        // create visualization
        GrainClusterVis *gv = new GrainClusterVis(cloudX, cloudY, cloudNumVoices, soundViews);
        grainCloudVis->push_back(gv);
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

        ++cloudCurrent;
    }

    return true;
}

bool Scene::save(QFile &sceneFile)
{
    // TODO structurer ce format de fichier
    // TODO ne pas sauvegarder la scène globale

    if (!sceneFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QJsonObject docRoot;

    // audio path
    cout << "record scene " << sceneFile.fileName().toStdString() << "\n";
    cout << "audio path : " << g_audioPath << "\n";
    docRoot["audio-path"] = QString::fromStdString(g_audioPath);

    // samples
    QJsonArray docSounds;
    cout << mySounds->size() << "samples : " << "\n";
    for (int i = 0; i < mySounds->size(); ++i) {
        AudioFile *af = mySounds->at(i);
        SoundRect *sv = soundViews->at(i);

        std::ostream &out = std::cout;
        out << "Audio file " << i << ":";
        af->describe(out);
        out << "Sound rect " << i << ":";
        sv->describe(out);

        QJsonObject objSound;
        objSound["name"] = QString::fromStdString(af->name);
        objSound["orientation"] = QString::number(sv->getOrientation());
        objSound["height"] = sv->getHeight();
        objSound["width"] = sv->getWidth();
        objSound["x"] = sv->getX();
        objSound["y"] = sv->getY();
        docSounds.append(objSound);
    }

    // grainclouds
    QJsonArray docGrains;
    cout << grainCloud->size() << " clouds : " << "\n";
    for (int i = 0; i < grainCloud->size(); i++) {
        GrainCluster *gc = grainCloud->at(i);
        GrainClusterVis *gv = grainCloudVis->at(i);

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
