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
#ifndef PHRASE_H
#define PHRASE_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <QtCore>
#include <QtGui>
#include <theglobals.h>
#include "utility/GTime.h"
#include "model/Scale.h"
#include "model/Adsr.h"


using namespace std;

struct ControlPoint
{
    float delay = 0;
    float value = 0;
    bool silence = false;
};

class Phrase
{
public:
    Phrase();
    // destructor
    virtual ~Phrase();
    ControlPoint getControlShade(unsigned long l_num);
    ControlPoint getControlInterval(unsigned long l_num);
    void setControlInterval(unsigned long l_num, float l_delay, float l_value);
    void setControlShade(unsigned long l_num, float l_delay, float l_value);
    int insertControlShade(float l_delay, float l_value);
    int insertControlInterval(float l_delay, float l_value, bool l_silence);
    void deleteControlShade(unsigned long l_num);
    void deleteControlInterval(unsigned long l_num);
    float getShade ();
    float getInterval ();
    unsigned long getMyControlShadeSize();
    unsigned long getMyControlIntervalSize();
    void debugListControlShade ();
    void debugListControlInterval();
    bool getActiveState();
    void setActiveState(bool l_activeState);
    double getPhraseStartTime();
    void setPhraseStartTime();
    void setTempo(int l_tempo);
    int getTempo();
    bool getEndedState();
    void setEndedState(bool n_state);
    bool getSilenceState();
    void setSilence (unsigned long l_num, bool l_silence);
    void setRestart (bool l_restart);
    bool getRestart();
    bool getRelease();
    bool getAttack();
    void actuasiseReleaseAndAttack();
    string askNamePhrase(FileDirection direction);
    bool save(QFile &phraseFile);
    bool load(QFile &phraseFile);
    void reset();
    Scale* getScale();
    bool scaleAttraction();
    void setScaleAttraction(bool n_state);
    void insertScalePosition(ScalePosition n_scalePosition);

private:
    bool activeState = true;
    bool silenceState = false;
    bool release = false;
    bool released = false;
    bool attack = false;
    bool attacked = false;
    int tempo = 60;
    vector<ControlPoint *> myControlShade;
    vector<ControlPoint *> myControlInterval;
    double phraseStartTime = 0;
    int currentShadeIndice = 0;
    int currentIntervalIndice = 0;
    float lastDelay = 0;
    float lastShade = 1;
    float lastInterval = 0;
    bool endedState = true;
    bool intervalEndedState = false;
    bool shadeEndedState = false;
    bool restart = true;

    bool myScaleAttraction = false;
    Scale myScale;

    // envelope

    float *envelopeVolumeBuff;
    double *intermediateBuff;
    std::atomic<int> envelopeAction;
    Env *envelopeVolume;


};

#endif // PHRASE_H
