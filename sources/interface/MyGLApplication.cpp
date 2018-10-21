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
#include "Frontieres.h"
#include "model/Scene.h"
#include <QTranslator>
#include <QLibraryInfo>
#include <QTimer>
#include <QDebug>

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

    // load the scene and its sounds
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

bool MyGLApplication::saveSceneFile()
{
    std::string nameSceneFile = Scene::askNameScene(FileDirection::Save);
    if (nameSceneFile.empty())
        return false;

    QFile sceneFile(QString::fromStdString(nameSceneFile));
    return ::currentScene->save(sceneFile);
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
