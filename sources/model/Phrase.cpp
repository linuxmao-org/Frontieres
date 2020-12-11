#include "Phrase.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QFile>

extern int version_major, version_minor, version_patch;

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
        l_result = i;
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
        l_result = i;
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

float Phrase::getShade()
{
    float l_delay = (GTime::instance().sec - phraseStartTime) * tempo / 60;
    if ((myControlShade.size() == 1) || (!activeState)) {
        shadeEndedState = true;
        if (intervalEndedState)
            setEndedState(true);
        return myControlShade[0]->value;
    }
    else if (myControlShade[myControlShade.size() - 1]->delay < l_delay) {
        shadeEndedState = true;
        if (intervalEndedState)
            setEndedState(true);
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

                if (silenceState)
                    result = 0;
                else
                    result = firstShade + ((lastShade - firstShade) * (l_delay - firstDelay) / (lastDelay - firstDelay));
                break;
            }
        }
        return result;
    }
}

float Phrase::getInterval()
{
    //cout << "getinterval, ";
    float l_delay = (GTime::instance().sec - phraseStartTime) * tempo / 60;
    //cout << "l_delay = " << l_delay << endl;
    if ((myControlInterval.size() == 1) || (!activeState)) {
        silenceState = false;
        intervalEndedState = true;
        if (shadeEndedState)
            setEndedState(true);
        actuasiseReleaseAndAttack();
        return myControlInterval[0]->value;
    }
    else {
        if (myControlInterval[myControlInterval.size() - 1]->delay < l_delay) {
            silenceState = myControlInterval[myControlInterval.size() - 1]->silence;
            intervalEndedState = true;
            if (shadeEndedState)
                setEndedState(true);
            actuasiseReleaseAndAttack();
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
                    silenceState = myControlInterval[i]->silence;
                    actuasiseReleaseAndAttack();
                    break;
                }
            }
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
    //cout << "entree getMyControlIntervalSize, myControlInterval.size()=" << myControlInterval.size() << endl;
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

bool Phrase::getActiveState()
{
    return activeState;
}

void Phrase::setActiveState(bool l_activeState)
{
    if (l_activeState) {
        if (endedState) {
            setPhraseStartTime();
        }
    }
    activeState = l_activeState;
}

double Phrase::getPhraseStartTime()
{
    return phraseStartTime;
}

void Phrase::setPhraseStartTime()
{
    phraseStartTime = GTime::instance().sec;
    intervalEndedState = false;
    shadeEndedState = false;
    setEndedState(false);
}

void Phrase::setTempo(int l_tempo)
{
    tempo = l_tempo;
}

int Phrase::getTempo()
{
    return tempo;
}

bool Phrase::getEndedState()
{
    return endedState;
}

void Phrase::setEndedState(bool n_state)
{
    endedState = n_state;
}

bool Phrase::getSilenceState()
{
    return silenceState;
}

void Phrase::setSilence(unsigned long l_num, bool l_silence)
{
    myControlInterval[l_num]->silence = l_silence;
}

void Phrase::setRestart(bool l_restart)
{
    restart = l_restart;
}

bool Phrase::getRestart()
{
    return restart;
}

bool Phrase::getRelease()
{
    return release;
}

bool Phrase::getAttack()
{
    return attack;
}

void Phrase::actuasiseReleaseAndAttack()
{
    //cout << "actualise release =" << release << "," << released << ",attack =" << attack <<"," << attacked<< endl;
    if (silenceState) {
      //  cout << "silenstate"<<endl;
       if (released) {
           release = false;
       }
       else {
           release = true;
           released = true;
           attacked = false;
       }
    }
    else {
        if (attacked) {
            attack = false;
        }
        else {
            attacked = true;
            attack = true;
        }
        release = false;
        released = false;
    }
    //  cout << "actualise release =" << release << "," << released << ",attack =" << attack <<"," << attacked<< endl;
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

    // Positions
    QJsonArray docIntervals;
    cout << myControlInterval.size() << " intervals : " << "\n";
    for (int i = 1, n = myControlInterval.size(); i < n; ++i) {

        cout << "Phrase interval " << i << ", delay : ";
        cout << myControlInterval[i]->delay << ",value :" << myControlInterval[i]->value << ", silence :" << myControlInterval[i]->silence << "\n";

        QJsonObject objInterval;

        objInterval["value"] = myControlInterval[i]->value;
        objInterval["delay"] = myControlInterval[i]->delay;
        objInterval["silence"] = myControlInterval[i]->silence;

        docIntervals.append(objInterval);

    }
    QJsonArray docShades;
    cout << myControlShade.size() << " shades : " << "\n";
    for (int i = 1, n = myControlShade.size(); i < n; ++i) {

        cout << "Phrase shade " << i << ", delay : ";
        cout << myControlShade[i]->delay << ",value :" << myControlShade[i]->value << "\n";

        QJsonObject objShade;

        objShade["value"] = myControlShade[i]->value;
        objShade["delay"] = myControlShade[i]->delay;


        docShades.append(objShade);

    }

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
        l_interval.value = objInterval["value"].toDouble();
        cout << "Interval value : " << l_interval.value << endl;
        l_interval.delay = objInterval["delay"].toDouble();
        cout << "Interval delay : " << l_interval.delay << endl;
        l_interval.silence = objInterval["silence"].toBool();
        cout << "Interval silence : " << l_interval.silence << endl;

        insertControlInterval(l_interval.delay,l_interval.value,l_interval.silence);
    }

    QJsonArray docShades = docRoot["shades"].toArray();
    for (const QJsonValue &jsonElement : docShades) {
        QJsonObject objShade = jsonElement.toObject();
        ControlPoint l_shade;
        l_shade.value = objShade["value"].toDouble();
        cout << "Shade value : " << l_shade.value << endl;
        l_shade.delay = objShade["delay"].toDouble();
        cout << "Shade delay : " << l_shade.delay << endl;

        insertControlShade(l_shade.delay,l_shade.value);
    }

}

void Phrase::reset()
{
    int l_max = myControlInterval .size();
    for (unsigned long i = 0; i < l_max - 1; i++) {
        myControlInterval.pop_back();
    }
    l_max = myControlShade .size();
    for (unsigned long i = 0; i < l_max - 1; i++) {
        myControlShade.pop_back();
    }
}
