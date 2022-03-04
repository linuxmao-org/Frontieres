//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     https://github.com/linuxmao-org/Frontieres
//
// Copyright (C) 2020  Olivier Flatres
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
#include "Phrase.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QFile>

extern int version_major, version_minor, version_patch;
// ids
static unsigned int phraseId = 0;

Phrase::Phrase()
{
    ControlPoint *l_controlInterval = new ControlPoint;
    l_controlInterval->value = 0;
    l_controlInterval->delay = 0;
    myControlInterval.push_back(l_controlInterval);
    ControlPoint *l_controlShade = new ControlPoint;
    l_controlShade->value = 1;
    l_controlShade->delay = 0;
    myControlShade.push_back(l_controlShade);
    // phrase id
    myId = ++phraseId;
    cout << "new phrase, id =" << myId << endl;

}

Phrase::~Phrase()
{
    for (unsigned long i = 0; i < myControlShade.size(); i++) {
        delete myControlShade[i];
    }
    for (unsigned long i = 0; i < myControlInterval.size(); i++) {
        delete myControlInterval[i];
    }
}

void Phrase::setName(QString newName)
{
    myName = newName;
}

QString Phrase::getName()
{
    return myName;
}

unsigned int Phrase::getId()
{
    return myId;
}

void Phrase::setId(unsigned int phraseId)
{
    myId = phraseId;
}

ControlPoint Phrase::getControlShade(unsigned long l_num)
{
    return *myControlShade[l_num];
}

ControlPoint Phrase::getControlInterval(unsigned long l_num)
{
    //cout << "get control interval " << l_num << endl;
    return *myControlInterval[l_num];
}

void Phrase::setControlInterval(unsigned long l_num, float l_delay, float l_value)
{
    if (myControlInterval.size() >= l_num) {
        myControlInterval[l_num]->value = l_value;
        myControlInterval[l_num]->delay = l_delay;
    }
}

void Phrase::setControlShade(unsigned long l_num, float l_delay, float l_value)
{
    if (myControlShade.size() >= l_num) {
        myControlShade[l_num]->value = l_value;
        myControlShade[l_num]->delay = l_delay;
    }
}

int Phrase::insertControlShade(float l_delay, float l_value)
{
    ControlPoint *l_controlPoint = new ControlPoint;
    l_controlPoint->delay = l_delay;
    l_controlPoint->value = l_value;
    int l_result = 0;
    for (unsigned long i = 0; i < myControlShade.size(); i = i + 1){
        l_result = int(i);
        if (myControlShade[i]->delay > l_delay) {
            myControlShade.insert(myControlShade.begin() + i, l_controlPoint);
            break;
        }
        if (i == myControlShade.size() - 1) {
            myControlShade.push_back(l_controlPoint);
            l_result = l_result + 1;
            break;
        }
    }
    return l_result;
}

int Phrase::insertControlInterval(float l_delay, float l_value, bool l_silence)
{
    ControlPoint *l_controlPoint = new ControlPoint;
    l_controlPoint->delay = l_delay;
    l_controlPoint->value = l_value;
    l_controlPoint->silence = l_silence;
    int l_result = 0;
    for (unsigned long i = 0; i < myControlInterval.size(); i = i + 1){
        l_result = int(i);
        //cout << "i=" << i << endl;
        if (myControlInterval[i]->delay > l_delay){
            myControlInterval.insert(myControlInterval.begin() + i, l_controlPoint);
            break;
        }
        if (i == myControlInterval.size() - 1) {
            myControlInterval.push_back(l_controlPoint);
            l_result = l_result + 1;
            break;
        }
    }
    //debugListControlInterval();
    //cout << "l_result = "  << l_result << endl;
    return l_result;
}

void Phrase::deleteControlShade(unsigned long l_num)
{
    myControlShade.erase(myControlShade.begin() + l_num - 1);
}

void Phrase::deleteControlInterval(unsigned long l_num)
{
    myControlInterval.erase(myControlInterval.begin() + l_num);
}

float Phrase::getShade(LocalPhrase &l_phrase)
{
    float l_delay = float((GTime::instance().sec - l_phrase.phraseStartTime) * l_phrase.tempo / 60);
    if ((myControlShade.size() == 1) || (!l_phrase.activeState)) {
        l_phrase.shadeEndedState = true;
        if (l_phrase.intervalEndedState)
            l_phrase.endedState = true;
        return myControlShade[0]->value;
    }
    else if (myControlShade[myControlShade.size() - 1]->delay < l_delay) {
        l_phrase.shadeEndedState = true;
        if (l_phrase.intervalEndedState)
                l_phrase.endedState = true;
        return myControlShade[myControlShade.size() - 1]->value;
    }
    else {
        float firstDelay;
        float lastDelay;
        float firstShade;
        float lastShade;
        float result = 0;

        for (unsigned long i = 0; i < myControlShade.size(); i = i + 1){
            if (myControlShade[i + 1]->delay > l_delay) {
                firstDelay = myControlShade[i]->delay;
                lastDelay = myControlShade[i+1]->delay;
                firstShade = myControlShade[i]->value;
                lastShade = myControlShade[i+1]->value;

                if (l_phrase.silenceState)
                    result = 0;
                else
                    result = firstShade + ((lastShade - firstShade) * (l_delay - firstDelay) / (lastDelay - firstDelay));
                break;
            }
        }
        return result;
    }
}

float Phrase::getInterval(LocalPhrase &l_phrase)
{
//    cout << "getinterval, ";
    float l_delay = float((GTime::instance().sec - l_phrase.phraseStartTime) * l_phrase.tempo / 60);
//    cout << "l_delay = " << l_delay << endl;
    if ((myControlInterval.size() == 1) || (!l_phrase.activeState)) {
        l_phrase.silenceState = false;
        l_phrase.intervalEndedState = true;
        if (l_phrase.shadeEndedState)
            l_phrase.endedState = true;
        actuasiseReleaseAndAttack(l_phrase);
        return myControlInterval[0]->value;
    }
    else {
        if (myControlInterval[myControlInterval.size() - 1]->delay <= l_delay) {
            l_phrase.silenceState = myControlInterval[myControlInterval.size() - 1]->silence;
            l_phrase.intervalEndedState = true;
            if (l_phrase.shadeEndedState)
                l_phrase.endedState = true;
            actuasiseReleaseAndAttack(l_phrase);
            return myControlInterval[myControlInterval.size() - 1]->value;
        }
        else {
            float firstDelay;
            float lastDelay;
            float firstInterval;
            float lastInterval;
            float result = 0;

            for (unsigned long i = 0; i < myControlInterval.size(); i = i + 1){
                if (myControlInterval[i + 1]->delay > l_delay) {
                    firstDelay = myControlInterval[i]->delay;
                    lastDelay = myControlInterval[i+1]->delay;
                    firstInterval = myControlInterval[i]->value;
                    lastInterval = myControlInterval[i+1]->value;
                    result = firstInterval + ((lastInterval - firstInterval) * (l_delay - firstDelay) / (lastDelay - firstDelay));
                    l_phrase.silenceState = myControlInterval[i]->silence;
                    actuasiseReleaseAndAttack(l_phrase);
                    if (l_phrase.silenceState) {
                        result = firstInterval;
                    }
                    break;
                }
            }
            /*for (unsigned long i=1; i < myControlInterval.size(); i=i+1) {
                if (l_delay >= myControlInterval[i]->delay) {
                    if (myControlInterval[i]->silence) {
                        result = myControlInterval[i]->value;
                        silenceState = true;
                        actuasiseReleaseAndAttack();
                        break;
                    }
                    else {
                        firstDelay = myControlInterval[i]->delay;
                        lastDelay = myControlInterval[i+1]->delay;
                        firstInterval = myControlInterval[i]->value;
                        lastInterval = myControlInterval[i+1]->value;
                        result = firstInterval + ((lastInterval - firstInterval) * (l_delay - firstDelay) / (lastDelay - firstDelay));
                        silenceState = false;
                        actuasiseReleaseAndAttack();
                        break;
                    }
                }
            }*/
            return result;
        }
    }
}

unsigned long Phrase::getMyControlShadeSize()
{
    return myControlShade.size();
}

unsigned long Phrase::getMyControlIntervalSize()
{
    //cout << "entree getMyControlIntervalSize, ";
    //cout << "myControlInterval.size()=" << myControlInterval.size() << endl;
    return myControlInterval.size();
}

void Phrase::debugListControlShade()
{
    cout << "liste controlShade" << endl;
    for (unsigned long i = 0; i < myControlShade.size(); i = i + 1) {
        cout <<  i << ", delay=" << myControlShade[i]->delay << ", value=" << myControlShade[i]->value << endl;
    }
}

void Phrase::debugListControlInterval()
{
    cout << "liste controlInterval" << endl;
    for (unsigned long i = 0; i < myControlInterval.size(); i = i + 1) {
        cout <<  i << ", delay=" << myControlInterval[i]->delay << ", value=" << myControlInterval[i]->value << endl;
    }
}

void Phrase::setActiveState(LocalPhrase &l_phrase, bool l_activeState)
{
    //cout << "entree phrase::setactivestate" << endl;
    if (l_activeState) {
        //if (endedState) {
            setPhraseStartTime(l_phrase);
        //}
    }
    l_phrase.activeState = l_activeState;
}


void Phrase::setPhraseStartTime(LocalPhrase &l_phrase)
{
    l_phrase.phraseStartTime = GTime::instance().sec;
    l_phrase.intervalEndedState = false;
    l_phrase.shadeEndedState = false;
    l_phrase.attack = true;
    l_phrase.attacked = false;
    l_phrase.endedState = false;
}

void Phrase::setTempo(LocalPhrase &l_phrase, int l_tempo)
{
    l_phrase.tempo = l_tempo;
}

void Phrase::setSilence(unsigned long l_num, bool l_silence)
{
    myControlInterval[l_num]->silence = l_silence;
}

void Phrase::actuasiseReleaseAndAttack(LocalPhrase &l_phrase)
{
    //cout << "actualise release =" << release << "," << released << ",attack =" << attack <<"," << attacked<< endl;
    if (l_phrase.silenceState) {
       //cout << "silenstate"<<endl;
       if (l_phrase.released) {
           l_phrase.release = false;
       }
       else {
           l_phrase.release = true;
           l_phrase.released = true;
           l_phrase.attacked = false;
       }
    }
    else {
        if (l_phrase.attacked) {
            l_phrase.attack = false;
        }
        else {
            l_phrase.attacked = true;
            l_phrase.attack = true;
        }
        l_phrase.release = false;
        l_phrase.released = false;
    }
    //cout << "actualise release =" << release << "," << released << ",attack =" << attack <<"," << attacked<< endl;
}

string Phrase::askNamePhrase(FileDirection direction)
{
    // choise file name and test extension
    QString filterExtensionScene = QObject::tr("Phrase files (*%1)").arg(g_extensionPhrase);
    QString pathScene = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    QFileDialog dlg(nullptr, QString(), pathScene, filterExtensionScene);
    dlg.setDefaultSuffix(g_extensionPhrase);
    if (direction == FileDirection::Save) {
        dlg.setWindowTitle(QObject::tr("Save phrase"));
        dlg.setAcceptMode(QFileDialog::AcceptSave);
    }
    else {
        dlg.setWindowTitle(QObject::tr("Load phrase"));
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

bool Phrase::save(QFile &phraseFile)
{
    QString phraseFileName = phraseFile.fileName();
    QDir phraseFileDir = QFileInfo(phraseFileName).dir();

    if (!phraseFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QJsonObject docRoot;

    // audio path
    cout << "record phrase " << phraseFile.fileName().toStdString() << "\n";

    QJsonArray docVersion;
    QJsonObject objVersion;
    objVersion["major"] = version_major;
    objVersion["minor"] = version_minor;
    objVersion["patch"] = version_patch;

    docRoot["version"] = objVersion;
    docVersion.append(objVersion);

    QJsonArray docPhrase;
    QJsonObject objPhrase;
    objPhrase["name"] = myName;

    // Positions
    QJsonArray docIntervals;
    cout << myControlInterval.size() << " intervals : " << "\n";
    for (unsigned long i = 1, n = myControlInterval.size(); i < n; ++i) {

        cout << "Phrase interval " << i << ", delay : ";
        cout << myControlInterval[i]->delay << ",value :" << myControlInterval[i]->value << ", silence :" << myControlInterval[i]->silence << "\n";

        QJsonObject objInterval;

        objInterval["value"] = double(myControlInterval[i]->value);
        objInterval["delay"] = double(myControlInterval[i]->delay);
        objInterval["silence"] = myControlInterval[i]->silence;

        docIntervals.append(objInterval);

    }
    QJsonArray docShades;
    cout << myControlShade.size() << " shades : " << "\n";
    for (unsigned long i = 1, n = myControlShade.size(); i < n; ++i) {

        cout << "Phrase shade " << i << ", delay : ";
        cout << myControlShade[i]->delay << ",value :" << myControlShade[i]->value << "\n";

        QJsonObject objShade;

        objShade["value"] = double(myControlShade[i]->value);
        objShade["delay"] = double(myControlShade[i]->delay);


        docShades.append(objShade);

    }

    docRoot["phrase"] = docPhrase;
    docRoot["version"] = docVersion;
    docRoot["intervals"] = docIntervals;
    docRoot["shades"] = docShades;

    QJsonDocument document;
    document.setObject(docRoot);

    phraseFile.write(document.toJson());
    if (!phraseFile.flush())
        return false;

    return true;
}

bool Phrase::load(QFile &phraseFile)
{
    QString phraseFileName = phraseFile.fileName();
    QDir phraseFileDir = QFileInfo(phraseFileName).dir();

    if (!phraseFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QJsonParseError jsonParseError;
    QJsonDocument doc = QJsonDocument::fromJson(phraseFile.readAll(), &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError)
        return false;

    QJsonObject docRoot = doc.object();

    QJsonArray docVersion = docRoot["version"].toArray();
    for (const QJsonValue &jsonElement : docVersion) {
        QJsonObject objVersion = jsonElement.toObject();
        int l_version_major = objVersion["major"].toInt();
        int l_version_minor = objVersion["minor"].toInt();
        int l_version_patch = objVersion["patch"].toInt();
        cout << "phrase version " << l_version_major << "." << l_version_minor << "." << l_version_patch << endl;
    }

    reset();

    QJsonArray docIntervals = docRoot["intervals"].toArray();
    for (const QJsonValue &jsonElement : docIntervals) {
        QJsonObject objInterval = jsonElement.toObject();
        ControlPoint l_interval;
        l_interval.value = float(objInterval["value"].toDouble());
        cout << "Interval value : " << l_interval.value << endl;
        l_interval.delay = float(objInterval["delay"].toDouble());
        cout << "Interval delay : " << l_interval.delay << endl;
        l_interval.silence = objInterval["silence"].toBool();
        cout << "Interval silence : " << l_interval.silence << endl;

        insertControlInterval(l_interval.delay,l_interval.value,l_interval.silence);
    }

    QJsonArray docShades = docRoot["shades"].toArray();
    for (const QJsonValue &jsonElement : docShades) {
        QJsonObject objShade = jsonElement.toObject();
        ControlPoint l_shade;
        l_shade.value = float(objShade["value"].toDouble());
        cout << "Shade value : " << l_shade.value << endl;
        l_shade.delay = float(objShade["delay"].toDouble());
        cout << "Shade delay : " << l_shade.delay << endl;

        insertControlShade(l_shade.delay,l_shade.value);
    }

}

void Phrase::reset()
{
    unsigned long l_max = myControlInterval .size();
    for (unsigned long i = 0; i < l_max - 1; i++) {
        myControlInterval.pop_back();
    }
    l_max = myControlShade .size();
    for (unsigned long i = 0; i < l_max - 1; i++) {
        myControlShade.pop_back();
    }
}

void Phrase::shiftControlInterval(unsigned long l_num, float l_value)
{
    unsigned long l_max = myControlInterval.size();
    float l_valueMin = 0;
    float n_value = 0;
    if (l_value < 0) {
        if (l_num > 1) {
            l_valueMin = myControlInterval[(l_num - 1)]->delay;
        }
        if (l_valueMin > (myControlInterval[l_num]->delay + l_value)) {
            n_value = l_valueMin - myControlInterval[l_num]->delay;
        }
        else {
            n_value = l_value;
        }
    }
    else {
        n_value = l_value;
    }
    //cout << "l_num = "<< l_num << ", l_value = " << l_value << ", n_value = " << n_value << ", l_valueMin = " << l_valueMin << endl;
    for (unsigned long i = l_num; i < l_max; i++) {
        myControlInterval[i]->delay = myControlInterval[i]->delay + n_value;
    }
}

void Phrase::shiftControlShade(unsigned long l_num, float l_value)
{
    unsigned long l_max = myControlShade.size();
    float l_valueMin = 0;
    float n_value = 0;
    if (l_value < 0) {
        if (l_num > 1) {
            l_valueMin = myControlShade[(l_num - 1)]->delay;
        }
        if (l_valueMin > (myControlShade[l_num]->delay + l_value)) {
            n_value = l_valueMin - myControlShade[l_num]->delay;
        }
        else {
            n_value = l_value;
        }
    }
    else {
        n_value = l_value;
    }
    //cout << "l_num = "<< l_num << ", l_value = " << l_value << ", n_value = " << n_value << ", l_valueMin = " << l_valueMin << endl;
    for (unsigned long i = l_num; i < l_max; i++) {
        myControlShade[i]->delay = myControlShade[i]->delay + n_value;
    }
}
