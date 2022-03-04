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

struct LocalPhrase
{
    bool activeState = true;
    bool silenceState = false;
    bool release = false;
    bool released = false;
    bool attack = false;
    bool attacked = false;
    unsigned int tempo = 60;
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

    // envelope

    float *envelopeVolumeBuff;
    double *intermediateBuff;
    std::atomic<int> envelopeAction;
    Env *envelopeVolume;
};

class Phrase
{
public:
    Phrase();
    // destructor
    virtual ~Phrase();
    void setName(QString newName);
    QString getName();
    unsigned int getId();
    void setId(unsigned int phraseId);

    ControlPoint getControlShade(unsigned long l_num);
    ControlPoint getControlInterval(unsigned long l_num);
    void setControlInterval(unsigned long l_num, float l_delay, float l_value);
    void setControlShade(unsigned long l_num, float l_delay, float l_value);
    int insertControlShade(float l_delay, float l_value);
    int insertControlInterval(float l_delay, float l_value, bool l_silence);
    void deleteControlShade(unsigned long l_num);
    void deleteControlInterval(unsigned long l_num);
    float getShade (LocalPhrase &l_phrase);
    float getInterval (LocalPhrase &l_phrase);
    unsigned long getMyControlShadeSize();
    unsigned long getMyControlIntervalSize();
    void debugListControlShade ();
    void debugListControlInterval();
    void setActiveState(LocalPhrase &l_phrase, bool l_activeState);
    void setPhraseStartTime(LocalPhrase &l_phrase);
    void setTempo(LocalPhrase &l_phrase, int l_tempo);
    void setSilence (unsigned long l_num, bool l_silence);
    void actuasiseReleaseAndAttack(LocalPhrase &l_phrase);
    string askNamePhrase(FileDirection direction);
    bool save(QFile &phraseFile);
    bool load(QFile &phraseFile);
    void reset();
    void shiftControlInterval(unsigned long l_num, float l_value);
    void shiftControlShade(unsigned long l_num, float l_value);

private:

    unsigned int myId;  // unique id
    QString myName;

    LocalPhrase myPhrase;
/*    bool activeState = true;
    bool silenceState = false;
    bool release = false;
    bool released = false;
    bool attack = false;
    bool attacked = false;
    int tempo = 60;
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
    Env *envelopeVolume;*/
    vector<ControlPoint *> myControlShade;
    vector<ControlPoint *> myControlInterval;



};

#endif // PHRASE_H
