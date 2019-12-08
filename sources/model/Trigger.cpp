#include "model/Trigger.h"
#include "model/Cloud.h"
#include "model/ParamCloud.h"
#include "visual/TriggerVis.h"
#include "visual/CloudVis.h"
#include "utility/GTime.h"
#include "model/Scene.h"
#include <QMessageBox>

extern CloudParams g_defaultCloudParams;

// ids
static unsigned int triggerId = 0;

Trigger::~Trigger()
{
    actionIn = NOTHING;
    actionOut = NOTHING;
}

Trigger::Trigger()
{
    // trigger id
    myId = ++triggerId;

    // default trajectory type
    trajectoryType = g_defaultCloudParams.trajectoryType;
    changed_trajectoryType = true;

    setActiveState(g_defaultCloudParams.activateState);

    setActiveRestartTrajectory(g_defaultCloudParams.activateRestartTrajectory);

    setIn(g_defaultCloudParams.triggerIn);
    setOut(g_defaultCloudParams.triggerOut);

    trajectoryType = STATIC;
}

void Trigger::setTrajectoryType(int l_TrajectoryType)
{
    if (locked) {
        showMessageLocked();
        return;
    }
    trajectoryType = l_TrajectoryType;
    myTriggerVis->setTrajectoryType(l_TrajectoryType);
    changed_trajectoryType = true;
}

int Trigger::getTrajectoryType()
{
    return trajectoryType;
}

bool Trigger::changedTrajectoryType()
{
    return changed_trajectoryType;
}

unsigned int Trigger::getId()
{
    return myId;
}

void Trigger::setId(int triggerId)
{
    myId = triggerId;
}

void Trigger::setName(QString newName)
{
    myName = newName;
}

QString Trigger::getName()
{
    return myName;
}

void Trigger::registerTriggerVis(TriggerVis *triggerVisToRegister)
{
    myTriggerVis = triggerVisToRegister;
}

void Trigger::toggleActive()
{
    setActiveState(!isActive);
    myTriggerVis->setIsPlayed(isActive);
}

void Trigger::setActiveState(bool activateState)
{
    isActive = activateState;
}

bool Trigger::getActiveState()
{
    return isActive;
}

void Trigger::describe(ostream &out)
{
    out << "- active : " << getActiveState() << "\n";
}

void Trigger::setLockedState(bool newLockedState)
{
    locked = newLockedState;
}

bool Trigger::getLockedState()
{
    return locked;
}

bool Trigger::dialogLocked()
{
    if (!locked)
        return false;

    QMessageBox msgBox;
    msgBox.setText("This trigger is locked, impossible to change parameters.");
    msgBox.setInformativeText("Do you want to unlock the trigger ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    locked = msgBox.exec() == QMessageBox::No;
    return locked;
}

void Trigger::changesDone(bool done)
{
    changed_trajectoryType = done;
    changed_In = done;
    changed_Out = done;
}

void Trigger::showMessageLocked()
{
    cout << "trigger locked, no change" << endl;
}

void Trigger::showParameters()
{
    cout << "trigger parameters :" << endl;
    cout << "id:" << myId << endl;
    cout << "locked = " << locked << endl;
    cout << "x = " << myTriggerVis->getX() << endl;
    cout << "y = " << myTriggerVis->getY() << endl;
    cout << "extent = " << myTriggerVis->getZoneExtent() << endl;
    cout << "trajectoryType = " << myTriggerVis->getTrajectoryType() << endl;
    switch (myTriggerVis->getTrajectoryType())  {
    case RECORDED: {
        Recorded *tr_show = dynamic_cast<Recorded*>(myTriggerVis->getTrajectory());
        cout << "positions : " << tr_show->lastPosition() << endl;
        for (int i = 1; i < tr_show->lastPosition(); i = i + 1) {
            cout << "position " << i << ", x = " << tr_show->getPosition(i).x
                 << ", y = " << tr_show->getPosition(i).y << ", delay = " << ", x = " << tr_show->getPosition(i).delay << endl;
        }
    }
        break;
    default:
        break;
    }
}

void Trigger::setActiveRestartTrajectory(bool l_choice)
{
    if (activeRestartTrajectory != l_choice)
        changed_ActiveRestartTrajectory = true;
    activeRestartTrajectory = l_choice;
}

bool Trigger::getActiveRestartTrajectory()
{
    return activeRestartTrajectory;
}

bool Trigger::changedActiveRestartTrajectory()
{
    return changed_ActiveRestartTrajectory;
}

void Trigger::setIn(int n_In)
{
    actionIn = n_In;
    changed_In = true;
}

int Trigger::getIn()
{
    return actionIn;
}

bool Trigger::changedIn()
{
    return changed_In;
}

void Trigger::setOut(int n_Out)
{
    actionOut = n_Out;
    changed_Out= true;
}

int Trigger::getOut()
{
    return actionOut;
}

bool Trigger::changedOut()
{
    return changed_Out;
}

void Trigger::computeCloudsIn()
{
    if (!isActive)
        return;
    Scene *scene = ::currentScene;
    for (int i = 0; i < scene->m_clouds.size(); i = i + 1) {
        //cout << "i=" << i << endl;
        SceneCloud *cloud = scene->m_clouds[i].get();
        Cloud *cloudToCompute = cloud->cloud.get();
        CloudVis *cloudVisToCompute = cloud->view.get();
        if (listCloudsIn.indexOf(cloudToCompute->getId()) == -1) {
            //cout << "recherche, id =" << cloudToCompute->getId() << endl;
            int cloudExtent;
            if (cloudVisToCompute->getXRandExtent() > cloudVisToCompute->getYRandExtent())
                cloudExtent = cloudVisToCompute->getXRandExtent();
            else
                cloudExtent = cloudVisToCompute->getYRandExtent();
            int x1 = cloudVisToCompute->getOriginX();
            int y1 = cloudVisToCompute->getOriginY();
            int x2 = myTriggerVis->getX();
            int y2 = myTriggerVis->getY();
            double distance = (qSqrt((pow((x1 - x2),2)+pow((y1 - y2),2))));
            double radiusSum = cloudExtent + minZone + myTriggerVis->getZoneExtent() + minZone;
            //cout << "distance =" << distance << ", rayons =" << radiusSum << endl;
            if (distance > radiusSum) {
                //cout << "dehors" << endl;
            }
            else {
                //cout << "dedans" << endl;
                listCloudsIn << cloudToCompute->getId();
                switch (getIn()) {
                case NOTHING:
                    //cout << " NOTHING" << endl;
                    break;
                case ON:
                    //cout << " ON" << endl;
                    cloudToCompute->setActiveState(true);
                    cloudVisToCompute->setIsPlayed(true);
                    break;
                case OFF:
                    //cout << " OFF" << endl;
                    cloudToCompute->setActiveState(false);
                    cloudVisToCompute->setIsPlayed(false);
                    break;
                case COMMUTE:
                    //cout << " COMMUTE" << endl;
                    cloudToCompute->toggleActive();
                    break;
                default:
                    break;
                }
            }
        }
        else {
            //cout << "déjà dedans" << endl;
        }
    }
}

void Trigger::computeCloudsOut()
{
    if (!isActive)
        return;
    Scene *scene = ::currentScene;
    for (int i = 0; i < listCloudsIn.size(); i = i + 1) {
        //cout << " out i=" << i << ", id=" << listCloudsIn[i] << endl;
        SceneCloud *cloud = scene->findCloudById(listCloudsIn[i]);
        Cloud *cloudToCompute = cloud->cloud.get();
        CloudVis *cloudVisToCompute = cloud->view.get();
        //cout << "recherche out, id =" << cloudToCompute->getId() << endl;
        int cloudExtent;
        if (cloudVisToCompute->getXRandExtent() > cloudVisToCompute->getYRandExtent())
            cloudExtent = cloudVisToCompute->getXRandExtent();
        else
            cloudExtent = cloudVisToCompute->getYRandExtent();
        int x1 = cloudVisToCompute->getOriginX();
        int y1 = cloudVisToCompute->getOriginY();
        int x2 = myTriggerVis->getX();
        int y2 = myTriggerVis->getY();
        double distance = (qSqrt((pow((x1 - x2),2)+pow((y1 - y2),2))));
        double radiusSum = cloudExtent + minZone + myTriggerVis->getZoneExtent() + minZone;
        //cout << "distance =" << distance << ", rayons =" << radiusSum << endl;
        if (distance > radiusSum) {
            //cout << "dehors" << endl;
            listCloudsIn.removeAll(cloudToCompute->getId());
            switch (getOut()) {
            case NOTHING:
                //cout << " NOTHING" << endl;
                break;
            case ON:
                //cout << " ON" << endl;
                cloudToCompute->setActiveState(true);
                break;
            case OFF:
                //cout << " OFF" << endl;
                cloudToCompute->setActiveState(false);
                break;
            case COMMUTE:
                //cout << " COMMUTE" << endl;
                cloudToCompute->setActiveState(!cloudToCompute->getActiveState());
                break;
            default:
                break;
            }
        }
        else {
            //cout << "dedans" << endl;
        }
    }
}
