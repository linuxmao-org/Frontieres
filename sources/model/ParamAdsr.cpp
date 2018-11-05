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

#include "ParamAdsr.h"

void ParamEnv::setTimeBasedParameters(
    float l1, float l2, float l3,
    float tAtk, float tSta, float tDec, float tRel,
    float sampleRate)
{
    this->l1 = l1;
    this->l2 = l2;
    this->l3 = l3;

    // the linear slopes r=dy/dt
    r1 = l1 / (tAtk * sampleRate);
    r2 = (l2 - l1) / (tSta * sampleRate);
    r3 = (l3 - l2) / (tDec * sampleRate);
    r4 = -l3 / (tRel * sampleRate);
}

void ParamEnv::getTimeBasedParameters(
    float &tAtk, float &tSta, float &tDec, float &tRel,
    float sampleRate) const
{
    // dt=dy/r
    tAtk = l1 / (r1 * sampleRate);
    tSta = (l2 - l1) / (r2 * sampleRate);
    tDec = (l3 - l2) / (r3 * sampleRate);
    tRel = -l3 / (r4 * sampleRate);
}
