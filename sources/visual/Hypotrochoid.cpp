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
#include "Hypotrochoid.h"

Hypotrochoid::Hypotrochoid(double c_speed, double c_xOr, double c_yOr, double c_radius, double c_radiusInt, double c_expansion, double c_angle, double c_progress)
                            : Trajectory(c_speed,c_xOr,c_yOr)
{
    //std::cout << "create hypo " << std::endl;
    pt2d  orig = getOrigin();
    centerX = orig.x;
    centerY = orig.y;
    setRadius(c_radius);
    setRadiusInt(c_radiusInt);
    setDistanceToCenter(0);
    setAngle(c_angle);
    setExpansion(c_expansion);
    setProgress(c_progress);
}

double Hypotrochoid::getCenterX()
{
    return centerX;
}

double Hypotrochoid::getCenterY()
{
    return centerY;
}

pt2d Hypotrochoid::computeTrajectory(double dt)
{
    double sp = this->getSpeed();
    double ph = this->getPhase();
    const double PI  =3.141592653589793238463;
    ph += sp * dt;
    //define the phase modulo the period of the trajectory to avoid having phase going to infinity after maxTurn
    if ((ph > maxTurn) & ((ph - int(ph)) * 100 < 1))
        setPhase(ph - int(ph));
    else
        setPhase(ph);

    pt2d orig = getOrigin();
    pt2d vecPos{0., 0.};

    //see Euler spiral for a smooth passage from a spiral to an hypotrochoid
    if (getDistanceToCenter() < 1)
        setDistanceToCenter(getDistanceToCenter() + 1/getProgress());
    else
        setDistanceToCenter(1);

    //the modulus and the angle, a polar complex number

    double progresAngle = (getPhase() - (getAngle()/360) + 0.25) * 2 * PI ;

    vecPos.x = orig.x + (getRadius() - getRadiusInt()) * getDistanceToCenter() * cos(progresAngle) + getExpansion() * getDistanceToCenter() * cos ((getRadius()
                      - getRadiusInt()) * getDistanceToCenter() / getRadiusInt() / getDistanceToCenter() * progresAngle);
    vecPos.y = orig.y + (getRadius() - getRadiusInt()) * getDistanceToCenter() * sin(progresAngle) - getExpansion() * getDistanceToCenter() * sin ((getRadius()
                      - getRadiusInt()) * getDistanceToCenter() / getRadiusInt() / getDistanceToCenter() * progresAngle);

    return vecPos;
}

Hypotrochoid::~Hypotrochoid()
{

}

int Hypotrochoid::getType()
{
    return HYPOTROCHOID;
}
