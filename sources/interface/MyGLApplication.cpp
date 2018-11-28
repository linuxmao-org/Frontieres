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
#include "OptionsDialog.h"
#include "Frontieres.h"
#include "I18n.h"
#include "model/Sample.h"
#include <QTranslator>
#include <QLibraryInfo>
#include <QTimer>
#include <QDebug>
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

    void onIdle();
};

MyGLApplication::MyGLApplication(int &argc, char *argv[])
    : QApplication(argc, argv),
      P(new Impl)
{
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
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]{ P->onIdle(); });
    timer->start(1e3 / fps);
}

bool MyGLApplication::loadSceneFile()
{
    std::string nameSceneFile = Scene::askNameScene(FileDirection::Load);
    if (nameSceneFile.empty())
        return false;


    // delete cloudMaps if existing

    destroyAllCloudDialogs();

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
        nullptr, _Q("", "Load sample"),
        QString(), _Q("", "Sample files (*.*)"));
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
    dlg->linkCloud(selectedCloud->cloud.get(), selectedCloud->view.get());
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

void MyGLApplication::showOptionsDialog()
{
    OptionsDialog optionsDlg;
    optionsDlg.exec();
}

void MyGLApplication::showCombiDialog()
{
  /*  combiDialog = new CombiDialog;
    combiDialog->setWindowTitle(tr("Combination"));
    Combination *myCombi = new Combination;
    for (int i = 0; i < currentScene->m_clouds.size(); i++) {
        Cloud *l_cloud = currentScene->m_clouds[i]->cloud.get();
        combiDialog->addCloudToDialog(QString::number(l_cloud->getId()), l_cloud->getName());
    }
    combiDialog->initCombi(myCombi);
    combiDialog->setModal(true);
    combiDialog->exec();*/
}

void MyGLApplication::showMidiBankDialog()
{
    bankDialog = new BankDialog;
    bankDialog->setWindowTitle(tr("Combinations"));
    bankDialog->init(currentScene);
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

void MyGLApplication::Impl::onIdle()
{
    window->screen()->update();
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
