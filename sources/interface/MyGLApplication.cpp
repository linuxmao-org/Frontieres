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

#include "MyGLApplication.h"
#include "MyGLWindow.h"
#include "CloudDialog.h"
#include "TriggerDialog.h"
#include "ControlDialog.h"
#include "PhraseDialog.h"
#include "OptionsDialog.h"
#include "Frontieres.h"
#include "model/Sample.h"
#include <QTranslator>
#include <QLibraryInfo>
#include <QTimer>
#include <QDebug>
#include <QInputDialog>
#include <map>

struct MyGLApplication::Impl {
    // translator of the internal parts of Qt
    QTranslator qtTranslator;
    // translator of the application itself
    QTranslator appTranslator;

    // translation path search according to platform
    QString getQtTranslationDir() const;
    QString getAppTranslationDir() const;

    // graphics window
    MyGLWindow *window = nullptr;

    // cloud dialogs, according to cloud ID
    std::map<unsigned, CloudDialog *> cloudDialogs;

    // trigger dialogs, according to trigger ID
    std::map<unsigned, TriggerDialog *> triggerDialogs;

    // control dialogs, according to cloud ID
    std::map<unsigned, ControlDialog *> controlDialogs;

    // phrase dialogs, according to cloud ID
    std::map<unsigned, PhraseDialog *> phraseDialogs;

    void onIdle();
};

MyGLApplication::MyGLApplication(int &argc, char *argv[])
    : QApplication(argc, argv),
      P(new Impl)
{
    // init resources
    Q_INIT_RESOURCE(docs);

    // init internationalization
    installTranslator(&P->qtTranslator);
    installTranslator(&P->appTranslator);

    // find directories for translation catalogs
    QString qtTranslationDir = P->getQtTranslationDir();
    qDebug() << "Qt translation dir:" << qtTranslationDir;
    QString appTranslationDir = P->getAppTranslationDir();
    qDebug() << "App translation dir:" << appTranslationDir;

    // set up the translations
    QString language = QLocale::system().name();
    P->qtTranslator.load("qt_" + language, qtTranslationDir);
    P->appTranslator.load("Frontieres_" + language, appTranslationDir);
}

void MyGLApplication::initializeInterface()
{
    // init graphic components
    MyGLWindow *window = new MyGLWindow;
    P->window = window;
}

MyGLApplication::~MyGLApplication()
{
}

MyGLWindow *MyGLApplication::GLwindow()
{
    return P->window;
}

void MyGLApplication::startIdleCallback(double fps)
{
    QTimer *timer = idleTimer;
    if (!timer) {
        idleTimer = timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this]{ P->onIdle(); });
    }
    timer->start(1e3 / fps);
}

int MyGLApplication::idleCallbackInterval() const
{
    QTimer *timer = idleTimer;
    if (!timer)
        return 0;
    return timer->interval();
}

bool MyGLApplication::loadSceneFile()
{
    std::string nameSceneFile = Scene::askNameScene(FileDirection::Load);
    if (nameSceneFile.empty())
        return false;


    // delete Maps if existing

    destroyAllCloudDialogs();
    destroyAllTriggerDialogs();
    destroyAllControlDialogs();

    // load the scene and its samples
    Scene *scene = new Scene;
    QFile sceneFile(QString::fromStdString(nameSceneFile));
    if (!scene->load(sceneFile) || !scene->loadSampleSet(true)) {
        delete scene;
        return false;
    }

    // swap with the current, acquiring mutex for very short time
    std::unique_lock<std::mutex> lock(::currentSceneMutex);
    std::swap(::currentScene, scene);
    lock.unlock();

    // delete the previous scene
    delete scene;
    return true;
}

bool MyGLApplication::loadCloudDefaultFile()
{
    std::string nameCloudFile = Scene::askNameCloud(FileDirection::Load);
    if (nameCloudFile.empty())
        return false;
    // load the clouds default params file

    QFile cloudFile(QString::fromStdString(nameCloudFile));
    return currentScene->loadCloudDefault(cloudFile);
}

bool MyGLApplication::saveCloudTrajectoryFile(SceneCloud *selectedCloudSave)
{
    std::string nameTrajectoryFile = Scene::askNameTrajectory(FileDirection::Save);
    if (nameTrajectoryFile.empty())
        return false;

    QFile trajectoryFile(QString::fromStdString(nameTrajectoryFile));
    return ::currentScene->saveCloudRecordedTrajectory(trajectoryFile, selectedCloudSave);

}

bool MyGLApplication::loadCloudTrajectoryFile(SceneCloud *selectedCloudLoad)
{
    std::string nameTrajectoryFile = Scene::askNameTrajectory(FileDirection::Load);
    if (nameTrajectoryFile.empty())
        return false;

    QFile trajectoryFile(QString::fromStdString(nameTrajectoryFile));
    return ::currentScene->loadCloudRecordedTrajectory(trajectoryFile, selectedCloudLoad);
}

bool MyGLApplication::saveTriggerTrajectoryFile(SceneTrigger *selectedTriggerSave)
{
    std::string nameTrajectoryFile = Scene::askNameTrajectory(FileDirection::Save);
    if (nameTrajectoryFile.empty())
        return false;

    QFile trajectoryFile(QString::fromStdString(nameTrajectoryFile));
    return ::currentScene->saveTriggerRecordedTrajectory(trajectoryFile, selectedTriggerSave);
}

bool MyGLApplication::loadTriggerTrajectoryFile(SceneTrigger *selectedTriggerLoad)
{
    std::string nameTrajectoryFile = Scene::askNameTrajectory(FileDirection::Load);
    if (nameTrajectoryFile.empty())
        return false;

    QFile trajectoryFile(QString::fromStdString(nameTrajectoryFile));
    return ::currentScene->loadTriggerRecordedTrajectory(trajectoryFile, selectedTriggerLoad);
}

bool MyGLApplication::saveSceneFile()
{
    std::string nameSceneFile = Scene::askNameScene(FileDirection::Save);
    if (nameSceneFile.empty())
        return false;

    QFile sceneFile(QString::fromStdString(nameSceneFile));
    return ::currentScene->save(sceneFile);
}

bool MyGLApplication::saveCloudFile(SceneCloud *selectedCloudSave)
{
    std::string nameCloudFile = Scene::askNameCloud(FileDirection::Save);
    if (nameCloudFile.empty())
        return false;

    QFile cloudFile(QString::fromStdString(nameCloudFile));
    return ::currentScene->saveCloud(cloudFile, selectedCloudSave);
}

void MyGLApplication::addSample()
{
    QStringList qSampleFiles = QFileDialog::getOpenFileNames(
        nullptr, tr("Load sample"),
        QString(), tr("Sample files (*.*)"));
    QString qSampleFile;
    for (int i = 0 ; i < qSampleFiles.count() ; i++) {
        qSampleFile = (qSampleFiles.at(i));
        QDir qSampleDir = QFileInfo(qSampleFile).dir();

        std::unique_lock<std::mutex> lock(::currentSceneMutex);
        Scene *scene = ::currentScene;

        Sample *af = scene->loadNewSample(qSampleFile.toStdString());
        if (af) {
            // add into the scene
            scene->addSampleVis(af);

            // add the file's location into search paths, if not already
            scene->addAudioPath(qSampleDir.path().toStdString());
        }
    }
}

void MyGLApplication::addInput()
{
    //todo : demander numero d'input
    bool ok = false;
    int l_input = QInputDialog::getInt(P->window, "",
                                 tr("Input number : "), 1, 1, theInChannelCount, 1, &ok);
    if (ok) {
        //int l_input = 1;

        std::unique_lock<std::mutex> lock(::currentSceneMutex);
        Scene *scene = ::currentScene;

        Sample *af = scene->loadNewInput(l_input);
        if (af) {
            // add into the scene
            scene->addSampleVis(af);
        }
    }
}

void MyGLApplication::showDialogVolumeEnvelope(SceneCloud *selectedCloudToVolumeEnvelope)
{
    currentScene->changeParamEnvelopeVolume(selectedCloudToVolumeEnvelope);
}

void MyGLApplication::showCloudDialog(SceneCloud *selectedCloud)
{
    unsigned id = selectedCloud->cloud->getId();

    CloudDialog *&dlgslot = P->cloudDialogs[id];
    CloudDialog *dlg = dlgslot;

    if (!dlg) {
        // if not existing, create it and insert it in the map
        dlg = new CloudDialog;
        dlg->setWindowTitle(tr("Cloud parameters"));
        dlgslot = dlg;
        dlg->show();
    }
    else {
        QPoint posdlg = dlg->pos();
        dlg->hide();
        dlg->show();
        dlg->move(posdlg);
    }
    selectedCloud->cloud.get()->changesDone(true);
    selectedCloud->view.get()->changesDone(true);
    dlg->linkCloud(selectedCloud->cloud.get(), selectedCloud->view.get());
}

void MyGLApplication::showTriggerDialog(SceneTrigger *selectedTrigger)
{
    unsigned id = selectedTrigger->trigger->getId();

    TriggerDialog *&dlgslot = P->triggerDialogs[id];
    TriggerDialog *dlg = dlgslot;

    if (!dlg) {
        // if not existing, create it and insert it in the map
        dlg = new TriggerDialog;
        dlg->setWindowTitle(tr("Trigger parameters"));
        dlgslot = dlg;
        dlg->show();
    }
    else {
        QPoint posdlg = dlg->pos();
        dlg->hide();
        dlg->show();
        dlg->move(posdlg);
    }
    selectedTrigger->trigger.get()->changesDone(true);
    selectedTrigger->view.get()->changesDone(true);
    dlg->linkTrigger(selectedTrigger->trigger.get(), selectedTrigger->view.get());
}

void MyGLApplication::showControlDialog(SceneCloud *selectedCloud)
{
//    cout << "showcontroldialog entree" << endl;
    unsigned id = selectedCloud->cloud->getId();

    ControlDialog *&dlgslot = P->controlDialogs[id];
    ControlDialog *dlg = dlgslot;

    if (!dlg) {
        // if not existing, create it and insert it in the map
        dlg = new ControlDialog;
        dlg->setWindowTitle(tr("Cloud controller"));
        dlgslot = dlg;
        dlg->show();
    }
    else {
        QPoint posdlg = dlg->pos();
        dlg->hide();
        dlg->show();
        dlg->move(posdlg);
    }
    //selectedCloud->cloud.get()->changesDone(true);
    //selectedCloud->view.get()->changesDone(true);
    dlg->linkCloud(selectedCloud->cloud.get());
    //cout << "showcontroldialog sortie" << endl;
}

void MyGLApplication::showPhraseDialog(SceneCloud *selectedCloud)
{
    //cout << "showPhrasedialog entree" << endl;
    unsigned id = selectedCloud->cloud->getId();

    PhraseDialog *&dlgslot = P->phraseDialogs[id];
    PhraseDialog *dlg = dlgslot;

    if (!dlg) {
        // if not existing, create it and insert it in the map
        dlg = new PhraseDialog;
        dlg->setWindowTitle(tr("Cloud phrase"));
        dlgslot = dlg;
        dlg->show();
    }
    else {
        QPoint posdlg = dlg->pos();
        dlg->hide();
        dlg->show();
        dlg->move(posdlg);
    }
    //cout << "showPhrasedialog milieu" << endl;
    //selectedCloud->cloud.get()->changesDone(true);
    //selectedCloud->view.get()->changesDone(true);
    dlg->linkCloud(selectedCloud->cloud.get());
    //cout << "showPhrasedialog sortie" << endl;
}

void MyGLApplication::destroyCloudDialog(unsigned selectedCloudId)
{
    auto it = P->cloudDialogs.find(selectedCloudId);

    if (it == P->cloudDialogs.end())
        return;  // not found

    CloudDialog *dlg = it->second;
    if (!dlg)
        return;

    delete dlg;
    P->cloudDialogs.erase(it);
}

void MyGLApplication::destroyTriggerDialog(unsigned selectedTriggerId)
{
    auto it = P->triggerDialogs.find(selectedTriggerId);

    if (it == P->triggerDialogs.end())
        return;  // not found

    TriggerDialog *dlg = it->second;
    if (!dlg)
        return;

    delete dlg;
    P->triggerDialogs.erase(it);
}

void MyGLApplication::destroyControlDialog(unsigned selectedCloudId)
{
    //cout << "entree destroycontroldialog" << endl;
    auto it = P->controlDialogs.find(selectedCloudId);

    if (it == P->controlDialogs.end())
        return;  // not found

    ControlDialog *dlg = it->second;
    if (!dlg)
        return;

    delete dlg;
    P->controlDialogs.erase(it);
    // cout << "sortie destroycontroldialog" << endl;
}

void MyGLApplication::destroyPhraseDialog(unsigned selectedCloudId)
{
    cout << "entree destroyPhraseDialog" << endl;
    auto it = P->phraseDialogs.find(selectedCloudId);

    if (it == P->phraseDialogs.end())
        return;  // not found

    PhraseDialog *dlg = it->second;
    if (!dlg)
        return;

    delete dlg;
    P->phraseDialogs.erase(it);
    cout << "sortie destroyPhraseDialog" << endl;
}

void MyGLApplication::midiNoteOn(int midiChannelToPlay, int midiKeyToPlay, int midiVeloToPlay)
{
    currentScene->midiNoteOn(midiChannelToPlay, midiKeyToPlay, midiVeloToPlay);
}

void MyGLApplication::midiNoteOff(int midiChannelToStop, int midiKeyToStop)
{
    currentScene->midiNoteOff(midiChannelToStop, midiKeyToStop);
}

void MyGLApplication::destroyAllCloudDialogs()
{
    while (P->cloudDialogs.size() > 0){
        destroyCloudDialog(P->cloudDialogs.begin()->first);
    }
}

void MyGLApplication::destroyAllTriggerDialogs()
{
    while (P->triggerDialogs.size() > 0){
        destroyTriggerDialog(P->triggerDialogs.begin()->first);
    }
}

void MyGLApplication::destroyAllControlDialogs()
{
    while (P->controlDialogs.size() > 0){
        destroyControlDialog(P->controlDialogs.begin()->first);
    }
}

void MyGLApplication::showOptionsDialog()
{
    OptionsDialog optionsDlg;
    optionsDlg.exec();
}

void MyGLApplication::showMidiBankDialog()
{
    bankDialog = new BankDialog;
    bankDialog->setWindowTitle(tr("Combinations"));
    bankDialog->init(currentScene, EDIT, -1);
    bankDialog->setModal(true);
    bankDialog->exec();
}

void MyGLApplication::showMidiInstrumentDialog()
{
    instrumentDialog = new InstrumentDialog;
    instrumentDialog->setWindowTitle(tr("Midi instrument"));
    instrumentDialog->init(currentScene);
    instrumentDialog->setModal(true);
    instrumentDialog->exec();
}

void MyGLApplication::showPhrasesDialog()
{
    phraseDialog = new PhraseDialog;
    phraseDialog->setWindowTitle(tr("Phrases"));
    phraseDialog->init(currentScene);
    phraseDialog->setModal(false);
    phraseDialog->show();
}

void MyGLApplication::showScalesDialog()
{
    scaleDialog = new ScalesDialog;
    scaleDialog->setWindowTitle(tr("Scales"));
    scaleDialog->init(currentScene);
    scaleDialog->setModal(false);
    scaleDialog->show();
}

void MyGLApplication::destroyPhrasesDialog()
{
    delete phraseDialog;
}

void MyGLApplication::destroyScalesDialog()
{
    delete scaleDialog;
}


void MyGLApplication::Impl::onIdle()
{
    MyGLScreen *screen = window->screen();
    if (screen->advanceFrame())
        screen->update();
}

QString MyGLApplication::Impl::getQtTranslationDir() const
{
#if defined(Q_OS_WIN)
    // on Windows, search the program directory
    return QCoreApplication::applicationDirPath() + "/translations";
#elif defined(Q_OS_DARWIN)
    // on Mac, search inside the app bundle
    return QCoreApplication::applicationDirPath() + "/../Resources/translations";
#else
    // search in the system library path
    return QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#endif
}

QString MyGLApplication::Impl::getAppTranslationDir() const
{
#if defined(Q_OS_WIN) || defined(Q_OS_DARWIN)
    // on Windows and Mac, translations ship with the program in the same place
    return getQtTranslationDir();
#else
    // search in the shared data path
    return DATA_ROOT_DIR "/Frontieres/translations";
#endif
}
