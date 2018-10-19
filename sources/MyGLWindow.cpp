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

#include "MyGLWindow.h"
#include "Frontieres.h"
#include "SoundRect.h"
#include "GrainCluster.h"
#include "Scene.h"
#include <QtFont3D.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>

extern string g_audioPath;

struct MyGLWindow::Impl {
    MyGLScreen *screen = nullptr;
};

MyGLWindow::MyGLWindow()
    : P(new Impl)
{
}

MyGLWindow::~MyGLWindow()
{
}

void MyGLWindow::initialize()
{
    MyGLScreen *screen = new MyGLScreen;
    P->screen = screen;

    resize(800, 600);
    setWindowTitle(u8"Frontières");
    setCentralWidget(screen);

    // initial window settings
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setDepthBufferSize(24);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    screen->setFormat(format);
}

MyGLScreen *MyGLWindow::screen() const
{
    return P->screen;
}

// the openGL screen
struct MyGLScreen::Impl {
    Scene sceneCurrent;
};

MyGLScreen::MyGLScreen(QWidget *parent)
    : QOpenGLWidget(parent), P(new Impl)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

MyGLScreen::~MyGLScreen()
{
}

Scene &MyGLScreen::currentScene()
{
    return P->sceneCurrent;
}

void MyGLScreen::initializeGL()
{
    float glyphThickness = 1;
    int numGlyphs = 256;  // glyph space for accented characters
    QFont font("Monospace", text_fontSize, QFont::Normal);
    font.setStyleHint(QFont::Monospace);
    text_renderer->setFont(font, glyphThickness, numGlyphs);

    // initial state
    glClearColor(0.15, 0.15, 0.15, 1.0f);
    // enable depth buffer updates
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // set polys with counterclockwise winding to be front facing
    // this is gl default anyway
    glFrontFace(GL_CCW);
    // set fill mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON_SMOOTH);
    // enable transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // antialias lines and points
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void MyGLScreen::paintGL()
{
    // clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0);

    ////PUSH //save current transform
    glPushMatrix();

    glLoadIdentity();

    // update viewer position
    glTranslatef(-position.x, -position.y, -position.z);  // translate the screen to the position of our camera
    if (menuFlag == false) {
        // render rectangles
        for (int i = 0; i < soundViews.size(); i++) {
            soundViews[i]->draw();
        }

        // render grain clouds if they exist
        for (int i = 0; i < grainCloudVis.size(); i++) {
            grainCloudVis[i]->draw();
        }

        // print current param if editing
        if ((selectedCloud >= 0) || (selectedRect >= 0))
            printParam();
    }
    else {
        printUsage();
    }


    // printUsage();

    // POP ---//restore state
    glPopMatrix();

    // flush and swap
    glFlush();  // renders and empties buffers
}

void MyGLScreen::resizeGL(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)w, 0.0, (GLdouble)h, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-position.x, -position.y, -position.z);  // translate the screen to the position of our camera
}

void MyGLScreen::mousePressEvent(QMouseEvent *event)
{
    Qt::MouseButton button = event->button();

    // cout << "button " << button << endl;

    // look for selections if button is down
    if ((button == Qt::LeftButton) || (button == Qt::RightButton)) {

        paramString = "";

        // hide menu
        if (menuFlag == true)
            menuFlag = false;

        deselect(CLOUD);
        // deselect existing selections
        deselect(RECT);
        // exit parameter editing
        currentParam = -1;

        lastDragX = veryHighNumber;
        lastDragY = veryHighNumber;
        // first check grain clouds to see if we have selection
        for (int i = 0; i < grainCloudVis.size(); i++) {
            if (grainCloudVis[i]->select(mouseX, mouseY) == true) {
                grainCloudVis[i]->setSelectState(true);
                selectedCloud = i;
                break;
            }
        }


        // clear selection buffer
        selectionIndices.clear();
        // allocate new buffer
        selectionIndex = 0;
        // if grain cloud is not selected - search for rectangle selection
        if (selectedCloud < 0) {
            // search for selections
            resizeDir = false;  // set resize direction to horizontal
            for (int i = 0; i < soundViews.size(); i++) {
                if (soundViews[i]->select(mouseX, mouseY) == true) {
                    selectionIndices.push_back(i);
                    // soundViews->at(i)->setSelectState(true);
                    // selectedRect = i;
                    // break;
                }
            }

            if (selectionIndices.size() > 0) {
                selectedRect = selectionIndices[0];
                soundViews[selectedRect]->setSelectState(true);
            }
        }
    }
}

void MyGLScreen::mouseReleaseEvent(QMouseEvent *event)
{
    //handle button up
    // if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)){
    //     lastDragX = -1;
    //     lastDragY = -1;
    //     dragMode = MOVE;
    // }
}

void MyGLScreen::mouseMoveEvent(QMouseEvent *event)
{
    if (!event->buttons())
        return mousePassiveMoveEvent(event);

    // update mouse coordinates based on drag position
    int x = event->x();
    int y = event->y();
    updateMouseCoords(x, y);
    int xDiff = 0;
    int yDiff = 0;

    if (selectedCloud >= 0) {
        grainCloudVis[selectedCloud]->updateCloudPosition(mouseX, mouseY);
    }
    else {

        switch (dragMode) {
        case MOVE:
            if ((lastDragX != veryHighNumber) && (lastDragY != veryHighNumber)) {

                if (selectedRect >= 0) {  // movement case
                    soundViews[selectedRect]->move(mouseX - lastDragX, mouseY - lastDragY);
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
                    float newWidth = soundViews[selectedRect]->getWidth();
                    float newHeight = soundViews[selectedRect]->getHeight();

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
                    soundViews[selectedRect]->setWidthHeight(newWidth, newHeight);
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

void MyGLScreen::mousePassiveMoveEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    updateMouseCoords(x, y);


    if (selectedCloud >= 0) {
        switch (currentParam) {
        case MOTIONX:
            grainCloudVis[selectedCloud]->setXRandExtent(mouseX);
            break;
        case MOTIONY:
            grainCloudVis[selectedCloud]->setYRandExtent(mouseY);
            break;
        case MOTIONXY:
            grainCloudVis[selectedCloud]->setRandExtent(mouseX, mouseY);
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
    //    for (int i = 0; i < soundViews.size(); i++)
    //        soundViews[i]->procMovement(x, y);
    //
}

void MyGLScreen::keyPressEvent(QKeyEvent *event)
{
    //qDebug() << event;

    static const float sidewaysMoveSpeed = 10.0f;
    static const float upDownMoveSpeed = 10.0f;

    // static const bool negativeFlag = false;  // for negative value entry
    int modkey = event->modifiers();

    switch (event->key()) {

    case Qt::Key_Tab:  // tab key

        if (selectionIndices.size() > 1) {
            soundViews[selectedRect]->setSelectState(false);
            selectionIndex++;
            if (selectionIndex >= selectionIndices.size()) {
                selectionIndex = 0;
            }
            selectedRect = selectionIndices[selectionIndex];
            soundViews[selectedRect]->setSelectState(true);
        }
        break;
    case Qt::Key_1:
        paramString.push_back('1');
        if (currentParam == WINDOW) {
            if (selectedCloud >= 0) {
                grainCloud[selectedCloud]->setWindowType(0);
            }
        }

        break;
    case Qt::Key_2:
        paramString.push_back('2');
        if (currentParam == WINDOW) {
            if (selectedCloud >= 0) {
                grainCloud[selectedCloud]->setWindowType(1);
            }
        }
        break;
    case Qt::Key_3:
        paramString.push_back('3');
        if (currentParam == WINDOW) {
            if (selectedCloud >= 0) {
                grainCloud[selectedCloud]->setWindowType(2);
            }
        }
        break;

    case Qt::Key_4:
        paramString.push_back('4');
        if (currentParam == WINDOW) {
            if (selectedCloud >= 0) {
                grainCloud[selectedCloud]->setWindowType(3);
            }
        }
        break;
    case Qt::Key_5:
        paramString.push_back('5');
        if (currentParam == WINDOW) {
            if (selectedCloud >= 0) {
                grainCloud[selectedCloud]->setWindowType(4);
            }
        }
        break;
    case Qt::Key_6:
        paramString.push_back('6');
        if (currentParam == WINDOW) {
            if (selectedCloud >= 0) {
                grainCloud[selectedCloud]->setWindowType(5);
            }
        }
        break;
    case Qt::Key_7:
        paramString.push_back('7');
        break;
    case Qt::Key_8:
        paramString.push_back('8');
        break;
    case Qt::Key_9:
        paramString.push_back('9');
        break;
    case Qt::Key_0:
        paramString.push_back('0');
        break;
    case Qt::Key_Period:
        paramString.push_back('.');
        break;

    case Qt::Key_Return:  // enter key - for saving param string
        if (paramString != "") {
            float value = atof(paramString.c_str());

            // cout << "value received " << value << endl;
            switch (currentParam) {
            case DURATION:
                if (selectedCloud >= 0) {
                    if (value < 1.0) {
                        value = 1.0;
                    }
                    grainCloud[selectedCloud]->setDurationMs(value);
                }
                break;
            case OVERLAP:
                if (selectedCloud >= 0) {
                    grainCloud[selectedCloud]->setOverlap(value);
                }
                break;
            case PITCH:
                if (selectedCloud >= 0) {
                    grainCloud[selectedCloud]->setPitch(value);
                }
                break;
            case P_LFO_FREQ:
                if (selectedCloud >= 0) {
                    grainCloud[selectedCloud]->setPitchLFOFreq(value);
                }
                break;
            case P_LFO_AMT:
                if (selectedCloud >= 0) {
                    grainCloud[selectedCloud]->setPitchLFOAmount(value);
                }
                break;

            case VOLUME:
                if (selectedCloud >= 0) {
                    grainCloud[selectedCloud]->setVolumeDb(value);
                }
            default:
                break;
            }
            paramString = "";
        }
        // cout << "enter key caught" << endl;
        break;


    case Qt::Key_Escape:  // esc key
        cleaningFunction();
        exit(1);
        break;

    case Qt::Key_Q:  // spatialization
        break;

    case Qt::Key_O: {
        QWidget *win = window();
        win->setWindowState(win->windowState() ^ Qt::WindowFullScreen);
        break;
    }

    case Qt::Key_T:
        paramString = "";
        if (selectedCloud >= 0) {
            if (currentParam != SPATIALIZE) {
                currentParam = SPATIALIZE;
            }
            else {
                if (modkey == Qt::ShiftModifier) {
                    if (selectedCloud >= 0) {
                        int theSpat = grainCloud[selectedCloud]->getSpatialMode();
                        grainCloud[selectedCloud]->setSpatialMode(theSpat - 1, -1);
                    }
                }
                else {
                    if (selectedCloud >= 0) {
                        int theSpat = grainCloud[selectedCloud]->getSpatialMode();
                        grainCloud[selectedCloud]->setSpatialMode(theSpat + 1, -1);
                    }
                }
            }
        }
        break;

    case Qt::Key_S:  // overlap control
        paramString = "";
        if (currentParam != OVERLAP) {
            currentParam = OVERLAP;
        }
        else {
            if (modkey == Qt::ShiftModifier) {
                if (selectedCloud >= 0) {
                    float theOver = grainCloud[selectedCloud]->getOverlap();
                    grainCloud[selectedCloud]->setOverlap(theOver - 0.01f);
                }
            }
            else {
                if (selectedCloud >= 0) {
                    float theOver = grainCloud[selectedCloud]->getOverlap();
                    grainCloud[selectedCloud]->setOverlap(theOver + 0.01f);
                }
            }
        }
        break;
    case Qt::Key_R:
        if (selectedCloud >= 0) {
            currentParam = MOTIONXY;
        }
        // toggle selection modes
        dragMode = RESIZE;
        break;
    case Qt::Key_F:  // direction
        paramString = "";
        if (selectedCloud >= 0) {
            if (currentParam != DIRECTION) {
                currentParam = DIRECTION;
            }
            else {
                if (modkey == Qt::ShiftModifier) {
                    if (selectedCloud >= 0) {
                        int theDir = grainCloud[selectedCloud]->getDirection();
                        grainCloud[selectedCloud]->setDirection(theDir - 1);
                    }
                }
                else {
                    if (selectedCloud >= 0) {
                        int theDir = grainCloud[selectedCloud]->getDirection();
                        grainCloud[selectedCloud]->setDirection(theDir + 1);
                    }
                }
            }
        }
        if (selectedRect >= 0) {
            soundViews[selectedRect]->toggleOrientation();
        }
        // cerr << "Looking from the front" << endl;
        break;
    case Qt::Key_P:  // waveform display on/off

        //            for (int i = 0; i < soundViews.size();i++){
        //                soundViews[i]->toggleWaveDisplay();
        //            }
        break;
    case Qt::Key_W:  // window editing for grain
        paramString = "";
        if (currentParam != WINDOW) {
            currentParam = WINDOW;
        }
        else {
            if (modkey == Qt::ShiftModifier) {
                if (selectedCloud >= 0) {
                    int theWin = grainCloud[selectedCloud]->getWindowType();
                    grainCloud[selectedCloud]->setWindowType(theWin - 1);
                }
            }
            else {
                if (selectedCloud >= 0) {
                    int theWin = grainCloud[selectedCloud]->getWindowType();
                    grainCloud[selectedCloud]->setWindowType(theWin + 1);
                }
            }
        }

        break;

    case Qt::Key_B:
        // cloud volume
        paramString = "";
        if (currentParam != VOLUME) {
            currentParam = VOLUME;
        }
        else {
            if (modkey == Qt::ShiftModifier) {
                if (selectedCloud >= 0) {
                    float theVol = grainCloud[selectedCloud]->getVolumeDb();
                    grainCloud[selectedCloud]->setVolumeDb(theVol - 0.5f);
                }
            }
            else {
                if (selectedCloud >= 0) {
                    float theVol = grainCloud[selectedCloud]->getVolumeDb();
                    grainCloud[selectedCloud]->setVolumeDb(theVol + 0.5f);
                }
            }
        }


        break;

    case Qt::Key_Slash:  // switch to menu view
    case Qt::Key_Question:
        menuFlag = !menuFlag;

        break;
    case Qt::Key_G:
        paramString = "";
        deselect(RECT);
        if (modkey == Qt::ShiftModifier) {
            if (grainCloud.size() > 0) {
                grainCloud.pop_back();
                grainCloudVis.pop_back();
                numClouds -= 1;
                // cout << "cloud removed" << endl;
            }
            if (numClouds == 0) {
                selectedCloud = -1;
            }
            else {
                // still have a cloud so select
                selectedCloud = numClouds - 1;
                grainCloudVis[selectedCloud]->setSelectState(true);
            }
            break;
        }
        else {
            int numVoices = 8;  // initial number of voices
            int idx = grainCloud.size();
            if (selectedCloud >= 0) {
                if (numClouds > 0) {
                    grainCloudVis[selectedCloud]->setSelectState(false);
                }
            }
            selectedCloud = idx;
            // create audio
            grainCloud.push_back(new GrainCluster(mySounds, numVoices));
            // create visualization
            grainCloudVis.push_back(
                new GrainClusterVis(mouseX, mouseY, numVoices, &soundViews));
            // select new cloud
            grainCloudVis.at(idx)->setSelectState(true);
            // register visualization with audio
            grainCloud.at(idx)->registerVis(grainCloudVis.at(idx));
            // grainCloud->at(idx)->toggleActive();
            numClouds += 1;
        }
        //                        cout << "cloud added" << endl;
        // grainControl->newCluster(mouseX,mouseY,1);

        break;
    case Qt::Key_V:  // grain voices (add, delete)
        paramString = "";
        if (currentParam != NUMGRAINS) {
            currentParam = NUMGRAINS;
        }
        else {
            if (modkey == Qt::ShiftModifier) {
                if (selectedCloud >= 0) {
                    grainCloud[selectedCloud]->removeGrain();
                    // cout << "grain removed" << endl;
                }
            }
            else {
                if (selectedCloud >= 0) {
                    grainCloud[selectedCloud]->addGrain();
                    // cout << "grain added" << endl;
                }
            }
        }
        break;

    case Qt::Key_D:
        paramString = "";
        if (currentParam != DURATION) {
            currentParam = DURATION;
        }
        else {
            if (modkey == Qt::ShiftModifier) {
                if (selectedCloud >= 0) {
                    float theDur = grainCloud[selectedCloud]->getDurationMs();
                    grainCloud[selectedCloud]->setDurationMs(theDur - 5.0f);
                }
            }
            else {
                if (selectedCloud >= 0) {
                    float theDur = grainCloud[selectedCloud]->getDurationMs();
                    grainCloud[selectedCloud]->setDurationMs(theDur + 5.0f);
                }
            }
        }
        break;

    case Qt::Key_L:
        paramString = "";
        if (currentParam != P_LFO_FREQ) {
            currentParam = P_LFO_FREQ;
        }
        else {
            if (modkey == Qt::ShiftModifier) {
                if (selectedCloud >= 0) {
                    float theLFOFreq = grainCloud[selectedCloud]->getPitchLFOFreq();
                    grainCloud[selectedCloud]->setPitchLFOFreq(theLFOFreq - 0.01f);
                }
            }
            else {
                if (selectedCloud >= 0) {
                    float theLFOFreq = grainCloud[selectedCloud]->getPitchLFOFreq();
                    grainCloud[selectedCloud]->setPitchLFOFreq(theLFOFreq + 0.01f);
                }
            }
        }
        break;

    case Qt::Key_K:
        paramString = "";
        if (currentParam != P_LFO_AMT) {
            currentParam = P_LFO_AMT;
        }
        else {
            if (modkey == Qt::ShiftModifier) {
                if (selectedCloud >= 0) {
                    float theLFOAmt = grainCloud[selectedCloud]->getPitchLFOAmount();
                    grainCloud[selectedCloud]->setPitchLFOAmount(theLFOAmt - 0.001f);
                }
            }
            else {
                if (selectedCloud >= 0) {
                    float theLFOAmnt = grainCloud[selectedCloud]->getPitchLFOAmount();
                    grainCloud[selectedCloud]->setPitchLFOAmount(theLFOAmnt + 0.001f);
                }
            }
        }
        break;

    case Qt::Key_X:
        paramString = "";
        if (selectedCloud >= 0) {
            currentParam = MOTIONX;
        }
        break;
    case Qt::Key_Y:
        paramString = "";
        if (selectedCloud >= 0) {
            currentParam = MOTIONY;
        }
        break;

    case Qt::Key_Z:
        paramString = "";
        if (currentParam != PITCH) {
            currentParam = PITCH;
        }
        else {
            if (modkey == Qt::ShiftModifier) {
                if (selectedCloud >= 0) {
                    float thePitch = grainCloud[selectedCloud]->getPitch();
                    grainCloud[selectedCloud]->setPitch(thePitch - 0.01f);
                }
            }
            else {
                if (selectedCloud >= 0) {
                    float thePitch = grainCloud[selectedCloud]->getPitch();
                    grainCloud[selectedCloud]->setPitch(thePitch + 0.01f);
                }
            }
        }
        break;

    case Qt::Key_Minus:
    case Qt::Key_Underscore:
        paramString.insert(0, "-");
        break;
    case Qt::Key_Delete:  // delete selected
        if (paramString == "") {
            if (selectedCloud >= 0) {
                grainCloud.erase(grainCloud.begin() + selectedCloud);
                grainCloudVis.erase(grainCloudVis.begin() + selectedCloud);
                selectedCloud = -1;
                numClouds -= 1;
            }
        }
        else {
            if (paramString.size() > 0)
                paramString.resize(paramString.size() - 1);
        }

        break;
    case Qt::Key_A:
        paramString = "";
        if (selectedCloud >= 0) {
            grainCloud[selectedCloud]->toggleActive();
        }
        break;

    case Qt::Key_Left:
        // move to the left
        position.x -= sidewaysMoveSpeed;
        mouseX -= sidewaysMoveSpeed;
        break;
    case Qt::Key_Right:
        // move to the right
        position.x += sidewaysMoveSpeed;
        mouseX += sidewaysMoveSpeed;
        break;
    case Qt::Key_Down:
        // move backward
        position.y -= upDownMoveSpeed;
        mouseY += sidewaysMoveSpeed;
        break;
    case Qt::Key_Up:
        // move forward
        position.y += upDownMoveSpeed;
        mouseY -= sidewaysMoveSpeed;
        break;
    case Qt::Key_M: {
        // record scene
        Scene &scene = P->sceneCurrent;
        string nameSceneFile = scene.askNameScene(FileDirection::Save);
        if (!nameSceneFile.empty()) {
            QFile sceneFile(QString::fromStdString(nameSceneFile));
            scene.save(sceneFile);
        }
        break;
    }

    default:
        break;
    }

    update();
}

void MyGLScreen::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_R:
        dragMode = MOVE;
        lastDragX = veryHighNumber;
        lastDragY = veryHighNumber;
        break;
    default:
        break;
    }

    update();
}
