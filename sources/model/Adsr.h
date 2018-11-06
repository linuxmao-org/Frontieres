//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2018  Olivier Flatres
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

#pragma once
#include "ParamAdsr.h"
#include <iostream>

struct Env {

    enum class State { Off, Atk, Sta, Dec, Sus, Rel };

    Env();
    void initialize();
    void setParam(const ParamEnv &p);
    ParamEnv getParam();
    void reset();
    void trigger();
    void release();
    State state() const;
    bool running() const;
    void generate(float *outp, unsigned n);
    static const char *nameof(State s);
    void envDialogShow (ParamEnv &paramEnvForDialog);
private:
    // parameters
    ParamEnv param_;
    // which state it's currently in
    State state_ = State::Off;
    // current level
    float l_ = 0;
    // whether key has been released yet
    bool rel_ = false;
};
