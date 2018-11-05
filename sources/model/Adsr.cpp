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

#include "Adsr.h"
#include <stdio.h>

//#define debug(fmt, ...) do { fprintf(stderr, fmt "\n", ##__VA_ARGS__); } while (0)
#undef debug

Env::Env()
{
}

void Env::setParam(const ParamEnv &p)
{
    param_ = p;
}

ParamEnv Env::getParam()
{
    return param_;
}

void Env::reset()
{
#ifdef debug
    if (state_ != State::Off)
        debug("%s -> %s", nameof(state_), nameof(State::Off));
#endif

    state_ = State::Off;
    rel_ = false;
    l_ = 0;
}
//void Env::trigger(unsigned vel)
void Env::trigger()
{
#ifdef debug
    if (state_ != State::Atk)
        debug("%s -> %s", nameof(state_), nameof(State::Atk));
#endif

    state_ = State::Atk;
    rel_ = false;
}
//void Env::release(unsigned vel)
void Env::release()
{
    rel_ = true;
}

auto Env::state() const -> State
{
    return state_;
}

bool Env::running() const
{
    return state_ != State::Off;
}

void Env::generate(float *outp, unsigned n)
{
    State state = state_;
    float l = l_;

    const ParamEnv &param = param_;
    const float l1 = param.l1;
    const float l2 = param.l2;
    const float l3 = param.l3;
    const float r1 = param.r1;
    const float r2 = param.r2;
    const float r3 = param.r3;
    const float r4 = param.r4;
    const int t1 = param.t1;
    const int t2 = param.t2;
    const int t3 = param.t3;
    const int t4 = param.t4;

    auto enter = [&state](State newstate) {
#ifdef debug
        if (newstate != state)
            debug("%s -> %s", nameof(state), nameof(newstate));
#endif
        state = newstate;
    };

    if (rel_)
        enter(State::Rel);

    for (unsigned i = 0; i < n; ++ i) {
        switch (state) {
        case State::Off:
            l = 0; break;
        case State::Atk: {
            if ((r1 < 0) ? (l > l1) : (l < l1)) {
                enter(State::Atk);
                l += r1;
                l = (r1 < 0) ? ((l < l1) ? l1 : l) :
                    ((l > l1) ? l1 : l);
                break;
            }
            // fall through
        }
        case State::Sta: {
            if ((r2 < 0) ? (l > l2) : (l < l2)) {
                enter(State::Sta);
                l += r2;
                l = (r2 < 0) ? ((l < l2) ? l2 : l) :
                    ((l > l2) ? l2 : l);
                break;
            }
            // fall through
        }
        case State::Dec: {
            if ((r3 < 0) ? (l > l3) : (l < l3)) {
                enter(State::Dec);
                l += r3;
                l = (r3 < 0) ? ((l < l3) ? l3 : l) :
                    ((l > l3) ? l3 : l);
                break;
            }
            // fall through
        }
        case State::Sus: {
            enter(State::Sus);
            l = l3;
            break;
        }
        case State::Rel: {
            enter(State::Rel);
            l += r4;
            l = (r4 < 0) ? ((l < 0) ? 0 : l) :
                ((l > 0) ? 0 : l);
            if (l == 0)
                enter(State::Off);
            break;
        }
        }
        outp[i] = l;
    }

    l_ = l;
    state_ = state;
}

const char *Env::nameof(State s) {
    static const char *names[] = {"Off", "Atk", "Sta", "Dec", "Sus", "Rel"};
    return names[(unsigned)s];
}
