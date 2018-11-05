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

// envelopes
struct ParamEnv {

    //** Levels **//

    // attack target level [0:1]
    float l1 = 0;
    // decay target level [0:1]
    float l2 = 0;
    // 2nd attack target level [0:1]
    float l3 = 0;

    //** Slopes in unit/sample **//
    // (multiply by sample rate to get unit/second)

    // attack slope, positive
    float r1 = 0;
    // stage slope, positive or negative
    float r2 = 0;
    // decay, positive or negative
    float r3 = 0;
    // release slope, negative
    float r4 = 0;

    // types of slopes
    enum SlopeType { Linear, Exp, Power, };
    int t1 = Linear;
    int t2 = Linear;
    int t3 = Linear;
    int t4 = Linear;
};
