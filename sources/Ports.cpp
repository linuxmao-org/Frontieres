//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
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

#include "Ports.h"
#include "model/Scene.h"
#include "model/Cloud.h"
#include "visual/CloudVis.h"
#include <rtosc/port-sugar.h>

namespace Ports {

static void dispatch_cloud(
    const char *msg, rtosc::RtData &data, rtosc::Ports &ports)
{
    // remove "cloud/" part of message
    SNIP;

    // read <id-cloud/> and remove it
    unsigned cloudId;
    if (sscanf(msg, "%u/", &cloudId) != 1)
        return;
    SNIP;

    // find cloud by its ID
    Scene *scene = (Scene *)data.obj;
    SceneCloud *cloud = scene->findCloudById(cloudId);
    if (!cloud)
        return;

    // dispatch to the "cloud/" sub-root
    data.obj = cloud;
    ports.dispatch(msg, data);
}

rtosc::Ports rtRoot {
    {"cloud/", rDoc("Cloud"), &Ports::rtCloud,
     [](const char *msg, rtosc::RtData &data) {
         dispatch_cloud(msg, data, Ports::rtCloud);
     }},
};

rtosc::Ports nonRtRoot {
    {"cloud/", rDoc("Cloud"), &Ports::rtCloud,
     [](const char *msg, rtosc::RtData &data) {
         dispatch_cloud(msg, data, Ports::nonRtCloud);
     }},
};

rtosc::Ports rtCloud {
    {"volume:f", rDoc("Volume"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setVolumeDb(arg);
     }},
    {"pitchLFOFreq:f", rDoc("PitchLFOFreq"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setPitchLFOFreq(arg);
     }},
    {"pitchLFOAmount:f", rDoc("PitchLFOAmount"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setPitchLFOAmount(arg);
     }},
    {"pitch:f", rDoc("Pitch"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setPitch(arg);
     }},
    {"overlap:f", rDoc("Overlap"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setOverlap(arg);
     }},
    {"duration:f", rDoc("Duration"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setDurationMs(arg);
     }},
    {"numGrains:f", rDoc("NumGrains"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setNumGrains(arg);
     }},
    {"direction:f", rDoc("Direction"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setDirection(arg);
     }},
    {"windowType:f", rDoc("WindowType"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setWindowType(arg);
     }},
    {"spatialMode:f", rDoc("SpatialMode"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setSpatialMode(arg, -1);
     }},
    {"midiChannel:f", rDoc("MidiChannel"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setMidiChannel(arg);
     }},
    {"midiNote:f", rDoc("MidiNote"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setMidiNote(arg);
     }},
    {"active:f", rDoc("Active"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setActiveState(arg);
     }},
    {"locked:f", rDoc("Locked"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->cloud->setLockedState(arg);
     }},
    {"controlh:f", rDoc("Controlh"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         sceneCloud->cloud->setCtrlX(rtosc_argument(msg, 1).f);
         sceneCloud->cloud->setCtrlY(rtosc_argument(msg, 0).f);
         sceneCloud->cloud->setCtrlAutoUpdate(true);
     }},
    {"controlv:f", rDoc("Controlv"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         sceneCloud->cloud->setCtrlX(rtosc_argument(msg, 0).f);
         sceneCloud->cloud->setCtrlY(rtosc_argument(msg, 1).f);
         sceneCloud->cloud->setCtrlAutoUpdate(true);
     }},        
};

rtosc::Ports nonRtCloud {
    {"x:f", rDoc("X"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->view->setX(arg);
     }},
    {"y:f", rDoc("Y"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->view->setY(arg);
     }},
    {"xy:f", rDoc("xy"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float argx = rtosc_argument(msg, 0).f;
         float argy = rtosc_argument(msg, 1).f;
         sceneCloud->view->setX(argx);
         sceneCloud->view->setY(argy);
     }},
    {"xExtent:f", rDoc("XExtent"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->view->setFixedXRandExtent(arg);
     }},
    {"yExtent:f", rDoc("YExtent"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float arg = rtosc_argument(msg, 0).f;
         sceneCloud->view->setFixedYRandExtent(arg);
     }},
    {"xyExtent:f", rDoc("xyExtent"), nullptr,
     [](const char *msg, rtosc::RtData &data) {
         SceneCloud *sceneCloud = (SceneCloud *)data.obj;
         float argx = rtosc_argument(msg, 0).f;
         float argy = rtosc_argument(msg, 1).f;
         sceneCloud->view->setFixedXRandExtent(argx);
         sceneCloud->view->setFixedYRandExtent(argy);
     }},
};

}  // namespace Ports
