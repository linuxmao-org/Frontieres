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
#include "interface/MonitorWidget.h"
#include "ui_MyGLWindow.h"
#include "ui_AboutDialog.h"
#include "model/Cloud.h"
#include "model/Scene.h"
#include "visual/CloudVis.h"
#include "visual/SampleVis.h"
#include "visual/Circular.h"
#include "visual/Hypotrochoid.h"
#include "visual/Recorded.h"
#include "model/ParamCloud.h"
#include "utility/GTime.h"
#include "Frontieres.h"
#include "MyRtOsc.h"
#include <chrono>
#include <QtFont3D.h>
#include <QTextBrowser>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QProcess>
#include <QDebug>

extern string g_audioPath;
extern CloudParams g_defaultCloudParams;

struct MyGLWindow::Impl {
    MyGLWindow *Q = nullptr;
    Ui::MyGLWindow ui;
    MonitorWidget *dspMonitor_ = nullptr;
    void setupDspMonitor();
};

MyGLWindow::MyGLWindow()
    : P(new Impl)
{
    P->Q = this;
}

MyGLWindow::~MyGLWindow()
{
}

void MyGLWindow::initialize()
{
    P->ui.setupUi(this);
    P->setupDspMonitor();

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

    connect(P->ui.actionCloud, &QAction::triggered,
            screen, &MyGLScreen::keyAction_Cloud);
    connect(P->ui.actionGrain, &QAction::triggered,
            screen, &MyGLScreen::keyAction_Grain);
    connect(P->ui.actionXmotion, &QAction::triggered,
            screen, &MyGLScreen::keyAction_MotionX);
    connect(P->ui.actionYmotion, &QAction::triggered,
            screen, &MyGLScreen::keyAction_MotionY);
    connect(P->ui.actionXYmotion, &QAction::triggered,
            screen, &MyGLScreen::keyAction_MotionXY);
    connect(P->ui.actionTrajectory, &QAction::triggered,
            screen, &MyGLScreen::keyAction_Trajectory);
    connect(P->ui.actionSpatialMode, &QAction::triggered,
            screen, &MyGLScreen::keyAction_SpatialMode);
    connect(P->ui.actionOverlap, &QAction::triggered,
            screen, &MyGLScreen::keyAction_Overlap);
    connect(P->ui.actionDirection, &QAction::triggered,
            screen, &MyGLScreen::keyAction_Direction);
    connect(P->ui.actionWindow, &QAction::triggered,
            screen, &MyGLScreen::keyAction_Window);
    connect(P->ui.actionVolume, &QAction::triggered,
            screen, &MyGLScreen::keyAction_Volume);
    connect(P->ui.actionDuration, &QAction::triggered,
            screen, &MyGLScreen::keyAction_Duration);
    connect(P->ui.actionLfoFrequency, &QAction::triggered,
            screen, &MyGLScreen::keyAction_LfoFreq);
    connect(P->ui.actionLfoAmount, &QAction::triggered,
            screen, &MyGLScreen::keyAction_LfoAmount);
    connect(P->ui.actionPitch, &QAction::triggered,
            screen, &MyGLScreen::keyAction_Pitch);
    connect(P->ui.actionActive, &QAction::triggered,
            screen, &MyGLScreen::keyAction_Active);
    connect(P->ui.actionEditEnvelope, &QAction::triggered,
            screen, &MyGLScreen::keyAction_EditEnvelope);
    connect(P->ui.actionEditCloud, &QAction::triggered,
            screen, &MyGLScreen::keyAction_EditCloud);
    connect(P->ui.actionSampleNames, &QAction::triggered,
            screen, &MyGLScreen::keyAction_SampleNames);
    connect(P->ui.actionFullScreen, &QAction::triggered,
            screen, &MyGLScreen::keyAction_FullScreen);

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

void MyGLWindow::on_actionAbout_triggered()
{
    QDialog dlg(this);
    Ui::AboutDialog ui;
    ui.setupUi(&dlg);
    dlg.exec();
}

void MyGLWindow::on_actionUserManual_triggered()
{
    QDialog dlg(this);

    // build dialog with text browser inside
    dlg.setWindowTitle(tr("User manual"));
    QVBoxLayout *vl = new QVBoxLayout;
    dlg.setLayout(vl);
    vl->setContentsMargins(0, 0, 0, 0);
    QTextBrowser *tb = new QTextBrowser;
    vl->addWidget(tb);

    // can visit external web pages
    tb->setOpenExternalLinks(true);

    // find manual according to language
    QLocale loc = locale();
    QString locName = loc.name();

    // search files in order: (ex.) "fr_FR", "fr", "en"
    QStringList paths;
    QString pathTmpl = ":/docs/manual/%1/index.html";
    paths << pathTmpl.arg(locName);
    { int i = locName.indexOf('_');
      if (i != -1) paths << pathTmpl.arg(locName.left(i)); }
    paths << pathTmpl.arg("en");

    // load the first one existing
    for (const QString &path : paths) {
        QFile docsFile(path);
        if (docsFile.open(QFile::ReadOnly)) {
            tb->setHtml(QString::fromUtf8(docsFile.readAll()));
            break;
        }
    }

    dlg.resize(600, 800);
    dlg.exec();
}

void MyGLWindow::Impl::setupDspMonitor()
{
    QMenuBar *mb = Q->menuBar();
    int h = mb->height();

    QWidget *w = new QWidget;
    mb->setCornerWidget(w);
    w->setFixedHeight(h);

    QVBoxLayout *vl = new QVBoxLayout;
    w->setLayout(vl);
    vl->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *hl  = new QHBoxLayout;
    vl->addLayout(hl);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->addWidget(new QLabel(tr("DSP%")));

    MonitorWidget *mon = new MonitorWidget;
    hl->addWidget(mon);
    mon->setMinimumWidth(50);

    dspMonitor_ = mon;

    QTimer *tm = new QTimer(Q);
    QObject::connect(tm, &QTimer::timeout, Q,
                     [mon]() { mon->feedSample(dspMonitorValue); });
    tm->start(300);
}

// the openGL screen
struct MyGLScreen::Impl {
    unsigned framesToSkip = 0;
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

bool MyGLScreen::advanceFrame()
{
    if (P->framesToSkip > 0) {  // skip the next frame ?
        --P->framesToSkip;
        return false;
    }
    return true;
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
    glEnable(GL_MULTISAMPLE_ARB);
}

void MyGLScreen::paintGL()
{
    typedef std::chrono::steady_clock clock;

    clock::time_point t_begin = clock::now();

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

    clock::time_point t_end = clock::now();

    // automatic frame skipping in case of lag
    double duration = std::chrono::duration<double>(t_end - t_begin).count();
    double durationAllowed = 1e-3 * theApplication->idleCallbackInterval();
    P->framesToSkip = (unsigned)(1.1 * duration / durationAllowed);
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

void MyGLScreen::keyAction_FullScreen()
{
    MyGLWindow *win = static_cast<MyGLWindow *>(window());
    Qt::WindowStates state = win->windowState() ^ Qt::WindowFullScreen;
    win->setWindowState(state);
    win->setMenuBarVisible(!(state & Qt::WindowFullScreen));
}

void MyGLScreen::keyAction_SpatialMode(int dir)
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        if (currentParam != SPATIALIZE) {
            currentParam = SPATIALIZE;
        }
        else {
            int theSpat = selectedCloud->cloud->getSpatialMode();
            selectedCloud->cloud->setSpatialMode(theSpat + dir, -1);
        }
    }
}

void MyGLScreen::keyAction_Overlap(int dir)
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        if (currentParam != OVERLAP) {
            currentParam = OVERLAP;
        }
        else {
            float theOver = selectedCloud->cloud->getOverlap();
            selectedCloud->cloud->setOverlap(theOver + dir * 0.01f);
        }
    }
}

void MyGLScreen::keyAction_MotionX()
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        currentParam = MOTIONX;
    }
}

void MyGLScreen::keyAction_MotionY()
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        currentParam = MOTIONY;
    }
}

void MyGLScreen::keyAction_MotionXY()
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        currentParam = MOTIONXY;
    }
}

void MyGLScreen::keyAction_Direction(int dir)
{
    Scene *scene = ::currentScene;
    SceneSample *selectedSample = scene->selectedSample();
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        if (currentParam != DIRECTION) {
            currentParam = DIRECTION;
        }
        else {
            int theDir = selectedCloud->cloud->getDirection();
            selectedCloud->cloud->setDirection(theDir + dir);
        }
    }
    if (selectedSample) {
        selectedSample->view->toggleOrientation();
    }
}

void MyGLScreen::keyAction_Window(int dir)
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        if (currentParam != WINDOW) {
            currentParam = WINDOW;
        }
        else {
            int theWin = selectedCloud->cloud->getWindowType();
            selectedCloud->cloud->setWindowType(theWin + dir);
        }
    }
}

void MyGLScreen::keyAction_Volume(int dir)
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    // cloud volume
    paramString = "";
    if (selectedCloud) {
        if (currentParam != VOLUME) {
            currentParam = VOLUME;
        }
        else {
            float theVol = selectedCloud->cloud->getVolumeDb();
            selectedCloud->cloud->setVolumeDb(theVol + dir * 0.5f);
        }
    }
}

void MyGLScreen::keyAction_Cloud(int dir)
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    scene->deselect(RECT);
    std::lock_guard<std::mutex> lock(::currentSceneMutex);
    if (dir < 0) {
        if (!scene->m_clouds.empty()) {
            scene->m_clouds.pop_back();
        }
        if (scene->m_clouds.empty()) {
            scene->m_selectedCloud = -1;
        }
        else {
            // still have a cloud so select
            scene->m_selectedCloud = scene->m_clouds.size() - 1;
            scene->m_clouds.back()->view->setSelectState(true);
        }
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
}



void MyGLScreen::keyAction_Trajectory(int dir)
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();


    Trajectory *tr=nullptr;
    paramString = "";

    if (selectedCloud) {
        if (currentParam != TRAJECTORY) {
            currentParam = TRAJECTORY;
        }
        else {
            if (dir < 0) {
                selectedCloud->view->stopTrajectory();
            }
            else  {
                if (selectedCloud->view->getTrajectory() == nullptr) {
                    tr=new Circular(g_defaultCloudParams.speed,selectedCloud->view->getOriginX(),selectedCloud->view->getOriginY(),g_defaultCloudParams.radius,
                                    g_defaultCloudParams.angle, 0, 1);
                    selectedCloud->cloud->setTrajectoryType (BOUNCING);
                    selectedCloud->view->updateCloudPosition(selectedCloud->view->getOriginX(),selectedCloud->view->getOriginY());
                    selectedCloud->view->setTrajectory(tr);
                    selectedCloud->view->startTrajectory();
                }
                else {
                    switch (selectedCloud->cloud->getTrajectoryType())  {
                    case BOUNCING: {
                        tr=new Circular(g_defaultCloudParams.speed,selectedCloud->view->getOriginX(),selectedCloud->view->getOriginY(),g_defaultCloudParams.radius,
                                        g_defaultCloudParams.angle, g_defaultCloudParams.stretch, g_defaultCloudParams.progress);
                        selectedCloud->cloud->setTrajectoryType (CIRCULAR);
                        selectedCloud->view->updateCloudPosition(selectedCloud->view->getOriginX(),selectedCloud->view->getOriginY());
                        selectedCloud->view->setTrajectory(tr);
                        selectedCloud->view->startTrajectory();
                        break;
                    }
                    case CIRCULAR: {
                        tr=new Hypotrochoid(g_defaultCloudParams.speed,selectedCloud->view->getOriginX(),selectedCloud->view->getOriginY(),g_defaultCloudParams.radius,
                                            g_defaultCloudParams.radiusInt,g_defaultCloudParams.expansion,g_defaultCloudParams.angle, g_defaultCloudParams.progress);
                        selectedCloud->cloud->setTrajectoryType (HYPOTROCHOID);
                        selectedCloud->view->updateCloudPosition(selectedCloud->view->getOriginX(),selectedCloud->view->getOriginY());
                        selectedCloud->view->setTrajectory(tr);
                        selectedCloud->view->startTrajectory();
                        break;
                    }
                    case HYPOTROCHOID: {
                        recordTrajectory();
                        break;
                    }
                    case RECORDED: {
                        tr=nullptr;
                        selectedCloud->cloud->setTrajectoryType (STATIC);
                        selectedCloud->view->updateCloudPosition(selectedCloud->view->getOriginX(),selectedCloud->view->getOriginY());
                        selectedCloud->view->setTrajectory(tr);
                        break;
                    }
                    default :
                        break;
                    }

                }
            }
        }
    }
}

void MyGLScreen::keyAction_Grain(int dir)
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        if (currentParam != NUMGRAINS) {
            currentParam = NUMGRAINS;
        }
        else {
            if (dir < 0) {
                selectedCloud->cloud->removeGrain();
            }
            else {
                selectedCloud->cloud->addGrain();
            }
        }
    }
}

void MyGLScreen::keyAction_Duration(int dir)
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        if (currentParam != DURATION) {
            currentParam = DURATION;
        }
        else {
            float theDur = selectedCloud->cloud->getDurationMs();
            selectedCloud->cloud->setDurationMs(theDur + dir * 5.0f);
        }
    }
}

void MyGLScreen::keyAction_LfoFreq(int dir)
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        if (currentParam != P_LFO_FREQ) {
            currentParam = P_LFO_FREQ;
        }
        else {
            float theLFOFreq = selectedCloud->cloud->getPitchLFOFreq();
            selectedCloud->cloud->setPitchLFOFreq(theLFOFreq + dir * 0.01f);
        }
    }
}

void MyGLScreen::keyAction_LfoAmount(int dir)
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    if (selectedCloud) {
        paramString = "";
        if (currentParam != P_LFO_AMT) {
            currentParam = P_LFO_AMT;
        }
        else {
            float theLFOAmnt = selectedCloud->cloud->getPitchLFOAmount();
            selectedCloud->cloud->setPitchLFOAmount(theLFOAmnt + dir * 0.001f);
        }
    }
}

void MyGLScreen::keyAction_Pitch(int dir)
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        if (currentParam != PITCH) {
            currentParam = PITCH;
        }
        else {
            float thePitch = selectedCloud->cloud->getPitch();
            selectedCloud->cloud->setPitch(thePitch + dir * 0.01f);
        }
    }
}

void MyGLScreen::keyAction_Active()
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    paramString = "";
    if (selectedCloud) {
        selectedCloud->cloud->toggleActive();
    }
}

void MyGLScreen::keyAction_EditEnvelope()
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    // volume envelope
    paramString = "";
    if (selectedCloud) {
        theApplication->showDialogVolumeEnvelope(selectedCloud);
    }
}

void MyGLScreen::keyAction_EditCloud()
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    // all parameters in a separate window
    paramString = "";
    if (selectedCloud) {
        theApplication->showCloudDialog(selectedCloud);
    }
}

void MyGLScreen::keyAction_SampleNames()
{
    // see name of cloud or sample
    paramString = "";
    showSampleNames = !showSampleNames;
}

void MyGLScreen::keyAction_ShowParameters()
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();
    if (scene->selectedCloud())
        scene->selectedCloud()->cloud->showParameters();
}

void MyGLScreen::recordTrajectory()
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    Trajectory *tr=nullptr;

    tr=new Recorded(0, selectedCloud->view->getOriginX(),selectedCloud->view->getOriginY());
    selectedCloud->cloud->setTrajectoryType(RECORDED);
    selectedCloud->view->updateCloudPosition(selectedCloud->view->getOriginX(),selectedCloud->view->getOriginY());
    selectedCloud->view->setTrajectory(tr);
    selectedCloud->view->setRecordTrajectoryAsked(true);
}

void MyGLScreen::loadTrajectory()
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    Trajectory *tr=nullptr;

    tr=new Recorded(0, selectedCloud->view->getOriginX(),selectedCloud->view->getOriginY());
    selectedCloud->cloud->setTrajectoryType(RECORDED);
    selectedCloud->view->setTrajectory(tr);
    theApplication->loadTrajectoryFile(selectedCloud);
}

void MyGLScreen::saveTrajectory()
{
    Scene *scene = ::currentScene;
    SceneCloud *selectedCloud = scene->selectedCloud();

    theApplication->saveTrajectoryFile(selectedCloud);
}

void MyGLScreen::mousePressEvent(QMouseEvent *event)
{
    Qt::MouseButton button = event->button();
    Scene *scene = ::currentScene;

    int modkey = event->modifiers();

    // look for selections if button is down
    switch (button) {
    case Qt::LeftButton: {
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

        // start recording trajectory
        if (scene->selectedCloud())
            if (scene->selectedCloud()->view->getRecordTrajectoryAsked()) {
                scene->selectedCloud()->view->setRecordTrajectoryAsked(false);
                scene->selectedCloud()->view->startTrajectory();
                scene->selectedCloud()->view->setRecordingTrajectory(true);
            }

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

                    if (modkey == Qt::ShiftModifier)
                        dragMode = RESIZE;
                }
            }

            if (!scene->m_selectionIndices.empty()) {
                scene->m_selectedSample = scene->m_selectionIndices[0];
                scene->selectedSample()->view->setSelectState(true);
            }
        }
        break;
    }
    case Qt::RightButton: {
        if (scene->selectedCloud()) {
           QMenu contextMenu(this);
           QIcon icon;
           QAction * pAction_recordTraj = contextMenu.addAction(icon, "Record trajectory");
           connect(pAction_recordTraj, SIGNAL(triggered()), this, SLOT(recordTrajectory()));
           QAction * pAction_loadTraj = contextMenu.addAction(icon, "Load trajectory");
           connect(pAction_loadTraj, SIGNAL(triggered()), this, SLOT(loadTrajectory()));
           if (scene->selectedCloud()->view->getTrajectoryType() == RECORDED) {
                QAction * pAction_saveTraj = contextMenu.addAction(icon, "Save trajectory");
                connect(pAction_saveTraj, SIGNAL(triggered()), this, SLOT(saveTrajectory()));
           }
           contextMenu.exec(QCursor::pos());
        }
        break;
    }
    default:
        break;
    }
}

void MyGLScreen::mouseReleaseEvent(QMouseEvent *event)
{
    Scene *scene = ::currentScene;
    if (scene->selectedCloud())
        if (scene->selectedCloud()->view->getRecordingTrajectory()) {
            scene->selectedCloud()->view->trajectoryAddPosition(mouseX, mouseY);
            scene->selectedCloud()->view->trajectoryAddPosition(mouseX, mouseY);
            scene->selectedCloud()->view->setRecordingTrajectory(false);
        }

    Qt::MouseButton button = event->button();

    if ((button == Qt::LeftButton) || (button == Qt::RightButton)) {
        lastDragX = veryHighNumber;
        lastDragY = veryHighNumber;
        dragMode = MOVE;
    }
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

    //origin of trajectory
    pt2d origin {0.,0.};
    double posCloudX=0.;
    double posCloudY=0.;

    Scene *scene = ::currentScene;
    SceneSample *selectedSample = scene->selectedSample();
    SceneCloud *selectedCloud = scene->selectedCloud();

    if (selectedCloud) {
        if (selectedCloud->cloud->getLockedState())
            if (selectedCloud->cloud->dialogLocked())
                return;
        posCloudX=selectedCloud->view->getOriginX(); //traj
        posCloudY=selectedCloud->view->getOriginY();
        if (selectedCloud->view->getRecordingTrajectory() == true){
            // add position in recorded trajectory
            selectedCloud->view->trajectoryAddPosition(mouseX, mouseY);
        }
        else {
            selectedCloud->view->updateCloudOrigin(mouseX, mouseY);
            selectedCloud->view->updateCloudPosition(selectedCloud->view->getOriginX(),selectedCloud->view->getOriginY());
            if(selectedCloud->view->getIsMoving())
            {
                origin=selectedCloud->view->getTrajectory()->getOrigin();
                selectedCloud->view->getTrajectory()->updateOrigin(origin.x-posCloudX+mouseX,origin.y-posCloudY+mouseY);
            }
        }

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

    case Qt::Key_O:
        keyAction_FullScreen();
        break;

    case Qt::Key_T:
        keyAction_SpatialMode((modkey == Qt::ShiftModifier) ? -1 : +1);
        break;

    case Qt::Key_S:  // overlap control
        keyAction_Overlap((modkey == Qt::ShiftModifier) ? -1 : +1);
        break;

    case Qt::Key_R:
        keyAction_MotionXY();
        break;

    case Qt::Key_F:  // direction
        keyAction_Direction((modkey == Qt::ShiftModifier) ? -1 : +1);
        break;

//    case Qt::Key_P:  // waveform display on/off
//        break;

    case Qt::Key_W:  // window editing for grain
        keyAction_Window((modkey == Qt::ShiftModifier) ? -1 : +1);
        break;

    case Qt::Key_B:
        keyAction_Volume((modkey == Qt::ShiftModifier) ? -1 : +1);
        break;

    case Qt::Key_Slash:  // switch to menu view
    case Qt::Key_Question:
        menuFlag = !menuFlag;
        break;

    case Qt::Key_I: {
        keyAction_Trajectory((modkey == Qt::ShiftModifier) ? -1 : +1);
        break;
    }

    case Qt::Key_G: {
        keyAction_Cloud((modkey == Qt::ShiftModifier) ? -1 : +1);
        break;
    }

    case Qt::Key_V:  // grains (add, delete)
        keyAction_Grain((modkey == Qt::ShiftModifier) ? -1 : +1);
        break;

    case Qt::Key_D:
        keyAction_Duration((modkey == Qt::ShiftModifier) ? -1 : +1);
        break;

    case Qt::Key_L:
        keyAction_LfoFreq((modkey == Qt::ShiftModifier) ? -1 : +1);
        break;

    case Qt::Key_K:
        keyAction_LfoAmount((modkey == Qt::ShiftModifier) ? -1 : +1);
        break;

    case Qt::Key_X:
        keyAction_MotionX();
        break;

    case Qt::Key_Y:
        keyAction_MotionY();
        break;

    case Qt::Key_Z:
        keyAction_Pitch((modkey == Qt::ShiftModifier) ? -1 : +1);
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
        keyAction_Active();
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
        keyAction_SampleNames();
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
        keyAction_EditEnvelope();
        break;
    }
    case Qt::Key_P: {
        keyAction_EditCloud();
        break;
    }
    case Qt::Key_U : {
        keyAction_ShowParameters();
        break;
    }
    default:
        break;
    }
    update();
}

void MyGLScreen::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
    // update();
}
