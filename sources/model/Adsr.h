#pragma once
#include <iostream>
#include "theglobals.h"
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
