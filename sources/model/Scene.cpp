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
#include "Trigger.h"
#include "Grain.h"
#include "Sample.h"
#include "ParamCloud.h"
#include "visual/SampleVis.h"
#include "visual/Trajectory.h"
#include "visual/Circular.h"
#include "visual/Hypotrochoid.h"
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
#include "visual/TriggerVis.h"
#include "Version.h"

extern CloudParams g_defaultCloudParams;
extern int version_major, version_minor, version_patch;

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
    : m_sampleSet(new SampleSet)
{
    initDefaultCloudParams();
}
//-----------------------------------------------------------------------------
// window to chose scene
//-----------------------------------------------------------------------------
std::string Scene::askNameScene(FileDirection direction)
{
    // choise file name and test extension
    QString filterExtensionScene = QObject::tr("Scene files (*%1)").arg(g_extensionScene);
    QString pathScene = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    QFileDialog dlg(nullptr, QString(), pathScene, filterExtensionScene);
    dlg.setDefaultSuffix(g_extensionScene);
    if (direction == FileDirection::Save) {
        dlg.setWindowTitle(QObject::tr("Save scene"));
        dlg.setAcceptMode(QFileDialog::AcceptSave);
    }
    else {
        dlg.setWindowTitle(QObject::tr("Load scene"));
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
    m_samples.clear();
    m_clouds.clear();
    m_sampleSet.reset(new SampleSet);
    m_selectedCloud = -1;
    m_selectedSample = -1;
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
    QString filterExtensionCloud = QObject::tr("Cloud files (*%1)").arg(g_extensionCloud);
    QString pathScene = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    QFileDialog dlg(nullptr, QString(), pathScene, filterExtensionCloud);
    dlg.setDefaultSuffix(g_extensionCloud);
    if (direction == FileDirection::Save) {
        dlg.setWindowTitle(QObject::tr("Save cloud"));
        dlg.setAcceptMode(QFileDialog::AcceptSave);
    }
    else {
        dlg.setWindowTitle(QObject::tr("Load cloud"));
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

string Scene::askNameTrajectory(FileDirection direction)
{
    // choise file name and test extension
    QString g_extensionTrajectory = "trj";
    QString filterExtensionTrajectory = QObject::tr("Trajectory files (*%1)").arg(g_extensionTrajectory);
    QString pathScene = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    QFileDialog dlg(nullptr, QString(), pathScene, filterExtensionTrajectory);
    dlg.setDefaultSuffix(g_extensionTrajectory);
    if (direction == FileDirection::Save) {
        dlg.setWindowTitle(QObject::tr("Save trajectory"));
        dlg.setAcceptMode(QFileDialog::AcceptSave);
    }
    else {
        dlg.setWindowTitle(QObject::tr("Load trajectory"));
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

    QJsonArray docVersion = docRoot["version"].toArray();
    for (const QJsonValue &jsonElement : docVersion) {
        QJsonObject objVersion = jsonElement.toObject();
        int l_version_major = objVersion["major"].toInt();
        int l_version_minor = objVersion["minor"].toInt();
        int l_version_patch = objVersion["patch"].toInt();
        cout << "scene version " << l_version_major << "." << l_version_minor << "." << l_version_patch << endl;
    }

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

    QJsonArray docSamples = docRoot["samples"].toArray();
    m_samples.clear();
    m_samples.reserve(docSamples.size());
    for (const QJsonValue &jsonElement : docSamples) { // samples
        QJsonObject objSample = jsonElement.toObject();
        SceneSample *sceneSample = new SceneSample;
        m_samples.emplace_back(sceneSample);

        std::string name = objSample["name"].toString().toStdString(); // sample
        sceneSample->name = name;
        cout << name << "\n";

        SampleVis *sv = new SampleVis();
        sceneSample->view.reset(sv);
        if ((bool)objSample["orientation"].toInt() != sv->getOrientation())
            sv->toggleOrientation();
        double heightSample = objSample["height"].toDouble();
        double widthSample = objSample["width"].toDouble();
        double xSample = objSample["x"].toDouble();
        double ySample = objSample["y"].toDouble();
        sv->setWidthHeight(widthSample, heightSample);
        sv->setXY(xSample, ySample);
        cout << ", heigth : " << heightSample
             << ", width : " << widthSample
             << ", x : " << xSample
             << ", y : " << ySample << "\n";
    }

    // clouds

    QJsonArray docClouds = docRoot["clouds"].toArray();
    m_clouds.clear();
    m_clouds.reserve(docClouds.size());
    for (const QJsonValue &jsonElement : docClouds) { // samples
        QJsonObject objCloud = jsonElement.toObject();
        SceneCloud *sceneCloud = new SceneCloud;
        m_clouds.emplace_back(sceneCloud);

        int cloudId = objCloud["id"].toInt();
        QString cloudName = objCloud["name"].toString();
        double cloudDuration = objCloud["duration"].toDouble();
        double cloudOverlap = objCloud["overlap"].toDouble();
        double cloudPitch = objCloud["pitch"].toDouble();
        double cloudPitchLFOFreq = objCloud["pitch-lfo-freq"].toDouble();
        double cloudPitchLFOAmount = objCloud["pitch-lfo-amount"].toDouble();
        int cloudDirection = objCloud["direction"].toInt();
        int cloudWindowType = objCloud["window-type"].toInt();
        int cloudSpatialMode = objCloud["spatial-mode"].toInt();
        int cloudOutputFirst = objCloud["output-first"].toInt();
        int cloudOutputLast = objCloud["output-last"].toInt();
        int cloudSpatialChannel = objCloud["spatial-channel"].toInt();
        int cloudMidiChannel = objCloud["midi-channel"].toInt();
        int cloudMidiNote = objCloud["midi-note"].toInt();
        double cloudVolumeDb = objCloud["volume"].toDouble();
        ParamEnv cloudEnvelopeVolume;
        {
            double l1 = objCloud["volume-envelope-L1"].toDouble();
            double l2 = objCloud["volume-envelope-L2"].toDouble();
            double l3 = objCloud["volume-envelope-L3"].toDouble();
            double tAtk = objCloud["volume-envelope-TAtk"].toDouble();
            double tSta = objCloud["volume-envelope-TSta"].toDouble();
            double tDec = objCloud["volume-envelope-TDec"].toDouble();
            double tRel = objCloud["volume-envelope-TRel"].toDouble();
            cloudEnvelopeVolume.setTimeBasedParameters(
                l1, l2, l3,
                tAtk, tSta, tDec, tRel,
                samp_rate);
        }
        cloudEnvelopeVolume.t1 = objCloud["volume-envelope-T1"].toInt();
        cloudEnvelopeVolume.t2 = objCloud["volume-envelope-T2"].toInt();
        cloudEnvelopeVolume.t3 = objCloud["volume-envelope-T3"].toInt();
        cloudEnvelopeVolume.t4 = objCloud["volume-envelope-T4"].toInt();
        int cloudNumGrains = objCloud["num-grains"].toInt();
        int cloudActiveState = objCloud["active-state"].toBool();
        int cloudActiveRestartTrajectory = objCloud["active-restart-trajectory"].toBool();
        double cloudX = objCloud["x"].toDouble();
        double cloudY = objCloud["y"].toDouble();
        double cloudXRandExtent = objCloud["x-rand-extent"].toDouble();
        double cloudYRandExtent = objCloud["y-rand-extent"].toDouble();

        //common trajectory parameters
        bool hasTrajectory=objCloud["has-trajectory"].toBool();
        int type=0;
        double xTrajectoryOrigin=0;
        double yTrajectoryOrigin=0;
        double phase=0;
        bool move=false;
        double speed=0;

        //specific trajectory parameters

        double centerX=0;
        double centerY=0;
        double radius=0;
        double angle=0;
        double stretch=0;
        double radiusInt=0;
        double expansion=0;
        double progress=0;
        int nbPositions=0;

        if(hasTrajectory)
        {
            type=objCloud["trajectory-type"].toInt();
            xTrajectoryOrigin=objCloud["xTrajectoryOrigin"].toDouble();
            yTrajectoryOrigin=objCloud["yTrajectoryOrigin"].toDouble();
            phase=objCloud["phase"].toDouble();
            move=objCloud["move"].toBool();
            speed=objCloud["speed"].toDouble();
            switch (type)
            {
                case BOUNCING:
                {
                    radius=objCloud["radius"].toDouble();
                    angle=objCloud["angle"].toDouble();
                }
                break;

                case CIRCULAR:
                {

                    centerX=objCloud["centerX"].toDouble();
                    centerY=objCloud["centerY"].toDouble();
                    radius=objCloud["radius"].toDouble();
                    angle=objCloud["angle"].toDouble();
                    stretch=objCloud["stretch"].toDouble();
                    progress=objCloud["progress"].toDouble();
                }
                break;

            case HYPOTROCHOID:
            {
                centerX = objCloud["centerX"].toDouble();
                centerY = objCloud["centerY"].toDouble();
                radius = objCloud["radius"].toDouble();
                radiusInt = objCloud["radiusInt"].toDouble();
                angle = objCloud["angle"].toDouble();
                expansion = objCloud["expansion"].toDouble();
                progress=objCloud["progress"].toDouble();
            }
            break;
            case RECORDED:
            {
                centerX = objCloud["centerX"].toDouble();
                centerY = objCloud["centerY"].toDouble();
                nbPositions = objCloud["positions-number"].toInt();
            }
            break;
            default:
            break;
            }
        }

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
        cout << "spatial channel = " << cloudSpatialChannel << "\n";
        cout << "output first = " << cloudOutputFirst << "\n";
        cout << "output last = " << cloudOutputLast << "\n";
        cout << "midi channel = " << cloudMidiChannel << "\n";
        cout << "midi note = " << cloudMidiNote << "\n";
        cout << "volume = " << cloudVolumeDb << "\n";
        cout << "volume envelope L1 = " << cloudEnvelopeVolume.l1 << "\n";
        cout << "volume envelope L2 = " << cloudEnvelopeVolume.l2 << "\n";
        cout << "volume envelope L3 = " << cloudEnvelopeVolume.l3 << "\n";
        cout << "volume envelope R1 = " << cloudEnvelopeVolume.r1 << "\n";
        cout << "volume envelope R2 = " << cloudEnvelopeVolume.r2 << "\n";
        cout << "volume envelope R3 = " << cloudEnvelopeVolume.r3 << "\n";
        cout << "volume envelope R4 = " << cloudEnvelopeVolume.t4 << "\n";
        cout << "volume envelope T1 = " << cloudEnvelopeVolume.t1 << "\n";
        cout << "volume envelope T2 = " << cloudEnvelopeVolume.t2 << "\n";
        cout << "volume envelope T3 = " << cloudEnvelopeVolume.t3 << "\n";
        cout << "volume envelope T4 = " << cloudEnvelopeVolume.t4 << "\n";
        cout << "grains = " << cloudNumGrains << "\n";
        cout << "active = " << cloudActiveState << "\n";
        cout << "active restart trajectory = " << cloudActiveRestartTrajectory << "\n";
        cout << "X = " << cloudX << "\n";
        cout << "Y = " << cloudY << "\n";
        cout << "X extent = " << cloudXRandExtent << "\n";
        cout << "Y extent = " << cloudYRandExtent << "\n";
        // create audio
        Cloud *cloudToLoad = new Cloud(&m_samples, cloudNumGrains);
        sceneCloud->cloud.reset(cloudToLoad);
        // create visualization
        CloudVis *cloudVisToLoad = new CloudVis(cloudX, cloudY, cloudNumGrains, &m_samples,false);
        sceneCloud->view.reset(cloudVisToLoad);
        // register visualization with audio
        cloudVisToLoad->setSelectState(true);
        cloudToLoad->registerCloudVis(cloudVisToLoad);
        cloudVisToLoad->registerCloud(cloudToLoad);
        cloudToLoad->setId(cloudId);
        cloudToLoad->setName(cloudName);
        cloudToLoad->setDurationMs(cloudDuration);
        cloudToLoad->setOverlap(cloudOverlap);
        cloudToLoad->setPitch(cloudPitch);
        cloudToLoad->setPitchLFOFreq(cloudPitchLFOFreq);
        cloudToLoad->setPitchLFOAmount(cloudPitchLFOAmount);
        cloudToLoad->setDirection(cloudDirection);
        cloudToLoad->setWindowType(cloudWindowType);
        cloudToLoad->setSpatialMode(cloudSpatialMode,cloudSpatialChannel);
        cloudToLoad->setOutputFirst(cloudOutputFirst);
        cloudToLoad->setOutputLast(cloudOutputLast);
        cloudToLoad->setMidiChannel(cloudMidiChannel);
        cloudToLoad->setMidiNote(cloudMidiNote);
        cloudToLoad->setVolumeDb(cloudVolumeDb);
        cloudToLoad->setActiveState(cloudActiveState);
        cloudToLoad->setActiveRestartTrajectory(cloudActiveRestartTrajectory);
        cloudVisToLoad->setIsPlayed(cloudActiveState);
        cloudVisToLoad->setFixedXRandExtent(cloudXRandExtent);
        cloudVisToLoad->setFixedYRandExtent(cloudYRandExtent);
        cloudVisToLoad->setSelectState(false);
        cloudToLoad->setEnvelopeVolumeParam(cloudEnvelopeVolume);


        //cloudVisToLoad->setTrajectoryType(type);
        if(hasTrajectory) {
            Trajectory *tr=nullptr;
            switch (type)
            {
                case BOUNCING:
                {
                    //tr=new Bouncing(bounceWidth,speed,0,xTrajectoryOrigin,yTrajectoryOrigin);
                    tr=new Circular(speed,xTrajectoryOrigin,yTrajectoryOrigin,radius, angle, 0, 1);
                    cloudVisToLoad->setTrajectory(tr);
                    cloudToLoad->setTrajectoryType(BOUNCING);
                    cout << "cloud has radius = " << radius << "\n";
                    cout << "cloud has angle = " << angle << "\n";
                }
                break;

                case CIRCULAR:
                {

                    tr=new Circular(speed,xTrajectoryOrigin,yTrajectoryOrigin,radius, angle, stretch, progress);
                    cloudVisToLoad->setTrajectory(tr);
                    cloudToLoad->setTrajectoryType(CIRCULAR);
                    cout << "cloud has center = (" << centerX << ","<<centerY<<")"<<"\n";
                    cout << "cloud has radius = " << radius << "\n";
                    cout << "cloud has angle = " << angle << "\n";
                    cout << "cloud has stretch = " << stretch << "\n";
                    cout << "cloud has progress = " << progress << "\n";
                }
                break;

                case HYPOTROCHOID:
                {
                    tr=new Hypotrochoid(speed,xTrajectoryOrigin,yTrajectoryOrigin,radius,radiusInt,expansion,angle, progress);
                    cloudVisToLoad->setTrajectory(tr);
                    cloudToLoad->setTrajectoryType(HYPOTROCHOID);
                    cout << "cloud has center = (" << centerX << ","<<centerY<<")"<<"\n";
                    cout << "cloud has radius = " << radius << "\n";
                    cout << "cloud has radius Int = " << radiusInt << "\n";
                    cout << "cloud has angle = " << angle << "\n";
                    cout << "cloud has expansion = " << expansion << "\n";
                    cout << "cloud has progress = " << progress << "\n";
                }
                break;
                case RECORDED:
                {
                    tr=new Recorded(0, xTrajectoryOrigin, yTrajectoryOrigin);
                    Recorded *tr_rec=dynamic_cast<Recorded*>(tr);
                    cloudVisToLoad->setTrajectory(tr);
                    cloudToLoad->setTrajectoryType(RECORDED);
                    QJsonArray docPositions = objCloud["positions"].toArray();

                    for (const QJsonValue &jsonElementPosition : docPositions) { // positions
                        QJsonObject objPosition = jsonElementPosition.toObject();
                        int l_x;
                        int l_y;
                        double l_delay;
                        l_x = objPosition["x"].toInt();
                        l_y = objPosition["y"].toInt();
                        l_delay = objPosition["delay"].toDouble();

                        cloudVisToLoad->trajectoryAddPositionDelayed(l_x, l_y, l_delay);
                    }
                    tr_rec->setRecording(false);
                }
                break;
                default:
                break;
            }

            if(move){
                cloudVisToLoad->startTrajectory();
            }
            else{
                cloudVisToLoad->stopTrajectory();
            }
        }

       cout << "cloud has trajectory type = " << type << "\n";
       cout << "cloud has trajectory = " << hasTrajectory << "\n";
       cout << "cloud is moving = " << move << "\n";
       cout << "cloud phase = " << phase << "\n";
       cout << "cloud trajectory x origin = " << xTrajectoryOrigin << "\n";
       cout << "cloud trajectory y origin = " << yTrajectoryOrigin << "\n";
       cout << "cloud has speed = " <<speed <<"\n";
       cout << "cloud has radius = " <<radius <<"\n";
       cout << "cloud has angle = " <<angle <<"\n";
       cout << "cloud has stretch = " <<stretch <<"\n";
       cout << "cloud has progress = " <<progress <<"\n";

    }

    // triggers

    QJsonArray docTriggers = docRoot["triggers"].toArray();
    m_triggers.clear();
    m_triggers.reserve(docTriggers.size());
    for (const QJsonValue &jsonElement : docTriggers) {
        QJsonObject objTrigger = jsonElement.toObject();
        SceneTrigger *sceneTrigger = new SceneTrigger;
        m_triggers.emplace_back(sceneTrigger);

        int triggerId = objTrigger["id"].toInt();
        QString triggerName = objTrigger["name"].toString();
        int triggerActiveState = objTrigger["active-state"].toBool();
        int triggerActiveRestartTrajectory = objTrigger["active-restart-trajectory"].toBool();
        int triggerIn = objTrigger["in"].toInt();
        int triggerOut = objTrigger["out"].toInt();
        int triggerPriority = objTrigger["priority"].toInt();
        double triggerX = objTrigger["x"].toDouble();
        double triggerY = objTrigger["y"].toDouble();
        int triggerZoneExtent = objTrigger["zone-extent"].toInt();

        //common trajectory parameters
        bool hasTrajectory=objTrigger["has-trajectory"].toBool();
        int type=0;
        double xTrajectoryOrigin=0;
        double yTrajectoryOrigin=0;
        double phase=0;
        bool move=false;
        double speed=0;

        //specific trajectory parameters

        double centerX=0;
        double centerY=0;
        double radius=0;
        double angle=0;
        double stretch=0;
        double radiusInt=0;
        double expansion=0;
        double progress=0;
        int nbPositions=0;

        if(hasTrajectory)
        {
            type=objTrigger["trajectory-type"].toInt();
            xTrajectoryOrigin=objTrigger["xTrajectoryOrigin"].toDouble();
            yTrajectoryOrigin=objTrigger["yTrajectoryOrigin"].toDouble();
            phase=objTrigger["phase"].toDouble();
            move=objTrigger["move"].toBool();
            speed=objTrigger["speed"].toDouble();
            switch (type)
            {
                case BOUNCING:
                {
                    radius=objTrigger["radius"].toDouble();
                    angle=objTrigger["angle"].toDouble();
                }
                break;

                case CIRCULAR:
                {

                    centerX=objTrigger["centerX"].toDouble();
                    centerY=objTrigger["centerY"].toDouble();
                    radius=objTrigger["radius"].toDouble();
                    angle=objTrigger["angle"].toDouble();
                    stretch=objTrigger["stretch"].toDouble();
                    progress=objTrigger["progress"].toDouble();
                }
                break;

            case HYPOTROCHOID:
            {
                centerX = objTrigger["centerX"].toDouble();
                centerY = objTrigger["centerY"].toDouble();
                radius = objTrigger["radius"].toDouble();
                radiusInt = objTrigger["radiusInt"].toDouble();
                angle = objTrigger["angle"].toDouble();
                expansion = objTrigger["expansion"].toDouble();
                progress=objTrigger["progress"].toDouble();
            }
            break;
            case RECORDED:
            {
                centerX = objTrigger["centerX"].toDouble();
                centerY = objTrigger["centerY"].toDouble();
                nbPositions = objTrigger["positions-number"].toInt();
            }
            break;
            default:
            break;
            }
        }

        cout << "trigger " << m_triggers.size() << " :" << "\n";
        cout << "id : " << triggerId << "\n";
        cout << "priority " << triggerPriority << "\n";
        cout << "active = " << triggerActiveState << "\n";
        cout << "active restart trajectory = " << triggerActiveRestartTrajectory << "\n";
        cout << "in = " << triggerIn << "\n";
        cout << "out = " << triggerOut << "\n";
        cout << "X = " << triggerX << "\n";
        cout << "Y = " << triggerY << "\n";
        cout << "extent = " << triggerZoneExtent << "\n";
        // create trigger
        Trigger *triggerToLoad = new Trigger();
        sceneTrigger->trigger.reset(triggerToLoad);
        // create visualization
        TriggerVis *triggerVisToLoad = new TriggerVis(triggerX, triggerY);
        sceneTrigger->view.reset(triggerVisToLoad);
        // register visualization with trigger
        triggerVisToLoad->setSelectState(true);
        triggerToLoad->registerTriggerVis(triggerVisToLoad);
        triggerVisToLoad->registerTrigger(triggerToLoad);
        triggerToLoad->setId(triggerId);
        triggerToLoad->setName(triggerName);
        triggerToLoad->setPriority(triggerPriority);
        triggerToLoad->setActiveState(triggerActiveState);
        triggerToLoad->setActiveRestartTrajectory(triggerActiveRestartTrajectory);
        triggerToLoad->setIn(triggerIn);
        triggerToLoad->setOut(triggerOut);
        triggerVisToLoad->setIsPlayed(triggerActiveState);
        triggerVisToLoad->setFixedZoneExtent(triggerZoneExtent, triggerZoneExtent);
        triggerVisToLoad->setSelectState(false);

        if(hasTrajectory) {
            Trajectory *tr=nullptr;
            switch (type)
            {
                case BOUNCING:
                {
                    //tr=new Bouncing(bounceWidth,speed,0,xTrajectoryOrigin,yTrajectoryOrigin);
                    tr=new Circular(speed,xTrajectoryOrigin,yTrajectoryOrigin,radius, angle, 0, 1);
                    triggerVisToLoad->setTrajectory(tr);
                    triggerToLoad->setTrajectoryType(BOUNCING);
                    cout << "trigger has radius = " << radius << "\n";
                    cout << "trigger has angle = " << angle << "\n";
                }
                break;

                case CIRCULAR:
                {

                    tr=new Circular(speed,xTrajectoryOrigin,yTrajectoryOrigin,radius, angle, stretch, progress);
                    triggerVisToLoad->setTrajectory(tr);
                    triggerToLoad->setTrajectoryType(CIRCULAR);
                    cout << "trigger has center = (" << centerX << ","<<centerY<<")"<<"\n";
                    cout << "trigger has radius = " << radius << "\n";
                    cout << "trigger has angle = " << angle << "\n";
                    cout << "trigger has stretch = " << stretch << "\n";
                    cout << "trigger has progress = " << progress << "\n";
                }
                break;

                case HYPOTROCHOID:
                {
                    tr=new Hypotrochoid(speed,xTrajectoryOrigin,yTrajectoryOrigin,radius,radiusInt,expansion,angle, progress);
                    triggerVisToLoad->setTrajectory(tr);
                    triggerToLoad->setTrajectoryType(HYPOTROCHOID);
                    cout << "trigger has center = (" << centerX << ","<<centerY<<")"<<"\n";
                    cout << "trigger has radius = " << radius << "\n";
                    cout << "trigger has radius Int = " << radiusInt << "\n";
                    cout << "trigger has angle = " << angle << "\n";
                    cout << "trigger has expansion = " << expansion << "\n";
                    cout << "trigger has progress = " << progress << "\n";
                }
                break;
                case RECORDED:
                {
                    tr=new Recorded(0, xTrajectoryOrigin, yTrajectoryOrigin);
                    Recorded *tr_rec=dynamic_cast<Recorded*>(tr);
                    triggerVisToLoad->setTrajectory(tr);
                    triggerToLoad->setTrajectoryType(RECORDED);
                    QJsonArray docPositions = objTrigger["positions"].toArray();

                    for (const QJsonValue &jsonElementPosition : docPositions) { // positions
                        QJsonObject objPosition = jsonElementPosition.toObject();
                        int l_x;
                        int l_y;
                        double l_delay;
                        l_x = objPosition["x"].toInt();
                        l_y = objPosition["y"].toInt();
                        l_delay = objPosition["delay"].toDouble();

                        triggerVisToLoad->trajectoryAddPositionDelayed(l_x, l_y, l_delay);
                    }
                    tr_rec->setRecording(false);
                }
                break;
                default:
                break;
            }

            if(move){
                triggerVisToLoad->startTrajectory();
            }
            else{
                triggerVisToLoad->stopTrajectory();
            }
        }

       cout << "trigger has trajectory type = " << type << "\n";
       cout << "trigger has trajectory = " << hasTrajectory << "\n";
       cout << "trigger is moving = " << move << "\n";
       cout << "trigger phase = " << phase << "\n";
       cout << "trigger trajectory x origin = " << xTrajectoryOrigin << "\n";
       cout << "trigger trajectory y origin = " << yTrajectoryOrigin << "\n";
       cout << "trigger has speed = " <<speed <<"\n";
       cout << "trigger has radius = " <<radius <<"\n";
       cout << "trigger has angle = " <<angle <<"\n";
       cout << "trigger has stretch = " <<stretch <<"\n";
       cout << "trigger has progress = " <<progress <<"\n";

    }

    // midi combis

    QJsonArray docCombis = docRoot["combis"].toArray();
    for (const QJsonValue &jsonElementCombi : docCombis) { // combis
        Combination l_combi;

        QJsonObject objCombi = jsonElementCombi.toObject();

        int l_numCombi = objCombi["num"].toInt();
        QString l_nameCombi = objCombi["name"].toString();

        m_midiBank.createCombi(l_numCombi);
        l_combi.setName(l_nameCombi);

        QJsonArray docNotes = objCombi["notes"].toArray();
        for (const QJsonValue &jsonElementNote : docNotes) { // notes
            QJsonObject objNote = jsonElementNote.toObject();
            int l_numNote = objNote["num"].toInt();
            QJsonArray docLayers = objNote["layers"].toArray();
            Note l_note = l_combi.getNote(l_numNote);
            l_note.cloudLayer.clear();
            l_note.cloudLayer.reserve(docLayers.size());
            for (const QJsonValue &jsonElementLayer : docLayers) { // layers
                QJsonObject objLayer = jsonElementLayer.toObject();
                Layer l_layer;
                l_layer.cloudId = objLayer["cloud id"].toInt();
                l_layer.velocity.max = objLayer["velocity max"].toInt();
                l_layer.velocity.min = objLayer["velocity min"].toInt();
                l_note.cloudLayer.emplace_back(l_layer);
            }
            l_combi.setNote(l_note, l_numNote);
        }
        m_midiBank.updateCombi(l_combi, l_numCombi);
    }

    // midi instrument

    QJsonArray docInstruments = docRoot["instruments"].toArray();
    for (const QJsonValue &jsonElementInstrument : docInstruments) { // instruments
        QJsonObject objInstrument = jsonElementInstrument.toObject();

        int l_channel= objInstrument["channel"].toInt();
        int l_numCombi = objInstrument["combi"].toInt();

        m_midiInstrument.setMidiCombi(l_channel, l_numCombi);
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
    m_selectedSample = -1;
    m_selectionIndex = 0;
    m_selectionIndices.clear();

    QJsonObject docRoot;

    // audio path
    cout << "record scene " << sceneFile.fileName().toStdString() << "\n";

    QJsonArray docVersion;
    QJsonObject objVersion;
    objVersion["major"] = version_major;
    objVersion["minor"] = version_minor;
    objVersion["patch"] = version_patch;

    docRoot["version"] = objVersion;
    docVersion.append(objVersion);


    QJsonArray docPaths;
    for (const std::string &path : m_audioPaths) {
        QString qpath = QString::fromStdString(path);
        // save the paths under scene directory as relative
        QString relative = sceneFileDir.relativeFilePath(qpath);
        docPaths.append(relative.startsWith("../") ? qpath : relative);
    }
    docRoot["audio-path"] = docPaths;

    // samples
    QJsonArray docSamples;
    cout << m_samples.size() << "samples : " << "\n";
    for (int i = 0, n = m_samples.size(); i < n; ++i) {
        SceneSample *sceneSample = m_samples[i].get();
        SampleVis *sv = sceneSample->view.get();

        std::ostream &out = std::cout;
        out << "Audio file " << i << ":\n";
        out << "- name : " << sceneSample->name << "\n";
        out << "Sample visual " << i << ":\n";
        sv->describe(out);

        QJsonObject objSample;
        objSample["name"] = QString::fromStdString(sceneSample->name);
        objSample["orientation"] = (int)sv->getOrientation();
        objSample["height"] = sv->getHeight();
        objSample["width"] = sv->getWidth();
        objSample["x"] = sv->getX();
        objSample["y"] = sv->getY();
        docSamples.append(objSample);
    }

    // clouds
    QJsonArray docClouds;
    for (int i = 0, n = m_clouds.size(); i < n; i++) {
        SceneCloud *cloud = m_clouds[i].get();
        Cloud *cloudToSave = cloud->cloud.get();
        CloudVis *cloudVisToSave = cloud->view.get();

        std::ostream &out = std::cout;
        out << "Cloud " << i << ":";
        cloudToSave->describe(out);
        out << "Cloud Vis " << i << ":";
        cloudVisToSave->describe(out);

        QJsonObject objCloud;
        objCloud["id"] = (int)cloudToSave->getId();
        objCloud["name"] = cloudToSave->getName();
        objCloud["duration"] = cloudToSave->getDurationMs();
        objCloud["overlap"] = cloudToSave->getOverlap();
        objCloud["pitch"] = cloudToSave->getPitch();
        objCloud["pitch-lfo-freq"] = cloudToSave->getPitchLFOFreq();
        objCloud["pitch-lfo-amount"] = cloudToSave->getPitchLFOAmount();
        objCloud["direction"] = cloudToSave->getDirection();
        objCloud["window-type"] = cloudToSave->getWindowType();
        objCloud["spatial-mode"] = cloudToSave->getSpatialMode();
        objCloud["output-first"] = cloudToSave->getOutputFirst();
        objCloud["output-last"] = cloudToSave->getOutputLast();
        objCloud["spatial-channel"] = cloudToSave->getSpatialChannel();
        objCloud["midi-channel"] = cloudToSave->getMidiChannel();
        objCloud["midi-note"] = cloudToSave->getMidiNote();
        objCloud["volume"] = cloudToSave->getVolumeDb();
        const ParamEnv &cloudEnvelopeVolumeParam = cloudToSave->getEnvelopeVolumeParam();
        {
            float tAtk, tSta, tDec, tRel;
            cloudEnvelopeVolumeParam.getTimeBasedParameters(tAtk, tSta, tDec, tRel, samp_rate);
            objCloud["volume-envelope-L1"] = cloudEnvelopeVolumeParam.l1;
            objCloud["volume-envelope-L2"] = cloudEnvelopeVolumeParam.l2;
            objCloud["volume-envelope-L3"] = cloudEnvelopeVolumeParam.l3;
            objCloud["volume-envelope-TAtk"] = tAtk;
            objCloud["volume-envelope-TSta"] = tSta;
            objCloud["volume-envelope-TDec"] = tDec;
            objCloud["volume-envelope-TRel"] = tRel;
        }
        objCloud["volume-envelope-T1"] = cloudEnvelopeVolumeParam.t1;
        objCloud["volume-envelope-T2"] = cloudEnvelopeVolumeParam.t2;
        objCloud["volume-envelope-T3"] = cloudEnvelopeVolumeParam.t3;
        objCloud["volume-envelope-T4"] = cloudEnvelopeVolumeParam.t4;
        objCloud["num-grains"] = (int)cloudToSave->getNumGrains();
        objCloud["active-state"] = cloudToSave->getActiveState();
        objCloud["active-restart-trajectory"] = cloudToSave->getActiveRestartTrajectory();
        objCloud["x"] = cloudVisToSave->getOriginX();
        objCloud["y"] = cloudVisToSave->getOriginY();
        objCloud["x-rand-extent"] = cloudVisToSave->getXRandExtent();
        objCloud["y-rand-extent"] = cloudVisToSave->getYRandExtent();

        //trajectory
        objCloud["has-trajectory"] = cloudVisToSave->hasTrajectory();
        if(cloudVisToSave->hasTrajectory()){
            objCloud["trajectory-type"] = cloudToSave->getTrajectoryType();
            int type = cloudToSave->getTrajectoryType();

            switch ( type )
            {
                case BOUNCING:
                {
                    Circular *b = dynamic_cast<Circular*>(cloudVisToSave->getTrajectory());
                    objCloud["radius"]=b->getRadius();
                    objCloud["angle"]=b->getAngle();
                    std::cout<<"radius saved "<<b->getRadius()<<std::endl;
                    std::cout<<"angle saved "<<b->getAngle()<<std::endl;
                }
                break;

                case CIRCULAR:
                {
                    Circular *c=dynamic_cast<Circular*>(cloudVisToSave->getTrajectory());
                    objCloud["radius"]=c->getRadius();
                    objCloud["centerX"]=c->getCenterX();
                    objCloud["centerY"]=c->getCenterY();
                    objCloud["angle"]=c->getAngle();
                    objCloud["stretch"]=c->getStretch();
                    objCloud["progress"]=c->getProgress();
                    std::cout<<"centerX saved "<<c->getCenterX()<<std::endl;
                    std::cout<<"centerY saved "<<c->getCenterY()<<std::endl;
                    std::cout<<"radius saved "<<c->getRadius()<<std::endl;
                    std::cout<<"angle saved "<<c->getAngle()<<std::endl;
                    std::cout<<"stretch saved "<<c->getStretch()<<std::endl;
                    std::cout<<"progress saved "<<c->getProgress()<<std::endl;
                }
                break;

            case HYPOTROCHOID:
            {
                Hypotrochoid *c=dynamic_cast<Hypotrochoid*>(cloudVisToSave->getTrajectory());
                objCloud["radius"]=c->getRadius();
                objCloud["centerX"]=c->getCenterX();
                objCloud["centerY"]=c->getCenterY();
                objCloud["angle"]=c->getAngle();
                objCloud["radiusInt"]=c->getRadiusInt();
                objCloud["expansion"]=c->getExpansion();
                objCloud["progress"]=c->getProgress();
                std::cout<<"centerX saved "<<c->getCenterX()<<std::endl;
                std::cout<<"centerY saved "<<c->getCenterY()<<std::endl;
                std::cout<<"radius saved "<<c->getRadius()<<std::endl;
                std::cout<<"radius Int saved "<<c->getRadiusInt()<<std::endl;
                std::cout<<"angle saved "<<c->getAngle()<<std::endl;
                std::cout<<"expansion saved "<<c->getExpansion()<<std::endl;
                std::cout<<"progress saved "<<c->getProgress()<<std::endl;
            }
            break;
            case RECORDED:
            {
                Recorded *c=dynamic_cast<Recorded*>(cloudVisToSave->getTrajectory());
                objCloud["positions-number"] = c->lastPosition();
                std::cout<<"number positions saved "<<c->getCenterX()<<std::endl;
                QJsonArray docPositions;
                for (int j = 1; j < c->lastPosition(); j++){
                    QJsonObject objPosition;
                    objPosition["num"] = j;
                    objPosition["x"] = c->getPosition(j).x;
                    objPosition["y"] = c->getPosition(j).y;
                    objPosition["delay"] = c->getPosition(j).delay;
                    std::cout<<"num "<<j<<std::endl;
                    std::cout<<"x "<<c->getPosition(j).x<<std::endl;
                    std::cout<<"y "<<c->getPosition(j).y<<std::endl;
                    std::cout<<"delay "<<c->getPosition(j).delay<<std::endl;
                    docPositions.append(objPosition);
                }
                objCloud["positions"] = docPositions;
            }
            break;
            default:
            break;

            }

            objCloud["xTrajectoryOrigin"] = cloudVisToSave->getTrajectory()->getOrigin().x;
            objCloud["yTrajectoryOrigin"] = cloudVisToSave->getTrajectory()->getOrigin().y;
            objCloud["phase"] = cloudVisToSave->getTrajectory()->getPhase();
            objCloud["move"] = cloudVisToSave->getIsMoving();
            objCloud["speed"] = cloudVisToSave->getTrajectory()->getSpeed();
        }

        docClouds.append(objCloud);
    }

    // triggers
    QJsonArray docTriggers;
    for (int i = 0, n = m_triggers.size(); i < n; i++) {
        SceneTrigger *trigger = m_triggers[i].get();
        Trigger *triggerToSave = trigger->trigger.get();
        TriggerVis *triggerVisToSave = trigger->view.get();

        std::ostream &out = std::cout;
        out << "Trigger " << i << ":";
        triggerToSave->describe(out);
        out << "Trigger Vis " << i << ":";
        triggerVisToSave->describe(out);

        QJsonObject objTrigger;
        objTrigger["id"] = (int)triggerToSave->getId();
        objTrigger["name"] = triggerToSave->getName();
        objTrigger["priority"] = triggerToSave->getPriority();
        objTrigger["active-state"] = triggerToSave->getActiveState();
        objTrigger["active-restart-trajectory"] = triggerToSave->getActiveRestartTrajectory();
        objTrigger["in"] = triggerToSave->getIn();
        objTrigger["out"] = triggerToSave->getOut();
        objTrigger["x"] = triggerVisToSave->getOriginX();
        objTrigger["y"] = triggerVisToSave->getOriginY();
        objTrigger["extent"] = triggerVisToSave->getZoneExtent();

        //trajectory
        objTrigger["has-trajectory"] = triggerVisToSave->hasTrajectory();
        if(triggerVisToSave->hasTrajectory()){
            objTrigger["trajectory-type"] = triggerToSave->getTrajectoryType();
            int type = triggerToSave->getTrajectoryType();

            switch ( type )
            {
                case BOUNCING:
                {
                    Circular *b = dynamic_cast<Circular*>(triggerVisToSave->getTrajectory());
                    objTrigger["radius"]=b->getRadius();
                    objTrigger["angle"]=b->getAngle();
                    std::cout<<"radius saved "<<b->getRadius()<<std::endl;
                    std::cout<<"angle saved "<<b->getAngle()<<std::endl;
                }
                break;

                case CIRCULAR:
                {
                    Circular *c=dynamic_cast<Circular*>(triggerVisToSave->getTrajectory());
                    objTrigger["radius"]=c->getRadius();
                    objTrigger["centerX"]=c->getCenterX();
                    objTrigger["centerY"]=c->getCenterY();
                    objTrigger["angle"]=c->getAngle();
                    objTrigger["stretch"]=c->getStretch();
                    objTrigger["progress"]=c->getProgress();
                    std::cout<<"centerX saved "<<c->getCenterX()<<std::endl;
                    std::cout<<"centerY saved "<<c->getCenterY()<<std::endl;
                    std::cout<<"radius saved "<<c->getRadius()<<std::endl;
                    std::cout<<"angle saved "<<c->getAngle()<<std::endl;
                    std::cout<<"stretch saved "<<c->getStretch()<<std::endl;
                    std::cout<<"progress saved "<<c->getProgress()<<std::endl;
                }
                break;

            case HYPOTROCHOID:
            {
                Hypotrochoid *c=dynamic_cast<Hypotrochoid*>(triggerVisToSave->getTrajectory());
                objTrigger["radius"]=c->getRadius();
                objTrigger["centerX"]=c->getCenterX();
                objTrigger["centerY"]=c->getCenterY();
                objTrigger["angle"]=c->getAngle();
                objTrigger["radiusInt"]=c->getRadiusInt();
                objTrigger["expansion"]=c->getExpansion();
                objTrigger["progress"]=c->getProgress();
                std::cout<<"centerX saved "<<c->getCenterX()<<std::endl;
                std::cout<<"centerY saved "<<c->getCenterY()<<std::endl;
                std::cout<<"radius saved "<<c->getRadius()<<std::endl;
                std::cout<<"radius Int saved "<<c->getRadiusInt()<<std::endl;
                std::cout<<"angle saved "<<c->getAngle()<<std::endl;
                std::cout<<"expansion saved "<<c->getExpansion()<<std::endl;
                std::cout<<"progress saved "<<c->getProgress()<<std::endl;
            }
            break;
            case RECORDED:
            {
                Recorded *c=dynamic_cast<Recorded*>(triggerVisToSave->getTrajectory());
                objTrigger["positions-number"] = c->lastPosition();
                std::cout<<"number positions saved "<<c->getCenterX()<<std::endl;
                QJsonArray docPositions;
                for (int j = 1; j < c->lastPosition(); j++){
                    QJsonObject objPosition;
                    objPosition["num"] = j;
                    objPosition["x"] = c->getPosition(j).x;
                    objPosition["y"] = c->getPosition(j).y;
                    objPosition["delay"] = c->getPosition(j).delay;
                    std::cout<<"num "<<j<<std::endl;
                    std::cout<<"x "<<c->getPosition(j).x<<std::endl;
                    std::cout<<"y "<<c->getPosition(j).y<<std::endl;
                    std::cout<<"delay "<<c->getPosition(j).delay<<std::endl;
                    docPositions.append(objPosition);
                }
                objTrigger["positions"] = docPositions;
            }
            break;
            default:
            break;

            }

            objTrigger["xTrajectoryOrigin"] = triggerVisToSave->getTrajectory()->getOrigin().x;
            objTrigger["yTrajectoryOrigin"] = triggerVisToSave->getTrajectory()->getOrigin().y;
            objTrigger["phase"] = triggerVisToSave->getTrajectory()->getPhase();
            objTrigger["move"] = triggerVisToSave->getIsMoving();
            objTrigger["speed"] = triggerVisToSave->getTrajectory()->getSpeed();
        }

        docTriggers.append(objTrigger);
    }

    // midi combinations

    QJsonArray docCombis;
    for (int i = 0; i <= 127; i++){
        if (m_midiBank.existCombi(i)) {
            QJsonObject objCombi;
            objCombi["num"] = i;
            objCombi["name"] = m_midiBank.findCombi(i).getName();
            QJsonArray docNotes;
            for (int j = 0; j <= 127; j++){
                int n_layers = m_midiBank.findCombi(i).getNote(j).cloudLayer.size();
                if (n_layers > 0) {
                    QJsonObject objNote;
                    objNote["num"] = j;
                    QJsonArray docLayers;
                    for (int k = 0; k < n_layers; k++) {
                        QJsonObject objLayer;
                        objLayer["num"] = k;
                        objLayer["cloud id"] = m_midiBank.findCombi(i).getNote(j).cloudLayer[k].cloudId;
                        objLayer["velocity min"] = m_midiBank.findCombi(i).getNote(j).cloudLayer[k].velocity.min;
                        objLayer["velocity max"] = m_midiBank.findCombi(i).getNote(j).cloudLayer[k].velocity.max;

                        docLayers.append(objLayer);
                    }

                    objNote["layers"] = docLayers;
                    docNotes.append(objNote);
                }
            }

            objCombi["notes"] = docNotes;
            docCombis.append(objCombi);
        }
    }

    // midi instrument

    QJsonArray docInstruments;
    for (int i = 1; i <= 16; i++){
        QJsonObject objInstrument;
        objInstrument["channel"] = i;
        objInstrument["combi"] = m_midiInstrument.getMidiCombi(i);

        docInstruments.append(objInstrument);
    }

    docRoot["version"] = docVersion;
    docRoot["samples"] = docSamples;
    docRoot["clouds"] = docClouds;
    docRoot["triggers"] = docTriggers;
    docRoot["combis"] = docCombis;
    docRoot["instruments"] = docInstruments;

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

    QJsonObject objCloud = docRoot["cloud"].toObject();
    g_defaultCloudParams.duration = objCloud["duration"].toDouble();
    g_defaultCloudParams.overlap = objCloud["overlap"].toDouble();
    g_defaultCloudParams.pitch = objCloud["pitch"].toDouble();
    g_defaultCloudParams.pitchLFOFreq = objCloud["pitch-lfo-freq"].toDouble();
    g_defaultCloudParams.pitchLFOAmount = objCloud["pitch-lfo-amount"].toDouble();
    g_defaultCloudParams.dirMode = objCloud["direction"].toInt();
    g_defaultCloudParams.windowType = objCloud["window-type"].toInt();
    g_defaultCloudParams.outputFirst = objCloud["output-first"].toInt();
    g_defaultCloudParams.outputLast = objCloud["output-last"].toInt();
    g_defaultCloudParams.spatialMode = objCloud["spatial-mode"].toInt();
    g_defaultCloudParams.channelLocation = objCloud["spatial-channel"].toInt();
    g_defaultCloudParams.midiChannel = objCloud["midi-channel"].toInt();
    g_defaultCloudParams.midiNote = objCloud["midi-note"].toInt();
    g_defaultCloudParams.volumeDB = objCloud["volume"].toDouble();
    ParamEnv cloudEnvelopeVolume;
    {
        double l1 = objCloud["volume-envelope-L1"].toDouble();
        double l2 = objCloud["volume-envelope-L2"].toDouble();
        double l3 = objCloud["volume-envelope-L3"].toDouble();
        double tAtk = objCloud["volume-envelope-TAtk"].toDouble();
        double tSta = objCloud["volume-envelope-TSta"].toDouble();
        double tDec = objCloud["volume-envelope-TDec"].toDouble();
        double tRel = objCloud["volume-envelope-TRel"].toDouble();
        cloudEnvelopeVolume.setTimeBasedParameters(
            l1, l2, l3,
            tAtk, tSta, tDec, tRel,
            samp_rate);
    }
    g_defaultCloudParams.envelope.l1 = cloudEnvelopeVolume.l1;
    g_defaultCloudParams.envelope.l2 = cloudEnvelopeVolume.l2;
    g_defaultCloudParams.envelope.l3 = cloudEnvelopeVolume.l3;
    g_defaultCloudParams.envelope.r1 = cloudEnvelopeVolume.r1;
    g_defaultCloudParams.envelope.r2 = cloudEnvelopeVolume.r2;
    g_defaultCloudParams.envelope.r3 = cloudEnvelopeVolume.r3;
    g_defaultCloudParams.envelope.r4 = cloudEnvelopeVolume.r4;
    g_defaultCloudParams.envelope.t1 = objCloud["volume-envelope-T1"].toInt();
    g_defaultCloudParams.envelope.t2 = objCloud["volume-envelope-T2"].toInt();
    g_defaultCloudParams.envelope.t3 = objCloud["volume-envelope-T3"].toInt();
    g_defaultCloudParams.envelope.t4 = objCloud["volume-envelope-T4"].toInt();
    g_defaultCloudParams.numGrains = objCloud["num-grains"].toInt();
    g_defaultCloudParams.activateState = objCloud["active-state"].toBool();
    g_defaultCloudParams.activateRestartTrajectory = objCloud["active-restart-trajectory"].toBool();
    g_defaultCloudParams.xRandExtent = objCloud["x-rand-extent"].toDouble();
    g_defaultCloudParams.yRandExtent = objCloud["y-rand-extent"].toDouble();
    g_defaultCloudParams.hasTrajectory = objCloud["hastrajectory"].toBool();
    g_defaultCloudParams.trajectoryType = objCloud["trajectory-type"].toInt();
    g_defaultCloudParams.radius = objCloud["radius"].toDouble();
    g_defaultCloudParams.speed = objCloud["speed"].toDouble();
    g_defaultCloudParams.angle = objCloud["angle"].toDouble();
    g_defaultCloudParams.stretch = objCloud["stretch"].toDouble();
    g_defaultCloudParams.radiusInt = objCloud["radius-int"].toDouble();
    g_defaultCloudParams.expansion = objCloud["expansion"].toDouble();
    g_defaultCloudParams.progress = objCloud["progress"].toDouble();

    for (int i = 0; i < g_defaultCloudParams.myPosition.size(); i++) {
        delete g_defaultCloudParams.myPosition[i];
    }

    if (g_defaultCloudParams.trajectoryType == RECORDED) {
        Position *first_Position = new Position;
        first_Position->x = 0;
        first_Position->y = 0;
        first_Position->delay = 0;
        g_defaultCloudParams.myPosition.push_back(first_Position);
        QJsonArray docPositions = objCloud["positions"].toArray();
        for (const QJsonValue &jsonElementPosition : docPositions) { // positions
            QJsonObject objPosition = jsonElementPosition.toObject();
            Position *tr_Position = new Position;
            tr_Position->x = objPosition["x"].toInt();
            tr_Position->y = objPosition["y"].toInt();
            tr_Position->delay = objPosition["delay"].toDouble();

            g_defaultCloudParams.myPosition.push_back(tr_Position);
        }
        for (int i = 0; i < g_defaultCloudParams.myPosition.size(); i = i + 1) { // positions
            int l_x;
            int l_y;
            double l_delay;
            l_x = g_defaultCloudParams.myPosition[i]->x;
            l_y = g_defaultCloudParams.myPosition[i]->y;
            l_delay = g_defaultCloudParams.myPosition[i]->delay;
        }
    }

    cout << "duration = " << g_defaultCloudParams.duration << "\n";
    cout << "overlap = " << g_defaultCloudParams.overlap << "\n";
    cout << "pitch = " << g_defaultCloudParams.pitch << "\n";
    cout << "pitchLFOFreq = " << g_defaultCloudParams.pitchLFOFreq << "\n";
    cout << "pitchLFOAmount = " << g_defaultCloudParams.pitchLFOAmount << "\n";
    cout << "direction = " << g_defaultCloudParams.dirMode << "\n";
    cout << "window type = " << g_defaultCloudParams.windowType << "\n";
    cout << "spatial mode = " << g_defaultCloudParams.spatialMode << "\n";
    cout << "spatial channel = " << g_defaultCloudParams.channelLocation << "\n";
    cout << "output first = " << g_defaultCloudParams.outputFirst << "\n";
    cout << "output last = " << g_defaultCloudParams.outputLast << "\n";
    cout << "midi channel = " << g_defaultCloudParams.midiChannel << "\n";
    cout << "midi note = " << g_defaultCloudParams.midiNote << "\n";
    cout << "volume = " << g_defaultCloudParams.volumeDB << "\n";
    cout << "grains = " << g_defaultCloudParams.numGrains << "\n";
    cout << "active = " << g_defaultCloudParams.activateState << "\n";
    cout << "active restart trajectory = " << g_defaultCloudParams.activateRestartTrajectory << "\n";
    cout << "xRandExtent = " << g_defaultCloudParams.xRandExtent << "\n";
    cout << "yRandExtent = " << g_defaultCloudParams.yRandExtent << "\n";
    cout << "volume-envelope-L1 = " << g_defaultCloudParams.envelope.l1 << "\n";
    cout << "volume-envelope-L2 = " << g_defaultCloudParams.envelope.l2 << "\n";
    cout << "volume-envelope-L3 = " << g_defaultCloudParams.envelope.l3 << "\n";
    cout << "volume-envelope-R1 = " << g_defaultCloudParams.envelope.r1 << "\n";
    cout << "volume-envelope-R2 = " << g_defaultCloudParams.envelope.r2 << "\n";
    cout << "volume-envelope-R3 = " << g_defaultCloudParams.envelope.r3 << "\n";
    cout << "volume-envelope-R4 = " << g_defaultCloudParams.envelope.r4 << "\n";
    cout << "volume-envelope-T1 = " << g_defaultCloudParams.envelope.t1 << "\n";
    cout << "volume-envelope-T2 = " << g_defaultCloudParams.envelope.t2 << "\n";
    cout << "volume-envelope-T3 = " << g_defaultCloudParams.envelope.t3 << "\n";
    cout << "volume-envelope-T4 = " << g_defaultCloudParams.envelope.t4 << "\n";
    cout << "hastrajectory = " << g_defaultCloudParams.hasTrajectory << "\n";
    cout << "trajectory type = " << g_defaultCloudParams.trajectoryType << "\n";
    cout << "speed = " << g_defaultCloudParams.speed << "\n";
    cout << "radius = " << g_defaultCloudParams.radius << "\n";
    cout << "angle = " << g_defaultCloudParams.angle << "\n";
    cout << "stretch = " << g_defaultCloudParams.stretch << "\n";
    cout << "radiusInt = " << g_defaultCloudParams.radiusInt << "\n";
    cout << "expansion = " << g_defaultCloudParams.expansion << "\n";
    cout << "progress = " << g_defaultCloudParams.progress << "\n";

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

    QJsonObject objCloud;
    objCloud["duration"] = cloudToSave->getDurationMs();
    objCloud["overlap"] = cloudToSave->getOverlap();
    objCloud["pitch"] = cloudToSave->getPitch();
    objCloud["pitch-lfo-freq"] = cloudToSave->getPitchLFOFreq();
    objCloud["pitch-lfo-amount"] = cloudToSave->getPitchLFOAmount();
    objCloud["direction"] = cloudToSave->getDirection();
    objCloud["window-type"] = cloudToSave->getWindowType();
    objCloud["spatial-mode"] = cloudToSave->getSpatialMode();
    objCloud["spatial-channel"] = cloudToSave->getSpatialChannel();
    objCloud["output-first"] = cloudToSave->getOutputFirst();
    objCloud["output-last"] = cloudToSave->getOutputLast();
    objCloud["midi-channel"] = cloudToSave->getMidiChannel();
    objCloud["midi-note"] = cloudToSave->getMidiNote();
    objCloud["volume"] = cloudToSave->getVolumeDb();
    const ParamEnv &cloudEnvelopeVolumeParam = cloudToSave->getEnvelopeVolumeParam();
    {
        float tAtk, tSta, tDec, tRel;
        cloudEnvelopeVolumeParam.getTimeBasedParameters(tAtk, tSta, tDec, tRel, samp_rate);
        objCloud["volume-envelope-L1"] = cloudEnvelopeVolumeParam.l1;
        objCloud["volume-envelope-L2"] = cloudEnvelopeVolumeParam.l2;
        objCloud["volume-envelope-L3"] = cloudEnvelopeVolumeParam.l3;
        objCloud["volume-envelope-TAtk"] = tAtk;
        objCloud["volume-envelope-TSta"] = tSta;
        objCloud["volume-envelope-TDec"] = tDec;
        objCloud["volume-envelope-TRel"] = tRel;
    }
    objCloud["volume-envelope-T1"] = cloudEnvelopeVolumeParam.t1;
    objCloud["volume-envelope-T2"] = cloudEnvelopeVolumeParam.t2;
    objCloud["volume-envelope-T3"] = cloudEnvelopeVolumeParam.t3;
    objCloud["volume-envelope-T4"] = cloudEnvelopeVolumeParam.t4;
    objCloud["num-grains"] = (int)cloudToSave->getNumGrains();
    objCloud["active-state"] = cloudToSave->getActiveState();
    objCloud["x-rand-extent"] = cloudVisToSave->getXRandExtent();
    objCloud["y-rand-extent"] = cloudVisToSave->getYRandExtent();


    //trajectory
    objCloud["has-trajectory"] = cloudVisToSave->hasTrajectory();
    if(cloudVisToSave->hasTrajectory()){
        objCloud["trajectory-type"] = cloudVisToSave->getTrajectory()->getType();
        int type=cloudVisToSave->getTrajectory()->getType();

        switch ( type )
        {
            case BOUNCING:
            {
                Circular *b = dynamic_cast<Circular*>(cloudVisToSave->getTrajectory());
                objCloud["radius"]=b->getRadius();
                objCloud["angle"]=b->getAngle();
            }
            break;

            case CIRCULAR:
            {
                Circular *c=dynamic_cast<Circular*>(cloudVisToSave->getTrajectory());
                objCloud["radius"] = c->getRadius();
                objCloud["angle"] = c->getAngle();
                objCloud["stretch"] = c->getStretch();
                objCloud["progress"] = c->getProgress();
                objCloud["centerX"] = c->getCenterX();
                objCloud["centerY"] = c->getCenterY();
            }
            break;

        case HYPOTROCHOID:
        {
            Hypotrochoid *c=dynamic_cast<Hypotrochoid*>(cloudVisToSave->getTrajectory());
            objCloud["radius"] = c->getRadius();
            objCloud["radiusInt"] = c->getRadiusInt();
            objCloud["angle"] = c->getAngle();
            objCloud["expansion"] = c->getExpansion();
            objCloud["progress"] = c->getProgress();
            objCloud["centerX"] = c->getCenterX();
            objCloud["centerY"] = c->getCenterY();
        }
        break;
        case RECORDED:
        {
            Recorded *c=dynamic_cast<Recorded*>(cloudVisToSave->getTrajectory());
            objCloud["positions-number"] = c->lastPosition();
            QJsonArray docPositions;
            for (int j = 1; j < c->lastPosition(); j++){
                QJsonObject objPosition;
                objPosition["num"] = j;
                objPosition["x"] = c->getPosition(j).x;
                objPosition["y"] = c->getPosition(j).y;
                objPosition["delay"] = c->getPosition(j).delay;
                docPositions.append(objPosition);
            }
            objCloud["positions"] = docPositions;
        }
        break;
        default:
        break;

        }

        objCloud["xTrajectoryOrigin"] = cloudVisToSave->getTrajectory()->getOrigin().x;
        objCloud["yTrajectoryOrigin"] = cloudVisToSave->getTrajectory()->getOrigin().y;
        objCloud["phase"] = cloudVisToSave->getTrajectory()->getPhase();
        objCloud["move"] = cloudVisToSave->getIsMoving();
        objCloud["speed"] = cloudVisToSave->getTrajectory()->getSpeed();
    }

    docRoot["cloud"] = objCloud;

    QJsonDocument document;
    document.setObject(docRoot);
    cloudFile.write(document.toJson());
    if (!cloudFile.flush())
        return false;

    return true;
}

bool Scene::saveTriggerRecordedTrajectory(QFile &trajectoryFile, SceneTrigger *selectedTriggerSave)
{
    QString trajectoryFileName = trajectoryFile.fileName();
    QDir trajectoryFileDir = QFileInfo(trajectoryFileName).dir();

    if (!trajectoryFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    TriggerVis *triggerVisToSave = selectedTriggerSave->view.get();

    QJsonObject docRoot;

    QJsonObject objTrajectory;

    Recorded *c=dynamic_cast<Recorded*>(triggerVisToSave->getTrajectory());
    objTrajectory["number-positions"] = c->lastPosition();
    std::cout<<"number positions saved "<<c->getCenterX()<<std::endl;
    QJsonArray docPositions;
    for (int j = 1; j < c->lastPosition(); j++){
        QJsonObject objPosition;
        objPosition["num"] = j;
        objPosition["x"] = c->getPosition(j).x;
        objPosition["y"] = c->getPosition(j).y;
        objPosition["delay"] = c->getPosition(j).delay;
        std::cout<<"num "<<j<<std::endl;
        std::cout<<"x "<<c->getPosition(j).x<<std::endl;
        std::cout<<"y "<<c->getPosition(j).y<<std::endl;
        std::cout<<"delay "<<c->getPosition(j).delay<<std::endl;
        docPositions.append(objPosition);
    }
    objTrajectory["positions"] = docPositions;
    docRoot["trajectory"] = objTrajectory;

    QJsonDocument document;
    document.setObject(docRoot);
    trajectoryFile.write(document.toJson());
    if (!trajectoryFile.flush())
        return false;

    return true;
}

bool Scene::loadTriggerRecordedTrajectory(QFile &trajectoryFile, SceneTrigger *selectedTriggerLoad)
{
    QString trajectoryFileName = trajectoryFile.fileName();
    QDir trajectoryFileDir = QFileInfo(trajectoryFileName).dir();

    if (!trajectoryFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(trajectoryFile.readAll(), &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    QJsonObject docRoot = doc.object();

    QJsonObject objTrajectory = docRoot["trajectory"].toObject();
    int nbPos = objTrajectory["number-positions"].toInt();

    TriggerVis *triggerVisToLoad = selectedTriggerLoad->view.get();

    Recorded *tr_load=dynamic_cast<Recorded*>(triggerVisToLoad->getTrajectory());

    QJsonArray docPositions = objTrajectory["positions"].toArray();
    for (const QJsonValue &jsonElementPosition : docPositions) { // positions
        QJsonObject objPosition = jsonElementPosition.toObject();
        int l_x;
        int l_y;
        double l_delay;
        l_x = objPosition["x"].toInt();
        l_y = objPosition["y"].toInt();
        l_delay = objPosition["delay"].toDouble();
        triggerVisToLoad->trajectoryAddPositionDelayed(l_x, l_y, l_delay);
    }
    tr_load->setRecording(false);
    selectedTriggerLoad->view.get()->startTrajectory();

    return true;

}

bool Scene::saveCloudRecordedTrajectory(QFile &trajectoryFile, SceneCloud *selectedCloudSave)
{
    QString trajectoryFileName = trajectoryFile.fileName();
    QDir trajectoryFileDir = QFileInfo(trajectoryFileName).dir();

    if (!trajectoryFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    CloudVis *cloudVisToSave = selectedCloudSave->view.get();

    QJsonObject docRoot;

    QJsonObject objTrajectory;

    Recorded *c=dynamic_cast<Recorded*>(cloudVisToSave->getTrajectory());
    objTrajectory["number-positions"] = c->lastPosition();
    std::cout<<"number positions saved "<<c->getCenterX()<<std::endl;
    QJsonArray docPositions;
    for (int j = 1; j < c->lastPosition(); j++){
        QJsonObject objPosition;
        objPosition["num"] = j;
        objPosition["x"] = c->getPosition(j).x;
        objPosition["y"] = c->getPosition(j).y;
        objPosition["delay"] = c->getPosition(j).delay;
        std::cout<<"num "<<j<<std::endl;
        std::cout<<"x "<<c->getPosition(j).x<<std::endl;
        std::cout<<"y "<<c->getPosition(j).y<<std::endl;
        std::cout<<"delay "<<c->getPosition(j).delay<<std::endl;
        docPositions.append(objPosition);
    }
    objTrajectory["positions"] = docPositions;
    docRoot["trajectory"] = objTrajectory;

    QJsonDocument document;
    document.setObject(docRoot);
    trajectoryFile.write(document.toJson());
    if (!trajectoryFile.flush())
        return false;

    return true;
}

bool Scene::loadCloudRecordedTrajectory(QFile &trajectoryFile, SceneCloud *selectedCloudLoad)
{
    QString trajectoryFileName = trajectoryFile.fileName();
    QDir trajectoryFileDir = QFileInfo(trajectoryFileName).dir();

    if (!trajectoryFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(trajectoryFile.readAll(), &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    QJsonObject docRoot = doc.object();

    QJsonObject objTrajectory = docRoot["trajectory"].toObject();
    int nbPos = objTrajectory["number-positions"].toInt();

    CloudVis *cloudVisToLoad = selectedCloudLoad->view.get();

    Recorded *tr_load=dynamic_cast<Recorded*>(cloudVisToLoad->getTrajectory());

    QJsonArray docPositions = objTrajectory["positions"].toArray();
    for (const QJsonValue &jsonElementPosition : docPositions) { // positions
        QJsonObject objPosition = jsonElementPosition.toObject();
        int l_x;
        int l_y;
        double l_delay;
        l_x = objPosition["x"].toInt();
        l_y = objPosition["y"].toInt();
        l_delay = objPosition["delay"].toDouble();
        cloudVisToLoad->trajectoryAddPositionDelayed(l_x, l_y, l_delay);
    }
    tr_load->setRecording(false);
    selectedCloudLoad->view.get()->startTrajectory();

    return true;
}

bool Scene::loadSampleSet(bool interactive)
{
    // a new sample collection
    std::unique_ptr<SampleSet> samples(new SampleSet);
    // total count of samples needed
    unsigned numSamples = m_samples.size();
    // count of samples left to load
    unsigned numSamplesLeft = numSamples;
    // list of samples associated with scene
    std::vector<Sample *> sampleFileAssoc(numSamples);

    // the scene's current audio paths
    std::vector<std::string> audioPaths = m_audioPaths;
    // a list of additional candidate paths, added interactively by user
    std::vector<std::string> candidateAudioPaths;
    // a temporary list of search paths to try in order
    std::vector<const std::string *> effectiveAudioPaths;

    while (numSamplesLeft > 0) {
        // attempt to load many missing samples as possible
        for (size_t i = 0; i < numSamples; ++i) {
            if (sampleFileAssoc[i])
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
            Sample *af = nullptr;
            QString qname = QString::fromStdString(m_samples[i]->name);
            for (unsigned i = 0, n = effectiveAudioPaths.size(); !af && i < n; ++i) {
                QDir dir(QString::fromStdString(*effectiveAudioPaths[i]));
                QString curpath = dir.filePath(qname);
                af = samples->loadFile(curpath.toStdString());

                // successful load from a candidate path?
                if (af && i < candidateAudioPaths.size()) {
                    // move the candidate path into the recognized audio path (front)
                    audioPaths.insert(audioPaths.begin(), *effectiveAudioPaths[i]);
                    candidateAudioPaths.erase(candidateAudioPaths.begin() + i);
                }
            }

            if (af) {
                sampleFileAssoc[i] = af;
                --numSamplesLeft;
            }
        }

        if (numSamplesLeft > 0) {  // failure to load some samples
            if (!interactive)
                return false;

            // ask the user to fix
            unsigned indexMissing = 0;
            while (sampleFileAssoc[indexMissing]) ++indexMissing;

            QString qnameMissing = QString::fromStdString(m_samples[indexMissing]->name);

            QMessageBox mbox(
                QMessageBox::Warning, QObject::tr("Load samples"),
                QObject::tr("Could not find the sample file \"%1\".").arg(qnameMissing));

            mbox.addButton(QObject::tr("Add a different sample directory"), QMessageBox::AcceptRole);
            mbox.addButton(QObject::tr("Discard missing samples"), QMessageBox::DestructiveRole);
            mbox.addButton(QMessageBox::Cancel);
            mbox.exec();

            QAbstractButton *clicked = mbox.clickedButton();
            QMessageBox::ButtonRole role = clicked ? mbox.buttonRole(clicked) : QMessageBox::RejectRole;

            if (role == QMessageBox::DestructiveRole) {
                // ignore missing samples and proceed
                break;
            }
            else if (role == QMessageBox::AcceptRole) {
                // ask for a directory to add to search paths
                QString candidateDir = QFileDialog::getExistingDirectory(
                    nullptr, QObject::tr("Add sample directory"),
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

    // load all samples into model
    m_sampleSet = std::move(samples);
    for (unsigned i = 0, j = 0, n = sampleFileAssoc.size(); i < n; ++i) {
        Sample *assoc = sampleFileAssoc[i];
        if (!assoc) {
            // could not load sample for element? remove
            m_samples.erase(m_samples.begin() + j);
        }
        else {
            // associate sample with its element
            SceneSample &sceneSample = *m_samples[j++];
            sceneSample.sample = assoc;
            sceneSample.view->associateSample(assoc->wave, assoc->frames, assoc->channels, assoc->name);
        }
    }

    for (unsigned i = 0, n = m_clouds.size(); i < n; ++i) {
        Cloud &cloudToUpdate = *m_clouds[i]->cloud;
        cloudToUpdate.updateSampleSet();
    }

    // update the audio path, adding user's candidates which worked
    m_audioPaths = audioPaths;
    return true;
}
// init default cloud params
void Scene::initDefaultCloudParams()
{
    g_defaultCloudParams.duration = 500.0;
    g_defaultCloudParams.overlap = 1.0f;
    g_defaultCloudParams.pitch = 0.0f;
    g_defaultCloudParams.pitchLFOFreq = 0.00f;
    g_defaultCloudParams.pitchLFOAmount = 0.00f;
    g_defaultCloudParams.windowType = HANNING;
    g_defaultCloudParams.spatialMode = UNITY;
    g_defaultCloudParams.outputFirst = 0;
    g_defaultCloudParams.outputLast = theOutChannelCount - 1;
    g_defaultCloudParams.channelLocation = -1;
    g_defaultCloudParams.numGrains = 8;
    g_defaultCloudParams.activateState = true;
    g_defaultCloudParams.activateRestartTrajectory = true;
    g_defaultCloudParams.midiChannel = 0;
    g_defaultCloudParams.midiNote = 69;
    g_defaultCloudParams.xRandExtent = 3.0f;
    g_defaultCloudParams.yRandExtent = 3.0f;
    g_defaultCloudParams.envelope.setTimeBasedParameters(
        // L1, L2, L3
        1.0, 0.9, 0.8,
        // TAtk, TSta, TDec, TRel
        1.0, 0.2, 0.5, 3.0,
        samp_rate);
    g_defaultCloudParams.envelope.t1 = ParamEnv::Linear;
    g_defaultCloudParams.envelope.t2 = ParamEnv::Linear;
    g_defaultCloudParams.envelope.t3 = ParamEnv::Linear;
    g_defaultCloudParams.envelope.t4 = ParamEnv::Linear;
    g_defaultCloudParams.hasTrajectory = false;
    g_defaultCloudParams.speed = 0.2;
    g_defaultCloudParams.radius = 100;
    g_defaultCloudParams.stretch = 0.5;
    g_defaultCloudParams.angle = 45;
    g_defaultCloudParams.radiusInt = 20;
    g_defaultCloudParams.expansion = 50;
    g_defaultCloudParams.progress = 150;
    g_defaultCloudParams.trajectoryType = STATIC;
    g_defaultCloudParams.triggerZoneExtent = 0;
    g_defaultCloudParams.triggerIn = ON;
    g_defaultCloudParams.triggerOut = OFF;
}

Sample *Scene::loadNewSample(const std::string &path)
{
    Sample *af = m_sampleSet->loadFile(path);
    if (!af) {
        // cannot load
        return nullptr;
    }
    return af;
}

Sample *Scene::loadNewInput(const int l_input)
{
    Sample *af = m_sampleSet->loadInput(l_input);
    if (!af) {
        // cannot load
        return nullptr;
    }
    return af;
}

bool Scene::removeSampleAt(unsigned index)
{
    if (index >= m_samples.size())
        return false;

    Sample *af = m_samples[index]->sample;
    m_samples.erase(m_samples.begin() + index);

    for (unsigned i = 0, n = m_clouds.size(); i < n; ++i) {
        Cloud &cloudToUpdate = *m_clouds[i]->cloud;
        cloudToUpdate.updateSampleSet();
    }

    removeSampleIfNotUsed(af);
    return true;
}

void Scene::removeSampleIfNotUsed(Sample *sampleToRemove)
{
    bool used = false;
    for (unsigned i = 0, n = m_samples.size(); !used && i < n; ++i)
        used = m_samples[i]->sample == sampleToRemove;
    if (!used) {
        std::cout << "Sample is not used, delete\n";
        m_sampleSet->removeSample(sampleToRemove);
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

void Scene::addSampleVis(Sample *sampleToAdd)
{
    SceneSample *sceneSample = new SceneSample;
    m_samples.emplace_back(sceneSample);
    sceneSample->name = sampleToAdd->name;
    sceneSample->sample = sampleToAdd;

    SampleVis *sv = new SampleVis;
    sceneSample->view.reset(sv);
    sv->associateSample(sampleToAdd->wave, sampleToAdd->frames, sampleToAdd->channels, sampleToAdd->name);
    sampleToAdd->registerSampleVis(sv);

    for (unsigned i = 0, n = m_clouds.size(); i < n; ++i) {
        Cloud &cloudToUpdate = *m_clouds[i]->cloud;
        cloudToUpdate.updateSampleSet();
    }
}

void Scene::addNewCloud(int numGrains)
{
    // create audio
    SceneCloud *sceneCloud = new SceneCloud;
    m_clouds.emplace_back(sceneCloud);
    sceneCloud->cloud.reset(new Cloud(&m_samples, numGrains));
    // create visualization
    sceneCloud->view.reset(new CloudVis(mouseX, mouseY, numGrains, &m_samples,false));
    // select new cloud
    sceneCloud->view->setSelectState(true);
    // register visualization with audio
    sceneCloud->cloud->registerCloudVis(sceneCloud->view.get());
    sceneCloud->view->registerCloud(sceneCloud->cloud.get());
    std::ostream &out = std::cout;
    sceneCloud->cloud->describe(out);
}

void Scene::addNewTrigger()
{
    //cout << "scene addnewtrigger" << endl;
    SceneTrigger *sceneTrigger = new SceneTrigger;
    m_triggers.emplace_back(sceneTrigger);
    sceneTrigger->trigger.reset(new Trigger());
    // create visualization
    sceneTrigger->view.reset(new TriggerVis(mouseX, mouseY));
    // select new trigger
    sceneTrigger->view->setSelectState(true);
    // register visualization
    sceneTrigger->trigger->registerTriggerVis(sceneTrigger->view.get());
    sceneTrigger->view->registerTrigger(sceneTrigger->trigger.get());
    std::ostream &out = std::cout;
    sceneTrigger->trigger->describe(out);
}



SceneSample *Scene::selectedSample()
{
    if ((unsigned)m_selectedSample >= m_samples.size())
        return nullptr;
    return m_samples[(unsigned)m_selectedSample].get();
}

SceneCloud *Scene::selectedCloud()
{
    if ((unsigned)m_selectedCloud >= m_clouds.size())
        return nullptr;
    return m_clouds[(unsigned)m_selectedCloud].get();
}

SceneTrigger *Scene::selectedTrigger()
{
    if ((unsigned)m_selectedTrigger >= m_triggers.size())
        return nullptr;
    return m_triggers[(unsigned)m_selectedTrigger].get();
}

SceneTrigger *Scene::findTriggerById(unsigned id)
{
    for (size_t i = 0, n = m_triggers.size(); i < n; i++) {
        SceneTrigger &trigger = *m_triggers[i];
        if (trigger.trigger->getId() == id)
            return &trigger;
    }
    return nullptr;
}

SceneCloud *Scene::findCloudById(unsigned id)
{
    for (size_t i = 0, n = m_clouds.size(); i < n; i++) {
        SceneCloud &cloud = *m_clouds[i];
        if (cloud.cloud->getId() == id)
            return &cloud;
    }
    return nullptr;
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
        if (SceneSample *sceneSample = selectedSample()) {
            // cout << "deselecting rect" << endl;
            sceneSample->view->setSelectState(false);
            m_selectedSample = -1;
        }
        break;
    case TRIGG:
        if (SceneTrigger *trigger = selectedTrigger()) {
            trigger->view->setSelectState(false);
            // reset selected trigger
            m_selectedTrigger = -1;
        }
        break;
    }
}

void Scene::midiNoteOn(int midiChannelToPlay, int midiNoteToPlay, int midiVeloToPlay)
{
    //std::cout << "midi in note on, channel = "<< midiChannelToPlay << ", note = "<< midiNoteToPlay<< ", velo =" << midiVeloToPlay << std::endl;
    int l_numCombi = m_midiInstrument.getMidiCombi(midiChannelToPlay + 1);
    if (l_numCombi != -1){
        Note l_note = m_midiBank.findCombi(l_numCombi).getNote(midiNoteToPlay);
        for (int i = 0; i < l_note.cloudLayer.size(); i++){
            if ((midiVeloToPlay >= l_note.cloudLayer[i].velocity.min) && (midiVeloToPlay <= l_note.cloudLayer[i].velocity.max)){
                findCloudById(l_note.cloudLayer[i].cloudId)->cloud.get()->setActiveMidiState(true, midiNoteToPlay, midiVeloToPlay);
            }
        }
    }
}

void Scene::midiNoteOff(int midiChannelToStop, int midiNoteToStop)
{
//    std::cout << "midi in note off, channel = "<< midiChannelToStop << ", note = "<< midiNoteToStop<<  std::endl;
    int l_numCombi = m_midiInstrument.getMidiCombi(midiChannelToStop + 1);
//    std::cout << "l_numCombi=" << l_numCombi << std::endl;
    if (l_numCombi != -1){
        Note l_note = m_midiBank.findCombi(l_numCombi).getNote(midiNoteToStop);
        for (int i = 0; i < l_note.cloudLayer.size(); i++){
            findCloudById(l_note.cloudLayer[i].cloudId)->cloud.get()->setActiveMidiState(false, midiNoteToStop, 127);
        }
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

int Scene::getNumTrigger(SceneTrigger *triggerCurrent)
{
    for (int i = 0, n = m_triggers.size(); i < n; i++) {
        Trigger *triggerForNum = triggerCurrent->trigger.get();
        Trigger *triggerToExplore = m_triggers[i]->trigger.get();
        if (triggerForNum->getId() == triggerToExplore->getId())
            return i;
    }
    return -1;
}

void Scene::changeParamEnvelopeVolume(SceneCloud *selectedCloud)
{
    ParamEnv localParamEnv;
    Cloud *cloudToChangePEV = selectedCloud->cloud.get();
    localParamEnv = cloudToChangePEV->getEnvelopeVolumeParam();
    cloudToChangePEV->getEnvelopeVolume().envDialogShow(localParamEnv);
    cloudToChangePEV->setEnvelopeVolumeParam(localParamEnv);
}


// destructor
SceneSample::~SceneSample()
{
}

// destructor
SceneCloud::~SceneCloud()
{
}

// destructor
SceneTrigger::~SceneTrigger()
{
}
