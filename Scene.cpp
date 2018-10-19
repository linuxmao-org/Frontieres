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
#include <QTextStream>
#include <QStandardPaths>
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

    sceneFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream sceneFlux(&sceneFile);
    string lineScene = sceneFlux.readLine().toUtf8().constData(); // path
    cout << "scene path : " << lineScene << endl;
    int nSamplesScene = sceneFlux.readLine().toInt(); // number of samples scene
    cout << nSamplesScene << " samples" << endl;
    int j = 0;
    int nSample = -1;
    int nCloud = -1;
    float widthSample = 0;
    float heightSample = 0;
    float xSample = 0;
    float ySample = 0;
    bool orientationSample = true;
    for (int i = 0; i < nSamplesScene ; i++) { // samples
        nSample = -1;
        lineScene = sceneFlux.readLine().toUtf8().constData(); // sample
        cout << lineScene;
        j = 0;
        while ( j < mySounds->size()) {
            if (mySounds->at(j)->name == lineScene) { // sample matching
                nSample = j;
                j = mySounds->size();
            }
            j += 1;
        }
        if (nSample != -1) {
            orientationSample = soundViews->at(nSample)->getOrientation();
            if (!!sceneFlux.readLine().toInt() != soundViews->at(nSample)->getOrientation())
                soundViews->at(nSample)->toggleOrientation();
            heightSample = sceneFlux.readLine().toFloat();
            widthSample = sceneFlux.readLine().toFloat();
            xSample = sceneFlux.readLine().toFloat();
            ySample = sceneFlux.readLine().toFloat();
            soundViews->at(nSample)->setWidthHeight(widthSample, heightSample);
            soundViews->at(nSample)->setXY(xSample, ySample);
            cout << ", heigth : " << heightSample
                 << ", width : " << widthSample
                 << ", x : " << xSample
                 << ", y : " << ySample << endl;
        }
        else
            cout << " not found" << endl;
        lineScene = sceneFlux.readLine().toUtf8().constData(); // ---
    }
    int nCloudScene = sceneFlux.readLine().toInt(); // number of clouds scene
    cout << nCloudScene << " clouds to create" << endl;
    float cloudDuration = 0;
    float cloudOverlap = 0;
    float cloudPitch = 0;
    float cloudPitchLFOFreq = 0;
    float cloudPitchLFOAmount = 0;
    int cloudDirection = 0;
    int cloudWindowType = 0;
    int cloudSpatialMode = 0;
    int cloudSpatialChanel = 0;
    float cloudVolumeDb = 0;
    int cloudNumVoices = 0;
    int cloudActivateState = 0;
    float cloudX = 0;
    float cloudY = 0;
    float cloudXRandExtent = 0;
    float cloudYRandExtent = 0;
    int cloudCurrent = 0;
    for (int i = 0; i < nCloudScene ; i++) { // clouds
        cloudCurrent = i + 1;
        cloudDuration = sceneFlux.readLine().toFloat();
        cloudOverlap = sceneFlux.readLine().toFloat();
        cloudPitch = sceneFlux.readLine().toFloat();
        cloudPitchLFOFreq = sceneFlux.readLine().toFloat();
        cloudPitchLFOAmount = sceneFlux.readLine().toFloat();
        cloudDirection = sceneFlux.readLine().toInt();
        cloudWindowType = sceneFlux.readLine().toInt();
        cloudSpatialMode = sceneFlux.readLine().toInt();
        cloudSpatialChanel = sceneFlux.readLine().toInt();
        cloudVolumeDb = sceneFlux.readLine().toFloat();
        cloudNumVoices = sceneFlux.readLine().toInt();
        cloudActivateState = sceneFlux.readLine().toInt();
        cloudX = sceneFlux.readLine().toFloat();
        cloudY = sceneFlux.readLine().toFloat();
        cloudXRandExtent = sceneFlux.readLine().toFloat();
        cloudYRandExtent = sceneFlux.readLine().toFloat();
        lineScene = sceneFlux.readLine().toUtf8().constData(); // ---
        cout << "cloud " << cloudCurrent << " :" << endl;
        cout << "duration = " << cloudDuration << endl;
        cout << "overlap = " << cloudOverlap << endl;
        cout << "pitch = " << cloudPitch << endl;
        cout << "pitchLFOFreq = " << cloudPitchLFOFreq << endl;
        cout << "pitchLFOAmount = " << cloudPitchLFOAmount << endl;
        cout << "direction = " << cloudDirection << endl;
        cout << "window type = " << cloudWindowType << endl;
        cout << "spatial mode = " << cloudSpatialMode << endl;
        cout << "spatial chanel = " << cloudSpatialChanel << endl;
        cout << "volume = " << cloudVolumeDb << endl;
        cout << "voices = " << cloudNumVoices << endl;
        cout << "active = " << cloudActivateState << endl;
        cout << "X = " << cloudX << endl;
        cout << "Y = " << cloudY << endl;
        cout << "X extent = " << cloudXRandExtent << endl;
        cout << "Y extent = " << cloudYRandExtent << endl;
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
        gc->setActiveState(cloudActivateState);
        gv->setFixedXRandExtent(cloudXRandExtent);
        gv->setFixedYRandExtent(cloudYRandExtent);
        gv->setSelectState(false);

        numClouds += 1;
    }

    return true;
}

bool Scene::save(QFile &sceneFile)
{
    // TODO structurer ce format de fichier

    if (!sceneFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream sceneFlux(&sceneFile);
    // audio path
    cout << "record scene " << sceneFile.fileName().toStdString() << endl;
    cout << "audio path : " << g_audioPath << endl;
    sceneFlux << QString::fromStdString(g_audioPath) << endl;
    // samples
    cout << mySounds->size() << "samples : " << endl;
    sceneFlux << QString::number(mySounds->size()) << endl;
    for (int i = 0; i < mySounds->size(); ++i) {
        AudioFile *af = mySounds->at(i);
        SoundRect *sv = soundViews->at(i);

        std::ostream &out = std::cout;
        out << "Audio file " << i << ":";
        af->describe(out);
        out << "Sound rect " << i << ":";
        sv->describe(out);

        sceneFlux << QString::fromStdString(af->name) << endl;
        sceneFlux << QString::number(sv->getOrientation()) << endl;
        sceneFlux << QString::number(sv->getHeight()) << endl;
        sceneFlux << QString::number(sv->getWidth()) << endl;
        sceneFlux << QString::number(sv->getX()) << endl;
        sceneFlux << QString::number(sv->getY()) << endl;
        sceneFlux << "---" << endl;
    }
    // grainclouds
    cout << grainCloud->size() << " clouds : " << endl;
    sceneFlux << QString::number(grainCloud->size()) << endl;
    for (int i = 0; i < grainCloud->size(); i++) {
        GrainCluster *gc = grainCloud->at(i);
        GrainClusterVis *gv = grainCloudVis->at(i);

        std::ostream &out = std::cout;
        out << "Grain Cluster " << i << ":";
        gc->describe(out);
        out << "Grain Vis " << i << ":";
        gv->describe(out);

        sceneFlux << QString::number(gc->getDurationMs()) << endl;
        sceneFlux << QString::number(gc->getOverlap()) << endl;
        sceneFlux << QString::number(gc->getPitch()) << endl;
        sceneFlux << QString::number(gc->getPitchLFOFreq()) << endl;
        sceneFlux << QString::number(gc->getPitchLFOAmount()) << endl;
        sceneFlux << QString::number(gc->getDirection()) << endl;
        sceneFlux << QString::number(gc->getWindowType()) << endl;
        sceneFlux << QString::number(gc->getSpatialMode()) << endl;
        sceneFlux << QString::number(gc->getSpatialChannel()) << endl;
        sceneFlux << QString::number(gc->getVolumeDb()) << endl;
        sceneFlux << QString::number(gc->getNumVoices()) << endl;
        sceneFlux << QString::number(gc->getActiveState()) << endl;
        sceneFlux << QString::number(gv->getX()) << endl;
        sceneFlux << QString::number(gv->getY()) << endl;
        sceneFlux << QString::number(gv->getXRandExtent()) << endl;
        sceneFlux << QString::number(gv->getYRandExtent()) << endl;
        sceneFlux << "---" << endl;
    }

    return true;
}
