#include "Scale.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QFile>

extern int version_major, version_minor, version_patch;

Scale::Scale()
{

}

Scale::~Scale()
{
    reset();
}

void Scale::insertScalePosition(ScalePosition n_scalePosition)
{
    seePositions();

    unsigned long i = myScalePositions.size();
    myScalePositions.push_back(new ScalePosition);
    setScalePosition(i, n_scalePosition);

    seePositions();
}

void Scale::deleteScalePosition(unsigned long i)
{
    for (unsigned long j = i; j < myScalePositions.size() - 1; i = i + 1) {
        myScalePositions[j] = myScalePositions[j + 1];
    }
    myScalePositions.pop_back();
}

ScalePosition Scale::getScalePosition(unsigned long i)
{
    if (i < myScalePositions.size())
        return *myScalePositions[i];
}

void Scale::setScalePosition(unsigned long i, ScalePosition n_scalePosition)
{
    myScalePositions[i]->pitchInterval = n_scalePosition.pitchInterval;
}

int Scale::getSize()
{
    return myScalePositions.size();
}

void Scale::reset()
{
    int l_max = myScalePositions.size();
    for (unsigned long i = 0; i < l_max; i++) {
        myScalePositions.pop_back();
    }
}

double Scale::nearest(double c_interval, double c_minInterval, double c_maxInterval)
{
    int l_num = 0;
    double l_interval = c_interval;
    if (myScalePositions.size() != 0) {
        l_num = 1;
        l_interval = myScalePositions[0]->pitchInterval;
        double n_proxi = abs(c_interval - myScalePositions[0]->pitchInterval);
        double a_proxi = n_proxi;
        for (int i = 0; i < myScalePositions.size(); i = i + 1) {
            n_proxi = abs(c_interval - myScalePositions[i]->pitchInterval);
            if ((n_proxi < a_proxi) &&
                (c_minInterval <= myScalePositions[i]->pitchInterval) &&
                (c_maxInterval >= myScalePositions[i]->pitchInterval)) {
                l_num = i + 1;
                a_proxi = n_proxi;
                l_interval = myScalePositions[i]->pitchInterval;
            }
        }
    }
    return l_interval;
}

void Scale::seePositions()
{
/*    for (int i = 0; i < myScalePositions.size(); i=i+1) {
        cout << "i=" << i ;
        cout << " , interval ="<< myScalePositions[i]->pitchInterval << endl;
    }*/
}

bool Scale::saveScale()
{
    /*std::string nameScaleFile = askNameScale(FileDirection::Save);
    if (nameScaleFile.empty())
        return false;

    QFile scaleFile(QString::fromStdString(nameScaleFile));
    return save(scaleFile);*/
}

bool Scale::loadScale()
{

}

string Scale::askNameScale(FileDirection direction)
{
    // choise file name and test extension
    QString filterExtensionScene = QObject::tr("Scale files (*%1)").arg(g_extensionScale);
    QString pathScene = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    QFileDialog dlg(nullptr, QString(), pathScene, filterExtensionScene);
    dlg.setDefaultSuffix(g_extensionScale);
    if (direction == FileDirection::Save) {
        dlg.setWindowTitle(QObject::tr("Save scale"));
         dlg.setAcceptMode(QFileDialog::AcceptSave);
    }
    else {
        dlg.setWindowTitle(QObject::tr("Load scale"));
        dlg.setAcceptMode(QFileDialog::AcceptOpen);
        dlg.setFileMode(QFileDialog::ExistingFile);
    }

    if (dlg.exec() != QDialog::Accepted)
        return std::string();

    QStringList selection = dlg.selectedFiles();
    if (selection.size() != 1)
        return std::string();

    return selection.front().toStdString();
}

bool Scale::save(QFile &scaleFile)
{
    QString scaleFileName = scaleFile.fileName();
    QDir scaleFileDir = QFileInfo(scaleFileName).dir();

    if (!scaleFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QJsonObject docRoot;

    // audio path
    cout << "record scale " << scaleFile.fileName().toStdString() << "\n";

    QJsonArray docVersion;
    QJsonObject objVersion;
    objVersion["major"] = version_major;
    objVersion["minor"] = version_minor;
    objVersion["patch"] = version_patch;

    docRoot["version"] = objVersion;
    docVersion.append(objVersion);

    // Positions
    QJsonArray docScalePositions;
    cout << myScalePositions.size() << " positions : " << "\n";
    for (int i = 0, n = myScalePositions.size(); i < n; ++i) {

        cout << "Scale position " << i << " : ";
        cout << myScalePositions[i]->pitchInterval << "\n";
//        cout << "- name : " << myScalePositions[i]->name << "\n";

        QJsonObject objPosition;

//        objPosition["name"] = myScalePositions[i]->name;
        objPosition["interval"] = myScalePositions[i]->pitchInterval;

        docScalePositions.append(objPosition);

    }

    docRoot["version"] = docVersion;
    docRoot["positions"] = docScalePositions;

    QJsonDocument document;
    document.setObject(docRoot);
    scaleFile.write(document.toJson());
    if (!scaleFile.flush())
        return false;

    return true;

}

bool Scale::load(QFile &scaleFile)
{
    QString scaleFileName = scaleFile.fileName();
    QDir scaleFileDir = QFileInfo(scaleFileName).dir();

    if (!scaleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(scaleFile.readAll(), &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    QJsonObject docRoot = doc.object();

    QJsonArray docVersion = docRoot["version"].toArray();
    for (const QJsonValue &jsonElement : docVersion) {
        QJsonObject objVersion = jsonElement.toObject();
        int l_version_major = objVersion["major"].toInt();
        int l_version_minor = objVersion["minor"].toInt();
        int l_version_patch = objVersion["patch"].toInt();
        cout << "scale version " << l_version_major << "." << l_version_minor << "." << l_version_patch << endl;
    }


    QJsonArray docPositions = docRoot["positions"].toArray();
    reset();
    for (const QJsonValue &jsonElement : docPositions) { // samples
        QJsonObject objPosition = jsonElement.toObject();
        ScalePosition l_scalePosition;
        l_scalePosition.pitchInterval = objPosition["interval"].toDouble();
        cout << "Interval : " << l_scalePosition.pitchInterval << endl;
        insertScalePosition(l_scalePosition);
    }
}
