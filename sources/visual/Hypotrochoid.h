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
#ifndef HYPOTROCHOID_H
#define HYPOTROCHOID_H

#include <memory>
#include <iostream>
#include "theglobals.h"
#include "Trajectory.h"


class Hypotrochoid : public Trajectory {
public:
    Hypotrochoid(double c_speed, double c_xOr, double c_yOr, double c_radius, double c_radiusInt, double c_expansion, double c_angle, double c_progress);
    double getCenterX();
    double getCenterY();
    void setCenter(double x,double y);
    pt2d computeTrajectory(double dt);
    ~Hypotrochoid();
    int getType();

private:
    double centerX;
    double centerY;
    const int maxTurn = 1000;
};

#endif // HYPOTROCHOID_H
