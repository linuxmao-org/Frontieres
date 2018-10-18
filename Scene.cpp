#include "Scene.h"

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
Scene::~Scene()
{
}
//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Scene::Scene()
{
}
//-----------------------------------------------------------------------------
// record
//----------------------------------------------------------------------------
string Scene::askNameScene(bool direction)
{

    // choise file name and test extension
    QString captionPath = qApp->translate("Frontieres","Frontieres : name of the scene");
    QString filterExtensionScene = "*" + g_extensionScene;
    QString pathScene = QString::fromStdString(g_audioPath);
    if (g_audioPath == g_audioPathDefault)
        pathScene = QStandardPaths::displayName(QStandardPaths::HomeLocation).toUtf8().constData();
    string nameSceneFile = "";
    if (direction == true){
        nameSceneFile = QFileDialog::getSaveFileName(nullptr, captionPath, pathScene, filterExtensionScene ).toUtf8().constData();
        if (nameSceneFile.length() != 0) {
            if (nameSceneFile.substr(nameSceneFile.size()-4, nameSceneFile.size()) != g_extensionScene.toUtf8().constData())
                nameSceneFile = nameSceneFile + g_extensionScene.toUtf8().constData();
            cout << "save scene " << nameSceneFile << endl;
        }
        else
            cout << "save scene aborted" << endl;
    }
    else {
        nameSceneFile = QFileDialog::getOpenFileName(nullptr, captionPath, pathScene, filterExtensionScene ).toUtf8().constData();
        if (nameSceneFile.length() != 0)
            cout << "load scene " << nameSceneFile << endl;
        else
            cout << "load scene aborted" << endl;
    }
    return nameSceneFile;
}
