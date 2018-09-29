//------------------------------------------------------------------------------
// BORDERLANDS:  An interactive granular sampler.  
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
//  Borderlands
//
//  Created by Christopher Carlson on 11/13/11.
//


//my includes
#include "theglobals.h"

//graphics includes
#ifdef __MACOSX_CORE__
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

//other libraries
#include <iostream>
#include <sys/stat.h>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <dirent.h>

//audio related
#include "MyRtAudio.h"
#include "AudioFileSet.h"
#include "Window.h"

//graphics related
#include "SoundRect.h"

//graphics and audio related
#include "GrainCluster.h"
 
#ifndef WIN32
    #include <sys/types.h>
#endif


using namespace std;

unsigned int screenWidth, screenHeight;


//-----------------------------------------------------------------------------
// Defines a point in a 3D space (coords x, y and z)
//-----------------------------------------------------------------------------
struct pt3d
{ 
    pt3d( GLfloat _x, GLfloat _y, GLfloat _z ) : x(_x), y(_y), z(_z) {};
    
    float x;
    float y;
    float z;
};



//-----------------------------------------------------------------------------
// Shared Data Structures, Global parameters
//-----------------------------------------------------------------------------
//audio system
MyRtAudio * theAudio = NULL;
//library path
//string g_audioPath = "./loops/";
string g_audioPath = "";
string g_repUser = getenv("HOME");
string g_programPathUser = g_repUser + "/.Borderlands/";
string g_audioPathUser = g_repUser + "/.Borderlands/loops/";
string g_audioPathDefault = "/usr/share/Borderlands/loops/";
//parameter string
string paramString = "";
//desired audio buffer size 
unsigned int g_buffSize = 1024;
//audio files
vector <AudioFile *> * mySounds = NULL;
//audio file visualization objects
vector <SoundRect *> * soundViews = NULL;
//grain cloud audio objects
vector<GrainCluster *> * grainCloud = NULL;
//grain cloud visualization objects
vector<GrainClusterVis *> * grainCloudVis;
//cloud counter
unsigned int numClouds = 0;

//global time increment - samples per second
//global time is incremented in audio callback
const double samp_time_sec = (double) 1.0 / (double)MY_SRATE;


//Initial camera movement vars
//my position
pt3d position(0.0,0.0,0.0f);


//ENUMS 
//user selection mode
enum{RECT,CLOUD};
enum{MOVE,RESIZE};
//default selection mode
int selectionMode = CLOUD;
int dragMode = MOVE;
bool resizeDir = false; //for rects
//rubber band select params
int rb_anchor_x = -1;
int rb_anchor_y = -1;

//not used yet - for multiple selection
vector<int> * selectionIndices = new vector<int>;

//selection helper vars
int selectedCloud = -1;
int selectedRect = -1;
bool menuFlag = true;
int selectionIndex = 0;

//cloud parameter changing
enum{NUMGRAINS,DURATION,WINDOW, MOTIONX, MOTIONY,MOTIONXY,DIRECTION,OVERLAP, PITCH, ANIMATE,P_LFO_FREQ,P_LFO_AMT,SPATIALIZE,VOLUME};
//flag indicating parameter change
bool paramChanged = false;
unsigned int currentParam = NUMGRAINS;
double lastParamChangeTime = 0.0;
double tempParamVal = -1.0;




//mouse coordinate initialization
int mouseX = -1;
int mouseY = -1;
long veryHighNumber = 50000000;
long lastDragX = veryHighNumber;
long lastDragY = veryHighNumber;


//--------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
//--------------------------------------------------------------------------------

void idleFunc();
void displayFunc();
void reshape(int w, int h);
void specialFunc(int key, int x, int y);
void keyboardFunc(unsigned char key, int x, int y);
void keyUpFunc(unsigned char key, int x, int y);
void deselect(int mode);

void mouseFunc(int button, int state, int x, int y);
void mouseDrag(int x, int y);
void mousePassiveMotion(int x, int y);
void updateMouseCoords(int x, int y);
void initialize();
void draw_string( GLfloat x, GLfloat y, GLfloat z, const char * str, GLfloat scale);
void printUsage();
void printParam();
void drawAxis();
int audioCallback( void * outputBuffer, void * inputBuffer, unsigned int numFrames, double streamTime,RtAudioStreamStatus status, void * userData);
void cleaningFunction();




//--------------------------------------------------------------------------------
// Cleanup code
//--------------------------------------------------------------------------------

void cleaningFunction(){
    try {
        theAudio->stopStream();
        theAudio->closeStream();
    } catch (RtError &err) {
        err.printMessage();
    }
    if (mySounds != NULL)
        delete mySounds;
    if (theAudio !=NULL)
        delete theAudio;
    
    if (grainCloud!=NULL){
        delete grainCloud;
    }    
    
    if (grainCloudVis!=NULL){
        delete grainCloudVis;
    }
    if (soundViews != NULL){
        delete soundViews;
    }
    if (selectionIndices != NULL){
        delete selectionIndices;
    }
}




//================================================================================
//   Audio Callback
//================================================================================

//audio callback
int audioCallback( void * outputBuffer, void * inputBuffer, unsigned int numFrames, double streamTime,
                  RtAudioStreamStatus status, void * userData)
{
        //cast audio buffers
    SAMPLE * out = (SAMPLE *)outputBuffer;
    SAMPLE * in = (SAMPLE *)inputBuffer;
    
    memset(out, 0, sizeof(SAMPLE)*numFrames*MY_CHANNELS );
    if (menuFlag == false){
        for(int i = 0; i < grainCloud->size(); i++){
            grainCloud->at(i)->nextBuffer(out, numFrames);
        }
    }
    GTime::instance().sec += numFrames*samp_time_sec;
    // cout << GTime::instance().sec<<endl;
    return 0;
}




//================================================================================
//   GRAPHICS/GLUT
//================================================================================

void windowInit(){    
    // assign glut function calls
    glutIdleFunc(idleFunc);
    glutReshapeFunc(reshape);
    glutDisplayFunc(displayFunc);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutPassiveMotionFunc(mousePassiveMotion);
    glutKeyboardFunc(keyboardFunc);
    glutKeyboardUpFunc(keyUpFunc);
    // set the special function - called on special keys events (fn, arrows, pgDown, etc)
    glutSpecialFunc( specialFunc );
}


void toggleFullScreen(){
    static bool isFullScreen = true;


    if (isFullScreen)
    {

        glutReshapeWindow(0.6*screenWidth,0.6*screenHeight);
        glutPositionWindow(screenWidth*0.1,100);
        isFullScreen = false;
        /*
        glutLeaveGameMode();
        cout << "left game mode" << endl;
        windowInit();
        glutMainLoop();
        */
        
        
    }else{
        
        /*
        //glutFullScreen();
        string res;
        char buffer[33];
        screenWidth = glutGet(GLUT_SCREEN_WIDTH);
        sprintf(buffer, "%d", screenWidth);
        res.append(buffer);
        res.append("x");
        char buffer2[33];
        screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
        sprintf(buffer2,"%d",screenHeight);
        res.append(buffer2);
        res.append(":32@75");
        cout << res.c_str() << endl;
        
        //presentation view
        //set resolution/refresh rate
        glutGameModeString(res.c_str());
        glutEnterGameMode();
        cout << "entered game mode" << endl;

                windowInit();
        glutMainLoop();
        */
        glutFullScreen();
        
        isFullScreen = true;
    }
    
    //glutDisplayFunc(displayFunc);

}


//-----------------------------------------------------------
//  GLUT Initialization
//-----------------------------------------------------------

void initialize()
{
    
    
    // initial window settings
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    
    
    screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    glutInitWindowSize (screenWidth,screenHeight);
    glutInitWindowPosition (0, 0);
    glutCreateWindow("Borderlands");
    

    /*
    //Game Mode   
    string res;
    char buffer[33];
    screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    sprintf(buffer, "%d", screenWidth);
    res.append(buffer);
    res.append("x");
    char buffer2[33];
    screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
    sprintf(buffer2,"%d",screenHeight);
    res.append(buffer2);
    res.append(":32@75");
    cout << res.c_str() << endl;
    
    //presentation view
    //set resolution/refresh rate
    glutGameModeString(res.c_str());
    glutEnterGameMode();
    
    */
        
    //full screen end
    
    // initial state
    glClearColor(0.15, 0.15, 0.15,1.0f);
    //enable depth buffer updates
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //set polys with counterclockwise winding to be front facing 
    //this is gl default anyway
    glFrontFace( GL_CCW );
    //set fill mode
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode( GL_FRONT_AND_BACK, GL_POLYGON_SMOOTH);
    //enable transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //antialias lines and points
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    
    // assign glut function calls
    glutIdleFunc(idleFunc);
    glutReshapeFunc(reshape);
    glutDisplayFunc(displayFunc);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseDrag);
    glutPassiveMotionFunc(mousePassiveMotion);
    glutKeyboardFunc(keyboardFunc);
    glutKeyboardUpFunc(keyUpFunc);
    // set the special function - called on special keys events (fn, arrows, pgDown, etc)
    glutSpecialFunc( specialFunc );
    
    glutFullScreen();
    


}



//------------------------------------------------------------------------------
// GLUT display function
//------------------------------------------------------------------------------
void displayFunc()
{
    //clear color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearDepth(1.0);
    
    ////PUSH //save current transform
    glPushMatrix();
    
    glLoadIdentity();
    
    //update viewer position
    glTranslatef(-position.x,-position.y,-position.z); //translate the screen to the position of our camera
    if (menuFlag == false){
        //render rectangles
        if (soundViews){
            for (int i = 0; i < soundViews->size(); i++)
            {
                soundViews->at(i)->draw();
            }
        }
        
         //render grain clouds if they exist
         if (grainCloudVis){
             for (int i = 0; i < grainCloudVis->size(); i++)
             {
                 grainCloudVis->at(i)->draw();
             }
         }
        
//print current param if editing
        if ( (selectedCloud >= 0) || (selectedRect >= 0) )
            printParam();
    }else{
        printUsage();
    }
    
    
    
    //printUsage();
    
    //POP ---//restore state
    glPopMatrix();
    
    //flush and swap
    glFlush();//renders and empties buffers
    glutSwapBuffers(); // brings hidden buffer to the front (using double buffering for smoother graphics)
    
}




//------------------------------------------------------------------------------
// GLUT reshape function
//------------------------------------------------------------------------------
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    screenWidth = w;
    screenHeight = h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble) screenWidth, 0.0, (GLdouble)screenHeight, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-position.x,-position.y,-position.z); //translate the screen to the position of our camera
    //request redisplay
    glutPostRedisplay( );
}


//-----------------------------------------------------------------------------
// GlUT idle function 
//-----------------------------------------------------------------------------

void idleFunc(){
    // render the scene
    glutPostRedisplay( );
}


///-----------------------------------------------------------------------------
// name: drawAxis()
// desc: draw 3d axis
//-----------------------------------------------------------------------------
void drawAxis()
{
    //PUSH -- //store state
    glPushMatrix();
    
    //specify vertices with this drawing mode
    glBegin(GL_LINES);
    glLineWidth(0.9f);
    //x axis
    glColor4f(1,0,0,0.9);
    glVertex3f(0,0,0);
    glVertex3f(screenWidth,0,0);
    
    //x axis
    glColor4f(0,1,0,0.9);
    glVertex3f(0,0,0);
    glVertex3f(0,screenHeight,0);
    
    //z axis
    glColor4f(0,0,1,0.7);
    glVertex3f(0,0,0);
    glVertex3f(0,0,400);
    
    //stop drawing
    glEnd();
    
    //POP -- //restore state
    glPopMatrix();
}


//-----------------------------------------------------------------------------
// Display simple string
// desc: from sndpeek source - Ge Wang, et al
//-----------------------------------------------------------------------------
void draw_string( GLfloat x, GLfloat y, GLfloat z, const char * str, GLfloat scale = 1.0f )
{
    GLint len = strlen( str ), i;
    
    glPushMatrix();
    glTranslatef( x, screenHeight-y, z );
    glScalef( .001f * scale, .001f * scale, .001f * scale );
    
    for( i = 0; i < len; i++ )
        glutStrokeCharacter( GLUT_STROKE_MONO_ROMAN, str[i] );
    
    glPopMatrix();
}


//-----------------------------------------------------------------------------
// Show usage on screen.  TODO:  add usage info 
//-----------------------------------------------------------------------------
void printUsage(){
    float smallSize = 0.03f;
    float mediumSize = 0.04f;
    glLineWidth(2.0f);
    float theA = 0.6f + 0.2*sin(0.8*PI*GTime::instance().sec);
    glColor4f(theA,theA,theA,theA);
    draw_string(screenWidth/2.0f + 0.2f*(float)screenWidth,(float)screenHeight/2.0f, 0.5f,"BORDERLANDS",(float)screenWidth*0.1f);
   
    theA = 0.6f + 0.2*sin(0.9*PI*GTime::instance().sec);
    float insColor = theA*0.4f;
    glColor4f(insColor,insColor,insColor,theA);
    //key info
    draw_string(screenWidth/2.0f + 0.2f*(float)screenWidth + 10.0,(float)screenHeight/2.0f + 30.0, 0.5f,"CLICK TO START",(float)screenWidth*0.04f);

    theA = 0.6f + 0.2*sin(1.0*PI*GTime::instance().sec);
    insColor = theA*0.4f;
    glColor4f(insColor,insColor,insColor,theA);
    //key info
    draw_string(screenWidth/2.0f + 0.2f*(float)screenWidth+10.0,(float)screenHeight/2.0f + 50.0, 0.5f,"ESCAPE TO QUIT",(float)screenWidth*0.04f);

    theA = 0.6f + 0.2*sin(1.1*PI*GTime::instance().sec);
    insColor = theA*0.4f;
    glColor4f(insColor,insColor,insColor,theA);
    //key info
    draw_string(screenWidth/2.0f + 0.2f*(float)screenWidth+10.0,(float)screenHeight/2.0f + 70.0, 0.5f,"PUT THE SAMPLES IN ~/.Borderlands/loops",(float)screenWidth*0.04f);
    
}


void printParam(){
    if ((numClouds > 0) && (selectedCloud >=0)){
        GrainClusterVis * theCloudVis= grainCloudVis->at(selectedCloud);
        GrainCluster * theCloud = grainCloud->at(selectedCloud);
        float cloudX = theCloudVis->getX();
        float cloudY = theCloudVis->getY();
        string myValue;
        ostringstream sinput;
        ostringstream sinput2;
        float theA = 0.7f + 0.3*sin(1.6*PI*GTime::instance().sec);
        glColor4f(1.0f,1.0f,1.0f,theA);
        
        switch (currentParam) {
            case NUMGRAINS:
                myValue = "Voices: ";
                sinput << theCloud->getNumVoices();            
                myValue = myValue+ sinput.str();
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);
                break;
            case DURATION:
                myValue = "Duration: ";
                if (paramString == ""){
                    sinput << theCloud->getDurationMs();
                    myValue = myValue + sinput.str() + " ms";
                }else{
                    myValue = myValue + paramString + " ms";
                }
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);
                //            myValue = "Duration (ms): " + theCloud->getDurationMs();
                break;
            case WINDOW:
                switch (theCloud->getWindowType()) {
                    case HANNING:
                        myValue = "Window: HANNING";
                        break;
                    case TRIANGLE:
                        myValue = "Window: TRIANGLE";
                        break;
                    case REXPDEC:
                        myValue = "Window: REXPDEC";
                        break;
                    case EXPDEC:
                        myValue = "Window: EXPDEC";
                        break;
                    case SINC:
                        myValue = "Window: SINC";
                        break;
                    case RANDOM_WIN:
                        myValue = "Window: RANDOM_WIN";
                        break;
                    default:
                        myValue = "";
                        break;
                }
                
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);
                break;
            case MOTIONX:
                myValue = "X: ";
                sinput << theCloudVis->getXRandExtent();
                myValue = myValue + sinput.str();
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);
                break;
            case MOTIONY:
                myValue = "Y: ";
                sinput << theCloudVis->getYRandExtent();
                myValue = myValue + sinput.str();
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);
                break;
            case MOTIONXY:
                myValue = "X,Y: ";
                sinput << theCloudVis->getXRandExtent();
                myValue = myValue + sinput.str() + ", ";
                sinput2 << theCloudVis->getYRandExtent();
                myValue = myValue + sinput2.str();
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);
                break;
                
            case DIRECTION:
                switch(theCloud->getDirection()){
                    case FORWARD:
                        myValue = "Direction: FORWARD";
                        break;
                    case BACKWARD:
                        myValue = "Direction: BACKWARD";
                        break;
                    case RANDOM_DIR:
                        myValue = "Direction: RANDOM";
                        break;
                    default:
                        myValue = "";
                        break;
                }
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);  
                break;
                
            case SPATIALIZE:
                switch(theCloud->getSpatialMode()){
                    case UNITY:
                        myValue = "Spatial Mode: UNITY";
                        break;
                    case STEREO:
                        myValue = "Spatial Mode: STEREO";
                        break;
                    case AROUND:
                        myValue = "Spatial Mode: AROUND";
                        break;
                    default:
                        myValue = "";
                        break;
                }
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);  
                break;
            case VOLUME:
                myValue = "Volume (dB): ";
                if (paramString == ""){
                    sinput << theCloud->getVolumeDb();
                    myValue = myValue + sinput.str();
                }else{
                    myValue = myValue + paramString;
                }
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);
                break;
            case OVERLAP:
                myValue = "Overlap: ";
                if (paramString == ""){
                    sinput << theCloud->getOverlap();
                    myValue = myValue + sinput.str();
                }else{
                    myValue = myValue + paramString;
                }
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);
                //            myValue = "Duration (ms): " + theCloud->getDurationMs();
                break;
            case PITCH:
                myValue = "Pitch: ";
                if (paramString == ""){
                    sinput << theCloud->getPitch();
                    myValue = myValue + sinput.str();
                }else{
                    myValue = myValue + paramString;
                }
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);
                //            myValue = "Duration (ms): " + theCloud->getDurationMs();
                break;
                
            case P_LFO_FREQ:
                myValue = "Pitch LFO Freq: ";
                if (paramString == ""){
                    sinput << theCloud->getPitchLFOFreq();
                    myValue = myValue + sinput.str();
                }else{
                    myValue = myValue + paramString;
                }
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);
                //            myValue = "Duration (ms): " + theCloud->getDurationMs();
                break;
            case P_LFO_AMT:
                myValue = "Pitch LFO Amount: ";
                if (paramString == ""){
                    sinput << theCloud->getPitchLFOAmount();
                    myValue = myValue + sinput.str();
                }else{
                    myValue = myValue + paramString;
                }
                draw_string((GLfloat)mouseX,(GLfloat) (screenHeight-mouseY),0.0,myValue.c_str(),100.0f);
                //            myValue = "Duration (ms): " + theCloud->getDurationMs();
                break;
            default:
                break;
        }
    }
    
}


//================================================================================
//   INTERACTION/GLUT
//================================================================================


//update mouse coords based on mousemovement
void updateMouseCoords(int x, int y){
    mouseX = x+position.x;
    mouseY = (screenHeight - (y-position.y) );
}


//-----------------------------------------------------------------------------
// Handle special function keys (arrows, etc)
//-----------------------------------------------------------------------------
void specialFunc( int key, int x, int y )
{
    static float sidewaysMoveSpeed = 10.0f;
    static float upDownMoveSpeed = 10.0f;
    //cout << "special key" << key <<endl;
    
    switch (key){
        case GLUT_KEY_LEFT:
            //move to the left
            position.x -=  sidewaysMoveSpeed;
            mouseX -=sidewaysMoveSpeed;
            break;
        case GLUT_KEY_RIGHT:
            //move to the right
            position.x += sidewaysMoveSpeed;
            mouseX +=sidewaysMoveSpeed;
            break;
        case GLUT_KEY_DOWN:
            //move backward
            position.y -= upDownMoveSpeed;
            mouseY +=sidewaysMoveSpeed;
            
            break;
        case GLUT_KEY_UP:
            //move forward
            position.y += upDownMoveSpeed;
            mouseY -=sidewaysMoveSpeed;
            break;
            
        case 14:
            //cout << "shift down" << endl;
            break;
        case 15://shift in
            //cout << "shift up" << endl;
            
            break;
            
        default:
            break;
    }
    //request redisplay
    glutPostRedisplay( );
}




//-----------------------------------------------------------------------------
// Handle key up events (other than special keys)
//-----------------------------------------------------------------------------

void keyUpFunc( unsigned char key, int x, int y ){
    //cout << key << endl;
    switch (key) {
        case 'a':
            break;
        case 'R':
        case 'r':
            dragMode = MOVE;
            lastDragX = veryHighNumber;
            lastDragY = veryHighNumber;
            break;
        default:
            break;
    }
    glutPostRedisplay( );
}


//-----------------------------------------------------------------------------
// Handle key down events (other than special keys)
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
    static bool negativeFlag = false;//for negative value entry
    int modkey = glutGetModifiers();
    //cout << "key pressed " << key << endl;
    switch( key )
    {
        
        case 9: //tab key
            
            if (selectionIndices->size() > 1){
                soundViews->at(selectedRect)->setSelectState(false);
                selectionIndex++;
                if (selectionIndex >= selectionIndices->size()){
                    selectionIndex = 0;
                }
                selectedRect = selectionIndices->at(selectionIndex);
                soundViews->at(selectedRect)->setSelectState(true);
            }
            break;
        case '1':
            paramString.push_back('1');
            if (currentParam == WINDOW){
                    if (selectedCloud >=0){
                        grainCloud->at(selectedCloud)->setWindowType(0);
                    }
            }

            break;
        case '2':
            paramString.push_back('2');
            if (currentParam == WINDOW){
                if (selectedCloud >=0){
                    grainCloud->at(selectedCloud)->setWindowType(1);
                }
            }
            break;
        case '3':
            paramString.push_back('3');
            if (currentParam == WINDOW){
                if (selectedCloud >=0){
                    grainCloud->at(selectedCloud)->setWindowType(2);
                }
            }
            break;
            
        case '4':
            paramString.push_back('4');
            if (currentParam == WINDOW){
                if (selectedCloud >=0){
                    grainCloud->at(selectedCloud)->setWindowType(3);
                }
            }
            break;
        case'5':
            paramString.push_back('5');
            if (currentParam == WINDOW){
                if (selectedCloud >=0){
                    grainCloud->at(selectedCloud)->setWindowType(4);
                }
            }
            break;
        case'6':
            paramString.push_back('6');
            if (currentParam == WINDOW){
                if (selectedCloud >=0){
                    grainCloud->at(selectedCloud)->setWindowType(5);
                }
            }
            break;
        case'7':
            paramString.push_back('7');
            break;
        case'8':
            paramString.push_back('8');
            break;
        case'9':
            paramString.push_back('9');
            break;
        case '0':
            paramString.push_back('0');
            break;
        case '.':
            paramString.push_back('.');            
            break;
            
        case 13://enter key - for saving param string
            if (paramString != ""){
                float value = atof(paramString.c_str());
                
                //cout << "value received " << value << endl;
                switch (currentParam){
                    case DURATION:
                        if (selectedCloud >=0){
                            if (value < 1.0){
                                value = 1.0;
                            }
                            grainCloud->at(selectedCloud)->setDurationMs(value);
                        }
                        break;
                    case OVERLAP:
                        if (selectedCloud >=0){
                            grainCloud->at(selectedCloud)->setOverlap(value);
                        }
                        break;
                    case PITCH:
                        if (selectedCloud >=0){
                            grainCloud->at(selectedCloud)->setPitch(value);
                        }
                        break;
                    case P_LFO_FREQ:
                        if (selectedCloud >=0){
                            grainCloud->at(selectedCloud)->setPitchLFOFreq(value);
                        }
                        break;
                    case P_LFO_AMT:
                        if (selectedCloud >=0){
                            grainCloud->at(selectedCloud)->setPitchLFOAmount(value);
                        }
                        break;
                        
                    case VOLUME:
                        if (selectedCloud >=0){
                            grainCloud->at(selectedCloud)->setVolumeDb(value);
                        }
                    default:
                        break;
                }
                paramString = "";
            }
           // cout << "enter key caught" << endl;
            break;
            
            
            
            
        case 27: //esc key
            cleaningFunction();
            exit(1);
            break;
            
        case 'Q'://spatialization
        case 'q':  
                      break;
            
        case 'O':
        case 'o':
            toggleFullScreen();
            break;
            
        case 'T':
        case 't':
            paramString = "";
            if (selectedCloud >=0){
                if (currentParam != SPATIALIZE){
                    currentParam = SPATIALIZE;
                }else{
                    if (modkey == GLUT_ACTIVE_SHIFT){
                        if (selectedCloud >=0){
                            int theSpat = grainCloud->at(selectedCloud)->getSpatialMode();
                            grainCloud->at(selectedCloud)->setSpatialMode(theSpat - 1,-1);
                            
                        }
                    }else{
                        if (selectedCloud >=0){
                            int theSpat = grainCloud->at(selectedCloud)->getSpatialMode();
                            grainCloud->at(selectedCloud)->setSpatialMode(theSpat + 1,-1);
                        }
                    }
                }
            }
            break;
            
        case 'S'://overlap control 
        case 's':
            paramString = "";
            if (currentParam != OVERLAP){
                currentParam = OVERLAP;
            }else{
                if (modkey == GLUT_ACTIVE_SHIFT){
                    if (selectedCloud >=0){
                        float theOver =  grainCloud->at(selectedCloud)->getOverlap();
                        grainCloud->at(selectedCloud)->setOverlap(theOver - 0.01f);
                    }
                }else{
                    if (selectedCloud >=0){
                        float theOver =  grainCloud->at(selectedCloud)->getOverlap();
                        grainCloud->at(selectedCloud)->setOverlap(theOver + 0.01f);
                    }
                }
            }
            break;
        case 'R':
        case 'r':
            if (selectedCloud >=0){
                currentParam = MOTIONXY;
            }
            //toggle selection modes
            dragMode = RESIZE;
            break;
        case 'F'://direction
        case 'f':
            paramString = "";
            if (selectedCloud >=0){
                if (currentParam != DIRECTION){
                    currentParam = DIRECTION;
                }else{
                    if (modkey == GLUT_ACTIVE_SHIFT){
                        if (selectedCloud >=0){
                            int theDir = grainCloud->at(selectedCloud)->getDirection();
                            grainCloud->at(selectedCloud)->setDirection(theDir - 1);
                            
                        }
                    }else{
                        if (selectedCloud >=0){
                            int theDir = grainCloud->at(selectedCloud)->getDirection();
                            grainCloud->at(selectedCloud)->setDirection(theDir + 1);
                        }
                    }
                }
            }
            if (selectedRect >=0){
                soundViews->at(selectedRect)->toggleOrientation();
            }
            //cerr << "Looking from the front" << endl;
            break;
        case 'P'://waveform display on/off
        case 'p':

//            for (int i = 0; i < soundViews->size();i++){
//                soundViews->at(i)->toggleWaveDisplay();
//            }
            break;
        case 'W'://window editing for grain
        case 'w':
            paramString = "";
            if (currentParam != WINDOW){
                currentParam = WINDOW;
            }else{
                if (modkey == GLUT_ACTIVE_SHIFT){
                    if (selectedCloud >=0){
                        int theWin = grainCloud->at(selectedCloud)->getWindowType();
                        grainCloud->at(selectedCloud)->setWindowType(theWin - 1);

                    }
                }else{
                    if (selectedCloud >=0){
                        int theWin = grainCloud->at(selectedCloud)->getWindowType();
                        grainCloud->at(selectedCloud)->setWindowType(theWin + 1);
                    }
                }
            }
            
            break;
            
        case 'B':
        case 'b':
            //cloud volume
            paramString = "";
            if (currentParam != VOLUME){
                currentParam = VOLUME;
            }else{
                if (modkey == GLUT_ACTIVE_SHIFT){
                    if (selectedCloud >=0){
                        float theVol = grainCloud->at(selectedCloud)->getVolumeDb();
                        grainCloud->at(selectedCloud)->setVolumeDb(theVol - 0.5f);
                    }
                }else{
                    if (selectedCloud >=0){
                        float theVol = grainCloud->at(selectedCloud)->getVolumeDb();
                        grainCloud->at(selectedCloud)->setVolumeDb(theVol + 0.5f);
                    }
                }
            }

            
            break;
            
        case '/'://switch to menu view
        case '?':
            menuFlag = !menuFlag;
        
            break;
        case 'G':
        case 'g':
            paramString = "";
            deselect(RECT);
            if (grainCloud != NULL){
                if (modkey == GLUT_ACTIVE_SHIFT){
                    if (grainCloud->size() > 0){
                        grainCloud->pop_back();
                        grainCloudVis->pop_back();
                        numClouds-=1;
                        //cout << "cloud removed" << endl;
                    }
                    if (numClouds == 0){
                        selectedCloud = -1;
                    }else{
                        //still have a cloud so select
                        selectedCloud = numClouds-1;
                        grainCloudVis->at(selectedCloud)->setSelectState(true);
                    }
                    break;
                    
                }else{
                    int numVoices = 8;//initial number of voices
                    int idx = grainCloud->size();
                    if (selectedCloud >=0){
                        if (numClouds > 0){
                            grainCloudVis->at(selectedCloud)->setSelectState(false);
                        }
                    }
                    selectedCloud = idx;
                    //create audio
                    grainCloud->push_back(new GrainCluster(mySounds,numVoices));
                    //create visualization
                    grainCloudVis->push_back(new GrainClusterVis(mouseX,mouseY,numVoices,soundViews));
                    //select new cloud
                    grainCloudVis->at(idx)->setSelectState(true);
                    //register visualization with audio
                    grainCloud->at(idx)->registerVis(grainCloudVis->at(idx));
                    //grainCloud->at(idx)->toggleActive();
                    numClouds+=1;
                }
                //                        cout << "cloud added" << endl;
                //grainControl->newCluster(mouseX,mouseY,1);
            }
          
            break;
        case 'V': //grain voices (add, delete)
        case 'v':
            paramString = "";
            if (currentParam != NUMGRAINS){
                currentParam = NUMGRAINS;
            }else{
                if (modkey == GLUT_ACTIVE_SHIFT){
                    if (selectedCloud >=0){
                        if (grainCloud)
                            grainCloud->at(selectedCloud)->removeGrain();
                        //cout << "grain removed" << endl;
                    }
                    
                }else{
                    if (selectedCloud >=0){
                        if (grainCloud)
                            grainCloud->at(selectedCloud)->addGrain();
                        //cout << "grain added" << endl;
                    }
                }
            }
            break;
        
        case 'D':
        case 'd':
            paramString = "";
            if (currentParam != DURATION){
                currentParam = DURATION;
            }else{
                if (modkey == GLUT_ACTIVE_SHIFT){
                    if (selectedCloud >=0){
                        float theDur = grainCloud->at(selectedCloud)->getDurationMs();
                        grainCloud->at(selectedCloud)->setDurationMs(theDur - 5.0f);
                    }
                }else{
                    if (selectedCloud >=0){
                        float theDur = grainCloud->at(selectedCloud)->getDurationMs();
                        grainCloud->at(selectedCloud)->setDurationMs(theDur + 5.0f);
                    }
                }
            }
            break;    
        case 'I':
        case 'i':
            break;
            
            
        case 'L':
        case 'l':
            paramString = "";
            if (currentParam != P_LFO_FREQ){
                currentParam = P_LFO_FREQ;
            }else{
                if (modkey == GLUT_ACTIVE_SHIFT){
                    if (selectedCloud >=0){
                        float theLFOFreq = grainCloud->at(selectedCloud)->getPitchLFOFreq();
                        grainCloud->at(selectedCloud)->setPitchLFOFreq(theLFOFreq - 0.01f);
                    }
                }else{
                    if (selectedCloud >=0){
                        float theLFOFreq = grainCloud->at(selectedCloud)->getPitchLFOFreq();
                        grainCloud->at(selectedCloud)->setPitchLFOFreq(theLFOFreq + 0.01f);
                    }
                }
            }
            break;
            
        case 'K':
        case 'k':
            paramString = "";
            if (currentParam != P_LFO_AMT){
                currentParam = P_LFO_AMT;
            }else{
                if (modkey == GLUT_ACTIVE_SHIFT){
                    if (selectedCloud >=0){
                        float theLFOAmt = grainCloud->at(selectedCloud)->getPitchLFOAmount();
                        grainCloud->at(selectedCloud)->setPitchLFOAmount(theLFOAmt - 0.001f);
                    }
                }else{
                    if (selectedCloud >=0){
                        float theLFOAmnt = grainCloud->at(selectedCloud)->getPitchLFOAmount();
                        grainCloud->at(selectedCloud)->setPitchLFOAmount(theLFOAmnt + 0.001f);
                    }
                }
            }
            break;
        case 'H':
        case 'h':
            break;
        case ' '://add delete
            
            break;
            
        case 'X':
        case 'x':
            paramString = "";
            if (selectedCloud >= 0){
                currentParam = MOTIONX;
            }
            break;
        case 'Y':
        case 'y':
            paramString = "";
            if (selectedCloud >= 0){
                currentParam = MOTIONY;
            }
            break;
            
        case 'Z':
        case 'z':
            paramString = "";
            if (currentParam != PITCH){
                currentParam = PITCH;
            }else{
                if (modkey == GLUT_ACTIVE_SHIFT){
                    if (selectedCloud >=0){
                        float thePitch =  grainCloud->at(selectedCloud)->getPitch();
                        grainCloud->at(selectedCloud)->setPitch(thePitch - 0.01f);
                    }
                }else{
                    if (selectedCloud >=0){
                        float thePitch =  grainCloud->at(selectedCloud)->getPitch();
                        grainCloud->at(selectedCloud)->setPitch(thePitch + 0.01f);
                    }
                }
            }
            break;
                        
        case '-':
        case '_':
            paramString.insert(0,"-");
            break;
        case 127://delete selected
            if (paramString == ""){
                if (selectedCloud >=0){
                    grainCloud->erase(grainCloud->begin() + selectedCloud);
                    grainCloudVis->erase(grainCloudVis->begin() + selectedCloud);
                    selectedCloud = -1;
                    numClouds-=1;
                }
            }else{
                if (paramString.size () > 0)  paramString.resize (paramString.size () - 1);
            }
                    
            break;
        case 'A':
        case 'a':
            paramString = "";
            if (selectedCloud >=0){
                grainCloud->at(selectedCloud)->toggleActive();
            }
            break;
        case '=':
        case '+':
            
           
                           
            break;
        case ',':
        case '<':
            break;
        case '>':
            break;
            
        default:
            //cout << "key pressed " << key << endl;
            break;
    }
    glutPostRedisplay( );
}


//-----------------------------------------------------------------------------
// Handle mouse clicks, etc.
//-----------------------------------------------------------------------------


//handle deselections
void deselect(int shapeType){
    switch (shapeType){
        case CLOUD:
            if (selectedCloud >=0){
                grainCloudVis->at(selectedCloud)->setSelectState(false);
                //reset selected cloud
                selectedCloud = -1;
                //cout << "deselecting cloud" <<endl;
            }
            
        case RECT:
            if (selectedRect >= 0){
                //cout << "deselecting rect" << endl;
                soundViews->at(selectedRect)->setSelectState(false);
                selectedRect = -1;
            }
            
    }
}



//mouse clicks
void mouseFunc(int button, int state, int x, int y){
    //cout << "button " << button << endl;

            //look for selections if button is down
            if ((button == GLUT_LEFT_BUTTON) || (button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)){
                
                paramString = "";
                
                //hide menu
                if (menuFlag == true)
                    menuFlag = false;
                
                deselect(CLOUD);
                //deselect existing selections
                deselect(RECT);
                //exit parameter editing
                currentParam = -1;
                
                lastDragX = veryHighNumber;
                lastDragY = veryHighNumber;
                //first check grain clouds to see if we have selection
                for (int i = 0; i < grainCloudVis->size(); i++){
                    if (grainCloudVis->at(i)->select(mouseX, mouseY) == true){
                        grainCloudVis->at(i)->setSelectState(true);
                        selectedCloud = i;
                        break;
                    }            
                }
                
                
                //clear selection buffer
                if (selectionIndices)
                    delete selectionIndices;
                //allocate new buffer
                selectionIndices = new vector<int>;
                selectionIndex = 0;
                //if grain cloud is not selected - search for rectangle selection
                if (selectedCloud < 0){
                    //search for selections
                    resizeDir = false;//set resize direction to horizontal
                    for (int i = 0; i < soundViews->size(); i++){
                        if (soundViews->at(i)->select(mouseX,mouseY) == true){
                            selectionIndices->push_back(i);
                            //soundViews->at(i)->setSelectState(true);
                            //selectedRect = i;
                            //break;
                        }
                    }
                    
                    if (selectionIndices->size() > 0){
                        selectedRect = selectionIndices->at(0);
                        soundViews->at(selectedRect)->setSelectState(true);
                    }
                }
                
            }

    
//    //handle button up
//    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)){
//        lastDragX = -1;
//        lastDragY = -1;
//        dragMode = MOVE;
//    }
    
}

//-----------------------------------------------------------------------------
// Handle mouse movement during button press (drag)
//-----------------------------------------------------------------------------

void mouseDrag(int x, int y)
{
    //update mouse coordinates based on drag position
    updateMouseCoords(x,y);
    int xDiff = 0;
    int yDiff = 0;
    
    if (selectedCloud >= 0){
        grainCloudVis->at(selectedCloud)->updateCloudPosition(mouseX,mouseY);
    }else{
        
        switch (dragMode) {
            case MOVE:
                if( (lastDragX != veryHighNumber) && (lastDragY != veryHighNumber)){
                    
                    if (selectedRect >=0){                    //movement case                    
                        soundViews->at(selectedRect)->move(mouseX - lastDragX,mouseY - lastDragY);
                    }
                }
                lastDragX = mouseX;
                lastDragY = mouseY;
                break;
                
            case RESIZE:
                if( (lastDragX != veryHighNumber) && (lastDragY != veryHighNumber)){
                    //cout << "drag ok" << endl;
                    //for width height - use screen coords
                    
                    if (selectedRect >= 0){
                        xDiff = x - lastDragX;
                        yDiff = y - lastDragY;
                        //get width and height
                        float newWidth = soundViews->at(selectedRect)->getWidth();
                        float newHeight = soundViews->at(selectedRect)->getHeight();
                        
                        int thresh = 0;
                        //check motion mag
                        if (xDiff < -thresh){
                            newWidth = newWidth * 0.8 + 0.2*(newWidth * (1.1 + abs(xDiff/50.0)));
                        }else{
                            if (xDiff > thresh)
                                newWidth = newWidth * 0.8 + 0.2*(newWidth * (0.85 - abs(xDiff/50.0)));
                        }
                        if (yDiff > thresh){
                            newHeight = newHeight * 0.8 + 0.2*(newHeight * (1.1 + abs(yDiff/50.0)));
                        }else{
                            if (yDiff < -thresh)
                         newHeight = newHeight * 0.8 + 0.2*(newHeight * (0.85 - abs(yDiff/50.0)));
                        }
                            
                            //update width and height
                        soundViews->at(selectedRect)->setWidthHeight(newWidth,newHeight);
                            
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
    updateMouseCoords(x,y);
    
    
        if (selectedCloud >=0){
            switch (currentParam) {
                case MOTIONX:
                    grainCloudVis->at(selectedCloud)->setXRandExtent(mouseX);
                    break;
                case MOTIONY:
                    grainCloudVis->at(selectedCloud)->setYRandExtent(mouseY);
                    break;
                case MOTIONXY:
                    grainCloudVis->at(selectedCloud)->setRandExtent(mouseX,mouseY);
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
    //process rectangles
    //    for (int i = 0; i < soundViews->size(); i++)
    //        soundViews->at(i)->procMovement(x, y);
    //    
}






//-----------------------------------------------------------------------------//
// MAIN
//-----------------------------------------------------------------------------//
int main (int argc, char ** argv)
{
    //init random number generator
    srand(time(NULL));
    //start time
    
    //-------------Graphics Initialization--------//
    
    // initialize GLUT
    glutInit( &argc, argv );
    
    // initialize graphics
    initialize();
    
    
    
    
    
    // load sounds

    mkdir(g_programPathUser.c_str(), 01777);
    mkdir(g_audioPathUser.c_str(), 01777);

    DIR* rep = NULL;
    struct dirent* fichierLu = NULL; /* Declaration of a pointer to the dirent structure. */
    g_audioPath = g_audioPathUser;
    rep = opendir(g_audioPathUser.c_str());

    fichierLu = readdir(rep); /* We read the first directory of the folder (.) */
    fichierLu = readdir(rep); /* We read the second directory of the folder (..) */
    if ((fichierLu = readdir(rep)) == NULL) {
        cout << "No sample in the user directory, loading the default sounds" << endl;
        g_audioPath = g_audioPathDefault;
        }

    AudioFileSet newFileMgr;
    
    if (newFileMgr.loadFileSet(g_audioPath) == 1){
        goto cleanup;
    }
    
    mySounds = newFileMgr.getFileVector();
    cout << "Sounds loaded successfully..." << endl;    
    
    
    
    //create visual representation of sounds    
    soundViews = new vector<SoundRect *>;
    for (int i = 0; i < mySounds->size(); i++)
    {
        soundViews->push_back(new SoundRect());
        soundViews->at(i)->associateSound(mySounds->at(i)->wave,mySounds->at(i)->frames,mySounds->at(i)->channels);
    }
    
    //init grain cloud vector and corresponding view vector
    grainCloud = new vector<GrainCluster *>;
    grainCloudVis = new vector<GrainClusterVis *>;
    
    
    
    //-------------Audio Configuration-----------//
    
    //configure RtAudio
    //create the object
    try {
        theAudio = new MyRtAudio(1,MY_CHANNELS, MY_SRATE, &g_buffSize, MY_FORMAT,true);
    } catch (RtError & err) {
        err.printMessage();
        exit(1);
    }    
    try
    {
        //open audio stream/assign callback
        theAudio->openStream(&audioCallback);
        //get new buffer size
        g_buffSize = theAudio->getBufferSize();
        //start audio stream
        theAudio->startStream();
        //report latency
        theAudio->reportStreamLatency();        
        
    }catch (RtError & err )
    {
        err.printMessage();
        goto cleanup;
    }
    
    
    
    //start graphics
    // let GLUT handle the current thread from here
    glutMainLoop();
    
    cout <<"Something went wrong...shouldn't be here" << endl;
    
    
    //cleanup routine
cleanup:
    cleaningFunction();
    
    // done
    return 0;
    
}

