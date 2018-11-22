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

rtosc::Ports root {
    {"cloud/", rDoc("Cloud"), &Ports::cloud,
     [](const char *msg, rtosc::RtData &data) {
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
         Ports::cloud.dispatch(msg, data);
     }},
};

rtosc::Ports cloud {
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
};

}  // namespace Ports
