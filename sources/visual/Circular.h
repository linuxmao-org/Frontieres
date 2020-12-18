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
#ifndef CIRCULAR_H
#define CIRCULAR_H

#include <memory>
#include <iostream>
#include "theglobals.h"
#include "Trajectory.h"
#include <complex>


class Circular : public Trajectory {
public:
    Circular(double s, double xOr, double yOr, double r, double ang, double stch, double prgs);
    double getCenterX();
    double getCenterY();
    pt2d computeTrajectory(double dt);
    ~Circular();
    int getType();

private:
    double centerX;
    double centerY;
};

#endif
