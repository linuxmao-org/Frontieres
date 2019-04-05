//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2011  Christopher Carlson
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


//
//  main.cpp
//  Frontières
//
//  Created by Christopher Carlson on 11/13/11.
//


// my includes
#include "theglobals.h"
#include "Frontieres.h"

// graphics includes
#include <qopengl.h>

// other libraries
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

// audio related
#include "MyRtAudio.h"
#include "model/Sample.h"
#include "dsp/Window.h"

// midi related
#include <ring_buffer.h>

// OSC related
#include "MyRtOsc.h"
#include "Ports.h"

// graphics related
#include "visual/SampleVis.h"
#include "visual/CloudVis.h"

// audio related
#include "model/Cloud.h"
#include "model/Scene.h"
#include "model/ParamCloud.h"

// time
#include "utility/GTime.h"

// Qt application
#include "interface/MyGLApplication.h"
#include "interface/MyGLWindow.h"
#include "interface/StartDialog.h"
#include <QtFont3D.h>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

#ifdef Q_OS_UNIX
// signal handling
#include <QSocketNotifier>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#endif

using namespace std;



//-----------------------------------------------------------------------------
// Shared Data Structures, Global parameters
//-----------------------------------------------------------------------------
// audio system
AbstractAudio *theAudio = NULL;
// buffer of midi input messages
Ring_Buffer *theMidiInBuffer = NULL;
// buffer of OSC input messages
Ring_Buffer *theOscInBuffer = NULL;
// library path
string g_audioPathUser;
string g_audioPathSystem;
// parameter string
string paramString = "";
// desired audio buffer size
unsigned int g_buffSize = 1024;
// current scene
Scene *currentScene = nullptr;
// current scene mutex
std::mutex currentSceneMutex;

// number of output channels
int theChannelCount = 16;
// sample rate - Hz
unsigned int samp_rate = 0;

// global time increment - samples per second
// global time is incremented in audio callback
double samp_time_sec = 0;


// Initial camera movement vars
// my position
pt3d position(0.0, 0.0, 0.0f);


// ENUMS
// default selection mode
int selectionMode = CLOUD;
int dragMode = MOVE;
bool resizeDir = false;  // for rects
// rubber band select params
int rb_anchor_x = -1;
int rb_anchor_y = -1;

// selection helper vars
bool menuFlag = true;

// sample name display
bool showSampleNames = false;

// flag indicating parameter change
bool paramChanged = false;
unsigned int currentParam = NUMGRAINS;
double lastParamChangeTime = 0.0;
double tempParamVal = -1.0;


// mouse coordinate initialization
int mouseX = -1;
int mouseY = -1;
long lastDragX = veryHighNumber;
long lastDragY = veryHighNumber;

// text renderer
QtFont3D *text_renderer = NULL;

ValueMin g_cloudValueMin;
ValueMax g_cloudValueMax;
CloudParams g_defaultCloudParams;

float dspMonitorValue = 0;

//--------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------

void updateMouseCoords(int x, int y);
void draw_string(GLfloat x, GLfloat y, GLfloat z, const QString &str, GLfloat scale);
void drawAxis();
void audioCallback(BUFFERPREC *out, unsigned int numFrames, void *userData);
void processMidiMessage(const unsigned char *message, unsigned length);
void processOscMessage(const char *message, size_t length, rtosc::Ports &ports);
void midiInCallback(const unsigned char *message, unsigned size, void *userData);
void oscCallback(const char *msg, size_t length, void *);

//--------------------------------------------------------------------------------
// Cleanup code
//--------------------------------------------------------------------------------

void cleaningFunction()
{
    if (theAudio != NULL) {
        delete theAudio;
        theAudio = NULL;
    }
    if (text_renderer != NULL) {
        delete text_renderer;
        text_renderer = NULL;
    }
}


//================================================================================
//   Audio Callback
//================================================================================

// audio callback
void audioCallback(BUFFERPREC *out, unsigned int numFrames, void *)
{
    // process the midi messages
    unsigned char midiMessageSize;
    unsigned char midiMessageBuffer[256];
    while (theMidiInBuffer->peek(midiMessageSize) &&
           theMidiInBuffer->size_used() >= 1 + midiMessageSize)  // must check the message is available in full
    {
        theMidiInBuffer->discard(1);  // the header that was peeked
        theMidiInBuffer->get(midiMessageBuffer, midiMessageSize);
        processMidiMessage(midiMessageBuffer, midiMessageSize);
    }

    // process the OSC messages
    size_t oscMessageSize;
    char oscMessageBuffer[1024];
    while (theOscInBuffer->peek(oscMessageSize) &&
           theOscInBuffer->size_used() >= sizeof(oscMessageSize) + oscMessageSize)  // must check the message is available in full
    {
        theOscInBuffer->discard(sizeof(oscMessageSize));  // the header that was peeked
        if (oscMessageSize > sizeof(oscMessageBuffer))
            theOscInBuffer->discard(oscMessageSize);
        else {
            theOscInBuffer->get(oscMessageBuffer, oscMessageSize);
            processOscMessage(oscMessageBuffer, oscMessageSize, Ports::rtRoot);
        }
    }

    // cast audio buffers
    memset(out, 0, sizeof(BUFFERPREC) * numFrames * theChannelCount);
    if (menuFlag == false) {
        std::unique_lock<std::mutex> lock(::currentSceneMutex, std::try_to_lock);
        if (lock.owns_lock()) {
            auto tp1 = std::chrono::steady_clock::now(); // record time before computation

            // generate audio by mixing all the clouds
            Scene *scene = ::currentScene;
            for (int i = 0, n = scene->m_clouds.size(); i < n; i++) {
                Cloud &theCloud = *scene->m_clouds[i]->cloud;
                theCloud.nextBuffer(out, numFrames);
            }

            auto tp2 = std::chrono::steady_clock::now(); // record time after computation

            // compute time difference and CPU usage
            double deltaTime = 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp1).count();
            double bufferTime = numFrames * samp_time_sec;
            dspMonitorValue = deltaTime / bufferTime;
        }
    }
    GTime::instance().sec += numFrames * samp_time_sec;
    // cout << GTime::instance().sec<<endl;
}

// midi processing routine
void processMidiMessage(const unsigned char *message, unsigned length)
{
    if (length < 2)
        return;  // valid midi length is at least 2

    unsigned status = message[0] & 0xf0; // the type of message
    unsigned channel = message[0] & 0x0f; // the channel on which it arrives

    int data1 = message[1] & 0x7f;  // data bytes
    int data2 = (length > 2) ? (message[2] & 0x7f) : 0;

    if (status == 0x90 && data2 == 0)
        status = 0x80;  // a note-on with velocity 0 means note-off

    //fprintf(stderr, "MIDI[%02u] %02X %02X %02X\n", channel + 1, status, data1, data2);

    switch (status) {
    case 0x90:  // note on, 1=key number, 2=velocity
        /* do something */
        theApplication->midiNoteOn(channel, data1, data2);
        break;

    case 0x80:  // note off, 1=key number, 2=velocity
        /* do something */
        theApplication->midiNoteOff(channel, data1);
        break;
    case 0xb0:  // control change, 1=control number, 2=value
        switch (data1) {
            /* do something */
        }
        break;

    case 0xc0:  // program change, 1=program number
        /* do something */
        break;

    case 0xe0:  // pitch bend, (2,1)=bend value (14 bit)
    {
        int bend = ((data2 << 7) | data1) - 8192;
        /* do something */
        break;
    }
    }
}

// OSC processing routing
void processOscMessage(const char *message, size_t length, rtosc::Ports &ports)
{
    rtosc::RtData d;
    d.obj = ::currentScene;

    if (!d.obj)
        return;

    ports.dispatch(message, d, true);
}

//================================================================================
//   Midi Input Callback
//================================================================================

void midiInCallback(const unsigned char *message, unsigned size, void *)
{
    if (size >= 256)
        return;  // drop large messages

    // send the midi into a buffer the audio thread will process from

    if (theMidiInBuffer->size_free() < 1 + size)
        return;  // check if the buffer can take the message, if not drop

    // write the message header, a 8bit size field
    theMidiInBuffer->put((unsigned char)size);
    // write the message body
    theMidiInBuffer->put(message, size);
}

//================================================================================
//   OSC Input Callback
//================================================================================
void oscCallback(const char *msg, size_t length, void *)
{
    Ring_Buffer &buffer = *theOscInBuffer;
    size_t need = sizeof(length) + length;

    if (need > buffer.capacity())
        return;  // message too large

    // wait for free space if full buffer
    while (buffer.size_free() < need)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // write the message header, a size field
    buffer.put(length);
    // write the message body
    buffer.put(msg, length);

    // also send message to UI
    Q_EMIT theApplication->oscMessageArrived(QByteArray(msg, length));
}

//================================================================================
//   GRAPHICS/QT OPENGL
//================================================================================


///-----------------------------------------------------------------------------
// name: drawAxis()
// desc: draw 3d axis
//-----------------------------------------------------------------------------
void drawAxis()
{
    // get screen width and height
    MyGLScreen *screen = theApplication->GLwindow()->screen();
    int screenWidth = screen->width();
    int screenHeight = screen->height();

    // PUSH -- //store state
    glPushMatrix();

    // specify vertices with this drawing mode
    glBegin(GL_LINES);
    glLineWidth(0.9f);
    // x axis
    glColor4f(1, 0, 0, 0.9);
    glVertex3f(0, 0, 0);
    glVertex3f(screenWidth, 0, 0);

    // x axis
    glColor4f(0, 1, 0, 0.9);
    glVertex3f(0, 0, 0);
    glVertex3f(0, screenHeight, 0);

    // z axis
    glColor4f(0, 0, 1, 0.7);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 400);

    // stop drawing
    glEnd();

    // POP -- //restore state
    glPopMatrix();
}


//-----------------------------------------------------------------------------
// Display simple string
// desc: from sndpeek source - Ge Wang, et al
//-----------------------------------------------------------------------------
void draw_string(float x, float y, float z, const QString &str, float scale)
{
    // get screen width and height
    MyGLScreen *screen = theApplication->GLwindow()->screen();
    int screenHeight = screen->height();

    text_renderer->print(
        str, QVector3D(x, screenHeight - y, z), Qt::AlignLeft,
        scale * (1.0 / text_fontScaleDown));
}


//-----------------------------------------------------------------------------
// Show usage on screen.  TODO:  add usage info
//-----------------------------------------------------------------------------
void printUsage()
{
    // get screen width and height
    MyGLScreen *screen = theApplication->GLwindow()->screen();
    int screenWidth = screen->width();
    int screenHeight = screen->height();

    glLineWidth(2.0f);
    float theA = 0.6f + 0.2 * sin(0.8 * PI * GTime::instance().sec);
    glColor4f(theA, theA, theA, theA);
    draw_string(screenWidth * 0.5f, screenHeight * 0.5f,
                0.5f, u8"FRONTIÈRES", 250.0f);

    theA = 0.6f + 0.2 * sin(0.9 * PI * GTime::instance().sec);
    float insColor = theA * 0.4f;
    glColor4f(insColor, insColor, insColor, theA);
    // key info
    draw_string(screenWidth * 0.5f + 10.0,
                screenHeight * 0.5f + 40.0, 0.5f, QObject::tr("CLICK TO START"),
                125.0f);

    theA = 0.6f + 0.2 * sin(1.0 * PI * GTime::instance().sec);
    insColor = theA * 0.4f;
    glColor4f(insColor, insColor, insColor, theA);
    // key info
    draw_string(screenWidth * 0.5f + 10.0,
                screenHeight * 0.5f + 70.0, 0.5f, QObject::tr("ESCAPE TO QUIT"),
                125.0f);
}


void printParam()
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();
    SceneSample *selectedSample = scene->selectedSample();

    // get screen width and height
    MyGLScreen *screen = theApplication->GLwindow()->screen();
    int screenHeight = screen->height();

    if (selectedCloud) {
        CloudVis &theCloudVis = *selectedCloud->view;
        Cloud &theCloud = *selectedCloud->cloud;

        // float cloudX = theCloudVis.getX();
        // float cloudY = theCloudVis.getY();
        string myValue;
        ostringstream sinput;
        ostringstream sinput2;
        float theA = 0.7f + 0.3 * sin(1.6 * PI * GTime::instance().sec);
        glColor4f(1.0f, 1.0f, 1.0f, theA);

        switch (currentParam) {
        case NUMGRAINS:
            myValue = QObject::tr("Grains: ").toStdString();
            sinput << theCloud.getNumGrains();
            myValue = myValue + sinput.str();
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case DURATION:
            myValue = QObject::tr("Duration: ").toStdString();
            if (paramString == "") {
                sinput << theCloud.getDurationMs();
                myValue = myValue + sinput.str() + " ms";
            }
            else {
                myValue = myValue + paramString + " ms";
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            //            myValue = "Duration (ms): " + theCloud->getDurationMs();
            break;
        case WINDOW:
            switch (theCloud.getWindowType()) {
            case HANNING:
                myValue = QObject::tr("Window: HANNING").toStdString();
                break;
            case TRIANGLE:
                myValue = QObject::tr("Window: TRIANGLE").toStdString();
                break;
            case REXPDEC:
                myValue = QObject::tr("Window: REXPDEC").toStdString();
                break;
            case EXPDEC:
                myValue = QObject::tr("Window: EXPDEC").toStdString();
                break;
            case SINC:
                myValue = QObject::tr("Window: SINC").toStdString();
                break;
            case RANDOM_WIN:
                myValue = QObject::tr("Window: RANDOM_WIN").toStdString();
                break;
            default:
                myValue = "";
                break;
            }

            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case MOTIONX:
            myValue = QObject::tr("X: ").toStdString();
            sinput << theCloudVis.getXRandExtent();
            myValue = myValue + sinput.str();
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case MOTIONY:
            myValue = QObject::tr("Y: ").toStdString();
            sinput << theCloudVis.getYRandExtent();
            myValue = myValue + sinput.str();
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case MOTIONXY:
            myValue = QObject::tr("X,Y: ").toStdString();
            sinput << theCloudVis.getXRandExtent();
            myValue = myValue + sinput.str() + ", ";
            sinput2 << theCloudVis.getYRandExtent();
            myValue = myValue + sinput2.str();
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;

        case DIRECTION:
            switch (theCloud.getDirection()) {
            case FORWARD:
                myValue = QObject::tr("Direction: FORWARD").toStdString();
                break;
            case BACKWARD:
                myValue = QObject::tr("Direction: BACKWARD").toStdString();
                break;
            case RANDOM_DIR:
                myValue = QObject::tr("Direction: RANDOM").toStdString();
                break;
            default:
                myValue = "";
                break;
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;

        case SPATIALIZE:
            switch (theCloud.getSpatialMode()) {
            case UNITY:
                myValue = QObject::tr("Spatial Mode: UNITY").toStdString();
                break;
            case STEREO:
                myValue = QObject::tr("Spatial Mode: STEREO").toStdString();
                break;
            case AROUND:
                myValue = QObject::tr("Spatial Mode: AROUND").toStdString();
                break;
            default:
                myValue = "";
                break;
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case VOLUME:
            myValue = QObject::tr("Volume (dB): ").toStdString();
            if (paramString == "") {
                sinput << theCloud.getVolumeDb();
                myValue = myValue + sinput.str();
            }
            else {
                myValue = myValue + paramString;
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case OVERLAP:
            myValue = QObject::tr("Overlap: ").toStdString();
            if (paramString == "") {
                sinput << theCloud.getOverlap();
                myValue = myValue + sinput.str();
            }
            else {
                myValue = myValue + paramString;
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            //            myValue = "Duration (ms): " + theCloud->getDurationMs();
            break;
        case PITCH:
            myValue = QObject::tr("Pitch: ").toStdString();
            if (paramString == "") {
                sinput << theCloud.getPitch();
                myValue = myValue + sinput.str();
            }
            else {
                myValue = myValue + paramString;
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            //            myValue = "Duration (ms): " + theCloud->getDurationMs();
            break;

        case P_LFO_FREQ:
            myValue = QObject::tr("Pitch LFO Freq: ").toStdString();
            if (paramString == "") {
                sinput << theCloud.getPitchLFOFreq();
                myValue = myValue + sinput.str();
            }
            else {
                myValue = myValue + paramString;
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            //            myValue = "Duration (ms): " + theCloud->getDurationMs();
            break;
        case P_LFO_AMT:
            myValue = QObject::tr("Pitch LFO Amount: ").toStdString();
            if (paramString == "") {
                sinput << theCloud.getPitchLFOAmount();
                myValue = myValue + sinput.str();
            }
            else {
                myValue = myValue + paramString;
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            //            myValue = "Duration (ms): " + theCloud->getDurationMs();
            break;
        case NUM:
            myValue = QObject::tr("Cloud ID/Num: ").toStdString()
                    + std::to_string(theCloud.getId())
                    + "/" + std::to_string(currentScene->getNumCloud(selectedCloud)+1);
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        default:
            break;
        }
    }
    if (selectedSample) {
        SampleVis &theSampleVis = *selectedSample->view;
        string myValue;
        float theA = 0.7f + 0.3 * sin(1.6 * PI * GTime::instance().sec);
        glColor4f(1.0f, 1.0f, 1.0f, theA);

        switch (currentParam) {
        case NAME:
            myValue = QObject::tr("Sample: ").toStdString() + theSampleVis.getName();
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        default:
            break;
        }
    }
}


//================================================================================
//   INTERACTION
//================================================================================


// update mouse coords based on mousemovement
void updateMouseCoords(int x, int y)
{
    // get screen width and height
    MyGLScreen *screen = theApplication->GLwindow()->screen();
    int screenHeight = screen->height();

    mouseX = x + position.x;
    mouseY = (screenHeight - (y - position.y));
}


//-----------------------------------------------------------------------------
// Handle mouse movement during button press (drag)
//-----------------------------------------------------------------------------

void mouseDrag(int x, int y)
{
    // update mouse coordinates based on drag position
    updateMouseCoords(x, y);
    int xDiff = 0;
    int yDiff = 0;

    Scene *scene = ::currentScene;
    SceneSample *selectedSample = scene->selectedSample();
    SceneCloud *selectedCloud = scene->selectedCloud();
    if (selectedCloud)
    {
        selectedCloud->view->updateCloudPosition(mouseX, mouseY);
    }
    else {

        switch (dragMode) {
        case MOVE:
            if ((lastDragX != veryHighNumber) && (lastDragY != veryHighNumber)) {

                if (selectedSample) {  // movement case
                    selectedSample->view->move(mouseX - lastDragX, mouseY - lastDragY);
                }
            }
            lastDragX = mouseX;
            lastDragY = mouseY;
            break;

        case RESIZE:
            if ((lastDragX != veryHighNumber) && (lastDragY != veryHighNumber)) {
                // cout << "drag ok" << endl;
                // for width height - use screen coords

                if (selectedSample) {
                    xDiff = x - lastDragX;
                    yDiff = y - lastDragY;
                    // get width and height
                    float newWidth = selectedSample->view->getWidth();
                    float newHeight = selectedSample->view->getHeight();

                    int thresh = 0;
                    // check motion mag
                    if (xDiff < -thresh) {
                        newWidth = newWidth * 0.8 +
                                   0.2 * (newWidth * (1.1 + abs(xDiff / 50.0)));
                    }
                    else {
                        if (xDiff > thresh)
                            newWidth = newWidth * 0.8 +
                                       0.2 * (newWidth * (0.85 - abs(xDiff / 50.0)));
                    }
                    if (yDiff > thresh) {
                        newHeight = newHeight * 0.8 +
                                    0.2 * (newHeight * (1.1 + abs(yDiff / 50.0)));
                    }
                    else {
                        if (yDiff < -thresh)
                            newHeight = newHeight * 0.8 +
                                        0.2 * (newHeight * (0.85 - abs(yDiff / 50.0)));
                    }

                    // update width and height
                    selectedSample->view->setWidthHeight(newWidth, newHeight);
                }
            }
            lastDragX = x;
            lastDragY = y;
            break;
        default:
            break;
        }
    }
}


//-----------------------------------------------------------------------------
// Handle mouse movement anytime
//-----------------------------------------------------------------------------

void mousePassiveMotion(int x, int y)
{
    updateMouseCoords(x, y);

    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    if (selectedCloud) {
        switch (currentParam) {
        case MOTIONX:
            selectedCloud->view->setXRandExtent(mouseX);
            break;
        case MOTIONY:
            selectedCloud->view->setYRandExtent(mouseY);
            break;
        case MOTIONXY:
            selectedCloud->view->setRandExtent(mouseX, mouseY);
            break;
        default:
            break;
        }
    }
    //            case NUMGRAINS:
    //                break;
    //            case DURATION:
    //                gv.setDurationMs((mouseY/screenHeight)*4000.0f);
    //
    //            default:
    //                break;
    //        }
    //    }
    // process rectangles
    //    for (int i = 0; i < soundViews.size(); i++)
    //        soundViews[i]->procMovement(x, y);
    //
}


//-----------------------------------------------------------------------------//
// MAIN
//-----------------------------------------------------------------------------//
int main(int argc, char **argv)
{
    int exitCode = 0;
    double fps = 50;

#ifdef Q_OS_UNIX
    //-------------Signal Handler-----------//
    static int signalFds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, signalFds) != 0) {
        perror("socketpair");
    }
    else {
        struct sigaction sigActionTerm = {};
        const int sigsTerm[] = {SIGINT, SIGTERM};

        // define the signal handler for term signals
        sigActionTerm.sa_handler = +[](int)
            { char a = 1; while (write(signalFds[1], &a, 1) == -1 && errno == EINTR); };
        sigemptyset(&sigActionTerm.sa_mask);
        for (int s : sigsTerm)
            sigaddset(&sigActionTerm.sa_mask, s);
        for (int s : sigsTerm) {
            if (sigaction(s, &sigActionTerm, nullptr) != 0)
                perror("sigaction");
        }

        // this thread just sleeps, its job is to be the signal catcher
        std::thread sigThread([]() { for (;;) pause(); });
        sigThread.detach();

        // make sure only the above thread will receive the signal
        if (pthread_sigmask(SIG_BLOCK, &sigActionTerm.sa_mask, nullptr) != 0)
            perror("pthread_sigmask");
    }
#endif

    bool autoconnect = false;

    // init random number generator
    srand(time(NULL));
    // start time

    MyGLApplication app(argc, argv);
    app.setApplicationName("Frontieres");
    app.setApplicationDisplayName(u8"Frontières");
    app.setApplicationVersion(APP_VERSION);

#ifdef Q_OS_UNIX
    //-------------Signal Handler-----------//
    QSocketNotifier signalFdNotifier(signalFds[0], QSocketNotifier::Read, &app);
    QObject::connect(&signalFdNotifier, &QSocketNotifier::activated,
                     &app, +[]() { std::cerr << "Interrupt\n"; theApplication->exit(1); });
#endif

    //-------------Command Line-----------//

    std::unique_ptr<QCommandLineParser> cmdParser(new QCommandLineParser);

    {
        cmdParser->setApplicationDescription(QObject::tr("Interactive granular sampler"));
        QCommandLineOption optHelp = cmdParser->addHelpOption();
        QCommandLineOption optVersion = cmdParser->addVersionOption();

        QCommandLineOption optNumChannels(
            QStringList() << "c" << "channels",
            QObject::tr("Set the number of output channels."),
            QObject::tr("channel-count"));
        cmdParser->addOption(optNumChannels);

        QCommandLineOption optAutoconnect(
            QStringList() << "a" << "autoconnect",
            QObject::tr("Automatically connect audio streams to the output device."));
        cmdParser->addOption(optAutoconnect);

        QCommandLineOption optFps(
            QStringList() << "f" << "fps",
            QObject::tr("Set the refresh rate in frames per second."),
            QObject::tr("refresh-rate"));
        cmdParser->addOption(optFps);

        cmdParser->process(app);

        if (cmdParser->isSet(optNumChannels))
            theChannelCount = cmdParser->value(optNumChannels).toUInt();
        if (cmdParser->isSet(optAutoconnect))
            autoconnect = true;
        if (cmdParser->isSet(optFps)) {
            fps = cmdParser->value(optFps).toDouble();
            if (fps < 1 || fps > 100) {
                std::cerr << QObject::tr("Invalid FPS value.").toStdString() << "\n";
                return 1;
            }
        }
    }

    cmdParser.reset();

    //-------------Audio Configuration-----------//

    // configure Jack or RtAudio
    std::cerr << "* try JACK audio\n";
    theAudio = new MyRtJack(theChannelCount);
    // open audio stream/assign callback
    if (!theAudio->openStream(&audioCallback, nullptr)) {
        delete theAudio;
        theAudio = nullptr;
    }
    if (!theAudio) {
        std::cerr << "* try RtAudio\n";
        theAudio = new MyRtAudio(theChannelCount);
        if (!theAudio->openStream(&audioCallback, nullptr)) {
            cleaningFunction();
            return 1;
        }
    }

    std::cerr << "desired channels: " << theChannelCount << "\n";
    theChannelCount = theAudio->getObtainedOutputChannels();
    std::cerr << "obtained channels: " << theChannelCount << "\n";

    unsigned sampleRate = theAudio->getSampleRate();
    ::samp_rate = sampleRate;
    ::samp_time_sec = 1.0 / sampleRate;
    Stk::setSampleRate(sampleRate);

    // get new buffer size
    g_buffSize = theAudio->getBufferSize();
    cerr << "obtained buffer size: " << g_buffSize << endl;
    // report latency
    cout << "stream latency: " << theAudio->getStreamLatency() << " frames" << endl;

    //-------------Midi Configuration-----------//
    theMidiInBuffer = new Ring_Buffer(1024);
    theAudio->openMidiInput(&midiInCallback, theMidiInBuffer->capacity(), nullptr);

    //-------------OSC Configuration-----------//
    theOscInBuffer = new Ring_Buffer(8192);

    //-------------App Initialization--------//

    // init openGL format for anti-aliasing
    QSurfaceFormat openGLFormat = QSurfaceFormat::defaultFormat();
    openGLFormat.setSamples(4);
    QSurfaceFormat::setDefaultFormat(openGLFormat);

    // init Qt application
    app.initializeInterface();

    //-------------Paths Initialization--------//
#if defined(Q_OS_WIN)
    // subdirectory of program path
    const QDir programDir = QFileInfo(QCoreApplication::applicationFilePath()).dir();
    g_audioPathSystem = programDir.filePath("loops").toStdString();
#elif defined(Q_OS_MAC)
    // folder of bundle resources
    const QDir programDir = QFileInfo(QCoreApplication::applicationFilePath()).dir();
    g_audioPathSystem = programDir.filePath("../Resources/loops").toStdString();
#else
    // in the share directory of the root prefix
    g_audioPathSystem = DATA_ROOT_DIR "/Frontieres/loops";
#endif

    // user path in system-dependent place
    g_audioPathUser = (QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
        "/loops").toStdString();

    cout << "Audio path of user: " << g_audioPathUser << "\n";
    cout << "Audio path of system: " << g_audioPathSystem << "\n";

    // create the user directory
    QDir(QString::fromStdString(g_audioPathUser)).mkpath(".");

    //-------------Graphics Initialization--------//

    // initialize graphics
    MyGLWindow *GLwindow = app.GLwindow();
    text_renderer = new QtFont3D;
    GLwindow->initialize();
    GLwindow->show();


    app.startIdleCallback(fps);

    StartDialog startDlg;
    startDlg.adjustSize();
    startDlg.exec();

    //
    int startChoice = startDlg.choiceResult();

    Scene *scene = new Scene;

    switch (startChoice) {
    default:
        break;

    case StartDialog::Choice_NewScene: {
        std::vector<Sample *> loaded;

        // attempt to load from user, then system if empty
        scene->m_sampleSet->loadFileSet(g_audioPathUser, &loaded);
        if (loaded.empty())
            scene->m_sampleSet->loadFileSet(g_audioPathSystem, &loaded);

        // add them into the scene
        for (Sample *af : loaded)
            scene->addSampleVis(af);

        break;
    }

    case StartDialog::Choice_LoadScene: {
        std::string nameSceneFile = scene->askNameScene(FileDirection::Load);
        QFile sceneFile(QString::fromStdString(nameSceneFile));
        if (!scene->load(sceneFile) || !scene->loadSampleSet(true))
            scene->clear();
        break;
    }

    }

    ::currentScene = scene;

    // start audio stream
    theAudio->startStream();
    if (autoconnect)
        theAudio->connectOutputs();

    // start OSC
    MyRtOsc &osc = MyRtOsc::instance();
    osc.setMessageHandler(&oscCallback, nullptr);
    if (!osc.open() || !osc.start()) {
        std::cerr << "Error: failed to start OSC!\n";
        return 1;
    }

    std::string oscUrl = osc.getUrl();
    std::cout << "OSC address: " << oscUrl << "\n";

    QObject::connect(
        &app, &MyGLApplication::oscMessageArrived,
        &app, [](const QByteArray &message)
                  { processOscMessage(message.data(), message.size(), Ports::nonRtRoot); },
        Qt::QueuedConnection);

    // let Qt handle the current thread from here
    exitCode = app.exec();


    // cleanup routine
    cleaningFunction();

    // done
    return exitCode;
}
