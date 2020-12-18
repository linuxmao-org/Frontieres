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
#ifndef TRIGGER_H
#define TRIGGER_H

#include <Stk.h>
#include <map>
#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <atomic>
#include <cstdlib>
#include <time.h>
#include <ctime>
#include <QFile>
#include <QDir>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QTranslator>
#include <mutex>
#include "Frontieres.h"
#include "visual/Trajectory.h"
#include <QtMath>


enum { NOTHING, ON, OFF, COMMUTE};

// forward declarations
class Trigger;
class TriggerVis;

class Trigger {

public:
    // destructor
    virtual ~Trigger();

    // constructor
    Trigger();

    // trajectory type
    void setTrajectoryType(int l_TrajectoryType);
    int getTrajectoryType();
    bool changedTrajectoryType();

    // get unique id of trigger
    unsigned int getId();
    void setId(int triggerId);

    void setName(QString newName);
    QString getName();

    // register visualization
    void registerTriggerVis(TriggerVis *triggerVisToRegister);

    // turn on/off
    void toggleActive();
    void setActiveState(bool activateState);
    bool getActiveState();

    // print information
    void describe(std::ostream &out);

    // lock flag
    void setLockedState(bool newLockedState);
    bool getLockedState();
    bool dialogLocked();

    // changes done
    void changesDone(bool done);

    // message when osc or midi want to change locked cloud
    void showMessageLocked();

    // console message with all cloud parameters
    void showParameters();

    void setActiveRestartTrajectory (bool l_choice);
    bool getActiveRestartTrajectory ();
    bool changedActiveRestartTrajectory();

    void setIn(int n_In);
    int getIn();
    bool changedIn();

    void setOut(int n_Out);
    int getOut();
    bool changedOut();

    void computeCloudsIn ();
    void computeCloudsOut ();

    void computeTriggersIn ();
    void computeTriggersOut ();

    void setPriority (int l_priority);
    int getPriority ();
    bool changedPriority();
    bool isInListTriggersIn(int l_triggerId);

private:

    unsigned int myId;  // unique id

    QString myName;

    bool isActive;  // on/off state

    // registered visualization
    TriggerVis *myTriggerVis;

    // trigger params
    int trajectoryType;
    bool changed_trajectoryType = false;

    // lock switch
    bool locked = false;

    // active restart trajectory
    bool activeRestartTrajectory = false;
    bool changed_ActiveRestartTrajectory = false;

    // In and Out actions
    int actionIn, actionOut;
    bool changed_In = false;
    bool changed_Out = false;

    // List of Clouds In
    QList<int> listCloudsIn;

    // List of Triggers In
    QList<int> listTriggersIn;

    // Priority for trigger action on other triggers
    int priority = 0;
    bool changed_Priority = false;

};

#endif // TRIGGER_H
