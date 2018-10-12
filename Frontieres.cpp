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
#ifdef __MACOSX_CORE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// other libraries
#include <iostream>
#include <vector>
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
#include "AudioFileSet.h"
#include "Window.h"

// midi related
#include <RtMidi.h>
#include <ring_buffer.h>

// graphics related
#include "SoundRect.h"

// graphics and audio related
#include "GrainCluster.h"

// internationalization
#include "I18n.h"

// Qt application
#include "MyGLApplication.h"
#include "MyGLWindow.h"
#include <QtFont3D.h>


using namespace std;



//-----------------------------------------------------------------------------
// Shared Data Structures, Global parameters
//-----------------------------------------------------------------------------
// audio system
MyRtAudio *theAudio = NULL;
// midi system
RtMidiIn *theMidiIn = NULL;
// buffer of midi input messages
Ring_Buffer *theMidiInBuffer = NULL;
// library path
string g_audioPath = "./loops/";
// parameter string
string paramString = "";
// desired audio buffer size
unsigned int g_buffSize = 1024;
// audio files
vector<AudioFile *> *mySounds = NULL;
// audio file visualization objects
vector<SoundRect *> *soundViews = NULL;
// grain cloud audio objects
vector<GrainCluster *> *grainCloud = NULL;
// grain cloud visualization objects
vector<GrainClusterVis *> *grainCloudVis;
// cloud counter
unsigned int numClouds = 0;

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

// not used yet - for multiple selection
vector<int> *selectionIndices = new vector<int>;

// selection helper vars
int selectedCloud = -1;
int selectedRect = -1;
bool menuFlag = true;
int selectionIndex = 0;

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


//--------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------

void updateMouseCoords(int x, int y);
void draw_string(GLfloat x, GLfloat y, GLfloat z, const QString &str, GLfloat scale);
void drawAxis();
int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int numFrames,
                  double streamTime, RtAudioStreamStatus status, void *userData);
void processMidiMessage(const unsigned char *message, unsigned length);
void midiInCallback(double timeStamp, std::vector<unsigned char> *message, void *userData);


//--------------------------------------------------------------------------------
// Cleanup code
//--------------------------------------------------------------------------------

void cleaningFunction()
{
    if (theAudio != NULL) {
        try {
            theAudio->stopStream();
            theAudio->closeStream();
        }
        catch (RtAudioError &err) {
            err.printMessage();
        }
        delete theAudio;
    }
    if (theMidiIn != NULL) {
        try {
            theMidiIn->closePort();
        }
        catch (RtAudioError &err) {
            err.printMessage();
        }
        delete theMidiIn;
    }
    if (mySounds != NULL)
        delete mySounds;

    if (grainCloud != NULL) {
        delete grainCloud;
    }

    if (grainCloudVis != NULL) {
        delete grainCloudVis;
    }
    if (soundViews != NULL) {
        delete soundViews;
    }
    if (selectionIndices != NULL) {
        delete selectionIndices;
    }
    if (text_renderer != NULL) {
        delete text_renderer;
    }
}


//================================================================================
//   Audio Callback
//================================================================================

// audio callback
int audioCallback(void *outputBuffer, void *inputBuffer, unsigned int numFrames,
                  double streamTime, RtAudioStreamStatus status, void *userData)
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

    // cast audio buffers
    SAMPLE *out = (SAMPLE *)outputBuffer;
    SAMPLE *in = (SAMPLE *)inputBuffer;

    memset(out, 0, sizeof(SAMPLE) * numFrames * MY_CHANNELS);
    if (menuFlag == false) {
        for (int i = 0; i < grainCloud->size(); i++) {
            grainCloud->at(i)->nextBuffer(out, numFrames);
        }
    }
    GTime::instance().sec += numFrames * samp_time_sec;
    // cout << GTime::instance().sec<<endl;
    return 0;
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
    case 0x80:  // note on, 1=key number, 2=velocity
        /* do something */
        break;

    case 0x90:  // note off, 1=key number, 2=velocity
        /* do something */
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

//================================================================================
//   Midi Input Callback
//================================================================================

void midiInCallback(double, std::vector<unsigned char> *message, void *)
{
    size_t size = message->size();
    if (size >= 256)
        return;  // drop large messages

    // send the midi into a buffer the audio thread will process from

    if (theMidiInBuffer->size_free() < 1 + size)
        return;  // check if the buffer can take the message, if not drop

    // write the message header, a 8bit size field
    theMidiInBuffer->put((unsigned char)size);
    // write the message body
    theMidiInBuffer->put(message->data(), size);
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
void draw_string(GLfloat x, GLfloat y, GLfloat z, const QString &str, GLfloat scale = 1.0f)
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

    float smallSize = 0.03f;
    float mediumSize = 0.04f;
    glLineWidth(2.0f);
    float theA = 0.6f + 0.2 * sin(0.8 * PI * GTime::instance().sec);
    glColor4f(theA, theA, theA, theA);
    draw_string(screenWidth / 2.0f + 0.2f * (float)screenWidth, (float)screenHeight / 2.0f,
                0.5f, u8"FRONTIÈRES", (float)screenWidth * 0.1f);

    theA = 0.6f + 0.2 * sin(0.9 * PI * GTime::instance().sec);
    float insColor = theA * 0.4f;
    glColor4f(insColor, insColor, insColor, theA);
    // key info
    draw_string(screenWidth / 2.0f + 0.2f * (float)screenWidth + 10.0,
                (float)screenHeight / 2.0f + 30.0, 0.5f, _Q("", "CLICK TO START"),
                (float)screenWidth * 0.04f);

    theA = 0.6f + 0.2 * sin(1.0 * PI * GTime::instance().sec);
    insColor = theA * 0.4f;
    glColor4f(insColor, insColor, insColor, theA);
    // key info
    draw_string(screenWidth / 2.0f + 0.2f * (float)screenWidth + 10.0,
                (float)screenHeight / 2.0f + 50.0, 0.5f, _Q("", "ESCAPE TO QUIT"),
                (float)screenWidth * 0.04f);

    theA = 0.6f + 0.2 * sin(1.1 * PI * GTime::instance().sec);
    insColor = theA * 0.4f;
    glColor4f(insColor, insColor, insColor, theA);
    // key info
    draw_string(screenWidth / 2.0f + 0.2f * (float)screenWidth + 10.0,
                (float)screenHeight / 2.0f + 70.0, 0.5f,
                _Q("", "PUT THE SAMPLES IN ~/.Frontieres/loops"), (float)screenWidth * 0.04f);
}


void printParam()
{
    // get screen width and height
    MyGLScreen *screen = theApplication->GLwindow()->screen();
    int screenHeight = screen->height();

    if ((numClouds > 0) && (selectedCloud >= 0)) {
        GrainClusterVis *theCloudVis = grainCloudVis->at(selectedCloud);
        GrainCluster *theCloud = grainCloud->at(selectedCloud);
        float cloudX = theCloudVis->getX();
        float cloudY = theCloudVis->getY();
        string myValue;
        ostringstream sinput;
        ostringstream sinput2;
        float theA = 0.7f + 0.3 * sin(1.6 * PI * GTime::instance().sec);
        glColor4f(1.0f, 1.0f, 1.0f, theA);

        switch (currentParam) {
        case NUMGRAINS:
            myValue = _S("", "Voices: ");
            sinput << theCloud->getNumVoices();
            myValue = myValue + sinput.str();
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case DURATION:
            myValue = _S("", "Duration: ");
            if (paramString == "") {
                sinput << theCloud->getDurationMs();
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
            switch (theCloud->getWindowType()) {
            case HANNING:
                myValue = _S("", "Window: HANNING");
                break;
            case TRIANGLE:
                myValue = _S("", "Window: TRIANGLE");
                break;
            case REXPDEC:
                myValue = _S("", "Window: REXPDEC");
                break;
            case EXPDEC:
                myValue = _S("", "Window: EXPDEC");
                break;
            case SINC:
                myValue = _S("", "Window: SINC");
                break;
            case RANDOM_WIN:
                myValue = _S("", "Window: RANDOM_WIN");
                break;
            default:
                myValue = "";
                break;
            }

            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case MOTIONX:
            myValue = _S("", "X: ");
            sinput << theCloudVis->getXRandExtent();
            myValue = myValue + sinput.str();
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case MOTIONY:
            myValue = _S("", "Y: ");
            sinput << theCloudVis->getYRandExtent();
            myValue = myValue + sinput.str();
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case MOTIONXY:
            myValue = _S("", "X,Y: ");
            sinput << theCloudVis->getXRandExtent();
            myValue = myValue + sinput.str() + ", ";
            sinput2 << theCloudVis->getYRandExtent();
            myValue = myValue + sinput2.str();
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;

        case DIRECTION:
            switch (theCloud->getDirection()) {
            case FORWARD:
                myValue = _S("", "Direction: FORWARD");
                break;
            case BACKWARD:
                myValue = _S("", "Direction: BACKWARD");
                break;
            case RANDOM_DIR:
                myValue = _S("", "Direction: RANDOM");
                break;
            default:
                myValue = "";
                break;
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;

        case SPATIALIZE:
            switch (theCloud->getSpatialMode()) {
            case UNITY:
                myValue = _S("", "Spatial Mode: UNITY");
                break;
            case STEREO:
                myValue = _S("", "Spatial Mode: STEREO");
                break;
            case AROUND:
                myValue = _S("", "Spatial Mode: AROUND");
                break;
            default:
                myValue = "";
                break;
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case VOLUME:
            myValue = _S("", "Volume (dB): ");
            if (paramString == "") {
                sinput << theCloud->getVolumeDb();
                myValue = myValue + sinput.str();
            }
            else {
                myValue = myValue + paramString;
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            break;
        case OVERLAP:
            myValue = _S("", "Overlap: ");
            if (paramString == "") {
                sinput << theCloud->getOverlap();
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
            myValue = _S("", "Pitch: ");
            if (paramString == "") {
                sinput << theCloud->getPitch();
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
            myValue = _S("", "Pitch LFO Freq: ");
            if (paramString == "") {
                sinput << theCloud->getPitchLFOFreq();
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
            myValue = _S("", "Pitch LFO Amount: ");
            if (paramString == "") {
                sinput << theCloud->getPitchLFOAmount();
                myValue = myValue + sinput.str();
            }
            else {
                myValue = myValue + paramString;
            }
            draw_string((GLfloat)mouseX, (GLfloat)(screenHeight - mouseY), 0.0,
                        myValue.c_str(), 100.0f);
            //            myValue = "Duration (ms): " + theCloud->getDurationMs();
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
// Handle mouse clicks, etc.
//-----------------------------------------------------------------------------


// handle deselections
void deselect(int shapeType)
{
    switch (shapeType) {
    case CLOUD:
        if (selectedCloud >= 0) {
            grainCloudVis->at(selectedCloud)->setSelectState(false);
            // reset selected cloud
            selectedCloud = -1;
            // cout << "deselecting cloud" <<endl;
        }

    case RECT:
        if (selectedRect >= 0) {
            // cout << "deselecting rect" << endl;
            soundViews->at(selectedRect)->setSelectState(false);
            selectedRect = -1;
        }
    }
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

    if (selectedCloud >= 0) {
        grainCloudVis->at(selectedCloud)->updateCloudPosition(mouseX, mouseY);
    }
    else {

        switch (dragMode) {
        case MOVE:
            if ((lastDragX != veryHighNumber) && (lastDragY != veryHighNumber)) {

                if (selectedRect >= 0) {  // movement case
                    soundViews->at(selectedRect)->move(mouseX - lastDragX, mouseY - lastDragY);
                }
            }
            lastDragX = mouseX;
            lastDragY = mouseY;
            break;

        case RESIZE:
            if ((lastDragX != veryHighNumber) && (lastDragY != veryHighNumber)) {
                // cout << "drag ok" << endl;
                // for width height - use screen coords

                if (selectedRect >= 0) {
                    xDiff = x - lastDragX;
                    yDiff = y - lastDragY;
                    // get width and height
                    float newWidth = soundViews->at(selectedRect)->getWidth();
                    float newHeight = soundViews->at(selectedRect)->getHeight();

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
                    soundViews->at(selectedRect)->setWidthHeight(newWidth, newHeight);
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


    if (selectedCloud >= 0) {
        switch (currentParam) {
        case MOTIONX:
            grainCloudVis->at(selectedCloud)->setXRandExtent(mouseX);
            break;
        case MOTIONY:
            grainCloudVis->at(selectedCloud)->setYRandExtent(mouseY);
            break;
        case MOTIONXY:
            grainCloudVis->at(selectedCloud)->setRandExtent(mouseX, mouseY);
            break;
        default:
            break;
        }
    }
    //            case NUMGRAINS:
    //                break;
    //            case DURATION:
    //                grainCloud->at(selectedCloud)->setDurationMs((mouseY/screenHeight)*4000.0f);
    //
    //            default:
    //                break;
    //        }
    //    }
    // process rectangles
    //    for (int i = 0; i < soundViews->size(); i++)
    //        soundViews->at(i)->procMovement(x, y);
    //
}


//-----------------------------------------------------------------------------//
// MAIN
//-----------------------------------------------------------------------------//
int main(int argc, char **argv)
{
    // init random number generator
    srand(time(NULL));
    // start time

    //-------------Audio Configuration-----------//

    // configure RtAudio
    // create the object
    try {
        theAudio = new MyRtAudio(1, MY_CHANNELS, &g_buffSize, MY_FORMAT, true);
    }
    catch (RtAudioError &err) {
        err.printMessage();
        cleaningFunction();
        exit(1);
    }
    try {
        unsigned sampleRate = theAudio->getSampleRate();
        ::samp_rate = sampleRate;
        ::samp_time_sec = 1.0 / sampleRate;
        Stk::setSampleRate(sampleRate);
        // open audio stream/assign callback
        theAudio->openStream(&audioCallback);
        // get new buffer size
        g_buffSize = theAudio->getBufferSize();
        // report latency
        theAudio->reportStreamLatency();
    }
    catch (RtAudioError &err) {
        err.printMessage();
        cleaningFunction();
        exit(1);
    }

    //-------------Midi Configuration-----------//
    theMidiInBuffer = new Ring_Buffer(1024);
    try {
        theMidiIn = new RtMidiIn(RtMidi::UNSPECIFIED, "Frontieres", theMidiInBuffer->capacity());
        theMidiIn->setCallback(&midiInCallback);
        theMidiIn->openVirtualPort();
    }
    catch (RtMidiError &err) {
        err.printMessage();
        cleaningFunction();
        exit(1);
    }

    //-------------Graphics Initialization--------//

    // init Qt application
    MyGLApplication app(argc, argv);

    // initialize graphics
    MyGLWindow *GLwindow = app.GLwindow();
    text_renderer = new QtFont3D;
    GLwindow->initialize();

    double fps = 50;
    app.startIdleCallback(fps);


    // load sounds
    QString captionPath = "Frontières : choix du répertoire des échantillons";
    string separateurPath = "/";
    QString homeUser = getenv("HOME");
    string audioPathDefault = DATA_ROOT_DIR "/Frontieres/loops/";
    QString qaudioPathDefault = DATA_ROOT_DIR "/Frontieres/loops/";
    string audioPathUser = QFileDialog::getExistingDirectory(nullptr, captionPath, qaudioPathDefault,
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks).toUtf8().constData() + separateurPath ;
    bool audioPathUserEmpty = true;
    if (DIR *rep = opendir(audioPathUser.c_str())) {
        struct dirent *ent;
            fprintf(stderr, "Coucou(1)\n");
        while (audioPathUserEmpty && (ent = readdir(rep)))
            audioPathUserEmpty = !strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..");
        closedir(rep);
    }
    g_audioPath = audioPathUserEmpty ? audioPathDefault : audioPathUser;

    cout << "Audio path of user: " << audioPathUser << "\n";
    cout << "Audio path of system: " << audioPathDefault << "\n";
    cout << "Audio path used: " << g_audioPath << "\n";

    AudioFileSet newFileMgr;

    if (newFileMgr.loadFileSet(g_audioPath) == 1) {
        goto cleanup;
    }

    mySounds = newFileMgr.getFileVector();
    cout << _S("", "Sounds loaded successfully...") << endl;


    // create visual representation of sounds
    soundViews = new vector<SoundRect *>;
    for (int i = 0; i < mySounds->size(); i++) {
        soundViews->push_back(new SoundRect());
        soundViews->at(i)->associateSound(mySounds->at(i)->wave, mySounds->at(i)->frames,
                                          mySounds->at(i)->channels);
    }

    // init grain cloud vector and corresponding view vector
    grainCloud = new vector<GrainCluster *>;
    grainCloudVis = new vector<GrainClusterVis *>;


    // start audio stream
    theAudio->startStream();


    // start graphics
    // let Qt handle the current thread from here
    GLwindow->show();
    return app.exec();


    // cleanup routine
cleanup:
    cleaningFunction();

    // done
    return 0;
}
