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

#include "interface/MyGLWindow.h"
#include "interface/MyGLApplication.h"
#include "ui_MyGLWindow.h"
#include "model/Cloud.h"
#include "model/Scene.h"
#include "visual/CloudVis.h"
#include "visual/SampleVis.h"
#include "Frontieres.h"
#include "MyRtOsc.h"
#include <QtFont3D.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QProcess>
#include <QDebug>

extern string g_audioPath;

struct MyGLWindow::Impl {
    Ui::MyGLWindow ui;
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
    P->ui.setupUi(this);
    MyGLScreen *screen = this->screen();

    connect(P->ui.action_Quit, &QAction::triggered,
            qApp, &QApplication::quit);
    connect(P->ui.action_Load_scene, &QAction::triggered,
            this, []() { theApplication->loadSceneFile(); });
    connect(P->ui.action_Save_scene, &QAction::triggered,
            this, []() { theApplication->saveSceneFile(); });
    connect(P->ui.action_Add_sample, &QAction::triggered,
            this, []() { theApplication->addSample(); });
    connect(P->ui.action_Load_clouds_defaults, &QAction::triggered,
            this, []() { theApplication->loadCloudDefaultFile(); });
    connect(P->ui.action_Options, &QAction::triggered,
            this, []() { theApplication->showOptionsDialog(); });
    connect(P->ui.action_Control, &QAction::triggered,
            this, []() { theApplication->showOptionsDialog(); });
    connect(P->ui.action_Combi, &QAction::triggered,
            this, []() { theApplication->showMidiBankDialog(); });
    connect(P->ui.action_Instrument, &QAction::triggered,
            this, []() { theApplication->showMidiInstrumentDialog(); });

    // initial window settings
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setDepthBufferSize(24);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    screen->setFormat(format);
}

void MyGLWindow::closeEvent(QCloseEvent *e)
{
    theApplication->destroyAllCloudDialogs();
}

void MyGLWindow::setMenuBarVisible(bool visible)
{
    P->ui.menubar->setVisible(visible);
}

MyGLScreen *MyGLWindow::screen() const
{
    return P->ui.screen;
}

// the openGL screen
struct MyGLScreen::Impl {
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
    glEnable(GL_POLYGON_SMOOTH);
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
        Scene *scene = ::currentScene;
        SceneSample *selectedSample = scene->selectedSample();
        SceneCloud *selectedCloud = scene->selectedCloud();

        // render rectangles
        for (int i = 0, n = scene->m_samples.size(); i < n; i++) {
            SampleVis &sv = *scene->m_samples[i]->view;
            sv.draw();
        }

        // render clouds if they exist
        for (int i = 0, n = scene->m_clouds.size(); i < n; i++) {
            CloudVis &gv = *scene->m_clouds[i]->view;
            gv.draw();
        }

        // print current param if editing
        if (selectedSample || selectedCloud)
            printParam();
    }
    else {
        printUsage();
    }

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
    Scene *scene = ::currentScene;

    // cout << "button " << button << endl;

    // look for selections if button is down
    if ((button == Qt::LeftButton) || (button == Qt::RightButton)) {

        paramString = "";

        // hide menu
        if (menuFlag == true)
            menuFlag = false;

        scene->deselect(CLOUD);
        // deselect existing selections
        scene->deselect(RECT);
        // exit parameter editing
        currentParam = -1;

        lastDragX = veryHighNumber;
        lastDragY = veryHighNumber;
        // first check clouds to see if we have selection
        for (int i = 0, n = scene->m_clouds.size(); i < n; i++) {
            CloudVis &gv = *scene->m_clouds[i]->view;
            if (gv.select(mouseX, mouseY)) {
                gv.setSelectState(true);
                scene->m_selectedCloud = i;
                break;
            }
        }


        // clear selection buffer
        scene->m_selectionIndices.clear();
        scene->m_selectionIndex = 0;
        // if cloud is not selected - search for rectangle selection
        if (!scene->selectedCloud()) {
            // search for selections
            resizeDir = false;  // set resize direction to horizontal
            for (int i = 0, n = scene->m_samples.size(); i < n; i++) {
                SampleVis &sv = *scene->m_samples[i]->view;
                if (sv.select(mouseX, mouseY) == true) {
                    scene->m_selectionIndices.push_back(i);
                    // sv.setSelectState(true);
                    // selectedRect = i;
                    // break;
                }
            }

            if (!scene->m_selectionIndices.empty()) {
                scene->m_selectedSample = scene->m_selectionIndices[0];
                scene->selectedSample()->view->setSelectState(true);
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

    Scene *scene = ::currentScene;
    SceneSample *selectedSample = scene->selectedSample();
    SceneCloud *selectedCloud = scene->selectedCloud();

    if (selectedCloud) {
        if (selectedCloud->cloud->getLockedState())
            if (selectedCloud->cloud->dialogLocked())
                return;
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

void MyGLScreen::mousePassiveMoveEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
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
}

void MyGLScreen::keyPressEvent(QKeyEvent *event)
{
    static const float sidewaysMoveSpeed = 10.0f;
    static const float upDownMoveSpeed = 10.0f;

    // static const bool negativeFlag = false;  // for negative value entry
    int modkey = event->modifiers();

    Scene *scene = ::currentScene;
    SceneSample *selectedSample = scene->selectedSample();
    SceneCloud *selectedCloud = scene->selectedCloud();

    if (selectedCloud and selectedCloud->cloud->getLockedState())
        if ((event->key() != Qt::Key_Escape)
                & (event->key() != Qt::Key_P)
                & (event->key() != Qt::Key_Slash))
            if (selectedCloud->cloud->dialogLocked())
                return;

    switch (event->key()) {

    case Qt::Key_Tab:  // tab key
        if (scene->m_selectionIndices.size() > 1) {
            selectedSample->view->setSelectState(false);
            scene->m_selectionIndex++;
            if (scene->m_selectionIndex >= scene->m_selectionIndices.size()) {
                scene->m_selectionIndex = 0;
            }
            scene->m_selectedSample = scene->m_selectionIndices[scene->m_selectionIndex];
            selectedSample = scene->selectedSample();
            selectedSample->view->setSelectState(true);
        }
        break;
    case Qt::Key_1:
        paramString.push_back('1');
        if (currentParam == WINDOW) {
            if (selectedCloud) {
                selectedCloud->cloud->setWindowType(0);
            }
        }

        break;
    case Qt::Key_2:
        paramString.push_back('2');
        if (currentParam == WINDOW) {
            if (selectedCloud) {
                selectedCloud->cloud->setWindowType(1);
            }
        }
        break;
    case Qt::Key_3:
        paramString.push_back('3');
        if (currentParam == WINDOW) {
            if (selectedCloud) {
                selectedCloud->cloud->setWindowType(2);
            }
        }
        break;

    case Qt::Key_4:
        paramString.push_back('4');
        if (currentParam == WINDOW) {
            if (selectedCloud) {
                selectedCloud->cloud->setWindowType(3);
            }
        }
        break;
    case Qt::Key_5:
        paramString.push_back('5');
        if (currentParam == WINDOW) {
            if (selectedCloud) {
                selectedCloud->cloud->setWindowType(4);
            }
        }
        break;
    case Qt::Key_6:
        paramString.push_back('6');
        if (currentParam == WINDOW) {
            if (selectedCloud) {
                selectedCloud->cloud->setWindowType(5);
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
                if (selectedCloud) {
                    if (value < 1.0) {
                        value = 1.0;
                    }
                    selectedCloud->cloud->setDurationMs(value);
                }
                break;
            case OVERLAP:
                if (selectedCloud) {
                    selectedCloud->cloud->setOverlap(value);
                }
                break;
            case PITCH:
                if (selectedCloud) {
                    selectedCloud->cloud->setPitch(value);
                }
                break;
            case P_LFO_FREQ:
                if (selectedCloud) {
                    selectedCloud->cloud->setPitchLFOFreq(value);
                }
                break;
            case P_LFO_AMT:
                if (selectedCloud) {
                    selectedCloud->cloud->setPitchLFOAmount(value);
                }
                break;
            case VOLUME:
                if (selectedCloud) {
                    selectedCloud->cloud->setVolumeDb(value);
                }
            default:
                break;
            }
            paramString = "";
        }
        break;


    case Qt::Key_Escape:  // esc key
        cleaningFunction();
        exit(1);
        break;

    case Qt::Key_Q:  // spatialization
        break;

    case Qt::Key_O: {
        MyGLWindow *win = static_cast<MyGLWindow *>(window());
        Qt::WindowStates state = win->windowState() ^ Qt::WindowFullScreen;
        win->setWindowState(state);
        win->setMenuBarVisible(!(state & Qt::WindowFullScreen));
        break;
    }

    case Qt::Key_T:
        paramString = "";
        if (selectedCloud) {
            if (currentParam != SPATIALIZE) {
                currentParam = SPATIALIZE;
            }
            else {
                if (modkey == Qt::ShiftModifier) {
                    if (selectedCloud) {
                        int theSpat = selectedCloud->cloud->getSpatialMode();
                        selectedCloud->cloud->setSpatialMode(theSpat - 1, -1);
                    }
                }
                else {
                    if (selectedCloud) {
                        int theSpat = selectedCloud->cloud->getSpatialMode();
                        selectedCloud->cloud->setSpatialMode(theSpat + 1, -1);
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
                if (selectedCloud) {
                    float theOver = selectedCloud->cloud->getOverlap();
                    selectedCloud->cloud->setOverlap(theOver - 0.01f);
                }
            }
            else {
                if (selectedCloud) {
                    float theOver = selectedCloud->cloud->getOverlap();
                    selectedCloud->cloud->setOverlap(theOver + 0.01f);
                }
            }
        }
        break;
    case Qt::Key_R:
        if (selectedCloud) {
            currentParam = MOTIONXY;
        }
        // toggle selection modes
        dragMode = RESIZE;
        break;
    case Qt::Key_F:  // direction
        paramString = "";
        if (selectedCloud) {
            if (currentParam != DIRECTION) {
                currentParam = DIRECTION;
            }
            else {
                if (modkey == Qt::ShiftModifier) {
                    if (selectedCloud) {
                        int theDir = selectedCloud->cloud->getDirection();
                        selectedCloud->cloud->setDirection(theDir - 1);
                    }
                }
                else {
                    if (selectedCloud) {
                        int theDir = selectedCloud->cloud->getDirection();
                        selectedCloud->cloud->setDirection(theDir + 1);
                    }
                }
            }
        }
        if (selectedSample) {
            selectedSample->view->toggleOrientation();
        }
        break;
//    case Qt::Key_P:  // waveform display on/off
//        break;
    case Qt::Key_W:  // window editing for grain
        paramString = "";
        if (currentParam != WINDOW) {
            currentParam = WINDOW;
        }
        else {
            if (modkey == Qt::ShiftModifier) {
                if (selectedCloud) {
                    int theWin = selectedCloud->cloud->getWindowType();
                    selectedCloud->cloud->setWindowType(theWin - 1);
                }
            }
            else {
                if (selectedCloud) {
                    int theWin = selectedCloud->cloud->getWindowType();
                    selectedCloud->cloud->setWindowType(theWin + 1);
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
                if (selectedCloud) {
                    float theVol = selectedCloud->cloud->getVolumeDb();
                    selectedCloud->cloud->setVolumeDb(theVol - 0.5f);
                }
            }
            else {
                if (selectedCloud) {
                    float theVol = selectedCloud->cloud->getVolumeDb();
                    selectedCloud->cloud->setVolumeDb(theVol + 0.5f);
                }
            }
        }


        break;

    case Qt::Key_Slash:  // switch to menu view
    case Qt::Key_Question:
        menuFlag = !menuFlag;

        break;
    case Qt::Key_G: {
        paramString = "";
        scene->deselect(RECT);
        std::lock_guard<std::mutex> lock(::currentSceneMutex);
        if (modkey == Qt::ShiftModifier) {
            if (!scene->m_clouds.empty()) {
                scene->m_clouds.pop_back();
                // cout << "cloud removed" << endl;
            }
            if (scene->m_clouds.empty()) {
                scene->m_selectedCloud = -1;
            }
            else {
                // still have a cloud so select
                scene->m_selectedCloud = scene->m_clouds.size() - 1;
                scene->m_clouds.back()->view->setSelectState(true);
            }
            break;
        }
        else {
            int numGrains = 8;  // initial number of grains
            if (selectedCloud) {
                if (!scene->m_clouds.empty()) {
                    selectedCloud->view->setSelectState(false);
                }
            }
            scene->addNewCloud(numGrains);
            scene->m_selectedCloud = scene->m_clouds.size() - 1;
        }
        break;
    }
    case Qt::Key_V:  // grains (add, delete)
        paramString = "";
        if (currentParam != NUMGRAINS) {
            currentParam = NUMGRAINS;
        }
        else {
            if (modkey == Qt::ShiftModifier) {
                if (selectedCloud) {
                    selectedCloud->cloud->removeGrain();
                }
            }
            else {
                if (selectedCloud) {
                    selectedCloud->cloud->addGrain();
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
                if (selectedCloud) {
                    float theDur = selectedCloud->cloud->getDurationMs();
                    selectedCloud->cloud->setDurationMs(theDur - 5.0f);
                }
            }
            else {
                if (selectedCloud) {
                    float theDur = selectedCloud->cloud->getDurationMs();
                    selectedCloud->cloud->setDurationMs(theDur + 5.0f);
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
                if (selectedCloud) {
                    float theLFOFreq = selectedCloud->cloud->getPitchLFOFreq();
                    selectedCloud->cloud->setPitchLFOFreq(theLFOFreq - 0.01f);
                }
            }
            else {
                if (selectedCloud) {
                    float theLFOFreq = selectedCloud->cloud->getPitchLFOFreq();
                    selectedCloud->cloud->setPitchLFOFreq(theLFOFreq + 0.01f);
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
                if (selectedCloud) {
                    float theLFOAmt = selectedCloud->cloud->getPitchLFOAmount();
                    selectedCloud->cloud->setPitchLFOAmount(theLFOAmt - 0.001f);
                }
            }
            else {
                if (selectedCloud) {
                    float theLFOAmnt = selectedCloud->cloud->getPitchLFOAmount();
                    selectedCloud->cloud->setPitchLFOAmount(theLFOAmnt + 0.001f);
                }
            }
        }
        break;

    case Qt::Key_X:
        paramString = "";
        if (selectedCloud) {
            currentParam = MOTIONX;
        }
        break;
    case Qt::Key_Y:
        paramString = "";
        if (selectedCloud) {
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
                if (selectedCloud) {
                    float thePitch = selectedCloud->cloud->getPitch();
                    selectedCloud->cloud->setPitch(thePitch - 0.01f);
                }
            }
            else {
                if (selectedCloud) {
                    float thePitch = selectedCloud->cloud->getPitch();
                    selectedCloud->cloud->setPitch(thePitch + 0.01f);
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
            if (selectedCloud) {
                theApplication->destroyCloudDialog(selectedCloud->cloud->getId());
                std::lock_guard<std::mutex> lock(::currentSceneMutex);
                scene->m_clouds.erase(scene->m_clouds.begin() + scene->m_selectedCloud);
                scene->m_selectedCloud = -1;
            }
            else if (selectedSample) {
                std::lock_guard<std::mutex> lock(::currentSceneMutex);
                scene->removeSampleAt(scene->m_selectedSample);
                scene->m_selectedSample = -1;
            }
        }
        else {
            if (paramString.size() > 0)
                paramString.resize(paramString.size() - 1);
        }

        break;
    case Qt::Key_A:
        paramString = "";
        if (selectedCloud) {
            selectedCloud->cloud->toggleActive();
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
        theApplication->saveSceneFile();
        break;
    }
    case Qt::Key_N: {
        // see name of cloud or sample
        if (selectedSample) {
            currentParam = NAME;
        }
        else {
            paramString = "";
            if (currentParam != NUM)
                currentParam = NUM;
        }
        break;
    }
    case Qt::Key_C: {
        // record cloud parameters
        if (selectedCloud) {
            theApplication->saveCloudFile(selectedCloud);
        }
        break;
    }
    case Qt::Key_E: {
        // volume envelope
        if (selectedCloud) {
            theApplication->showDialogVolumeEnvelope(selectedCloud);
        }
        break;
    }
    case Qt::Key_P: {
        // all parameters in a separate window
        if (selectedCloud) {
           theApplication->showCloudDialog(selectedCloud);
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
