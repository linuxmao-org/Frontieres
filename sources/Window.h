//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2011  Christopher Carlson
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
//  Window.h
//  Frontières
//
//  Contains elements of chuck_fft by Ge Wang and Perry Cook
//  Created by Christopher Carlson on 11/13/11.
//


#ifndef WINDOW_H
#define WINDOW_H

#include <cmath>
#include "math.h"
#include "assert.h"
#include "theglobals.h"
#include <iostream>
#include <Stk.h>

using namespace std;

enum { HANNING, TRIANGLE, EXPDEC, REXPDEC, SINC, RANDOM_WIN };

class Window {
public:
    static Window &Instance();


    // return window
    double *getWindow(unsigned int windowType);
    // resize windows - future possibility, but probably not needed
    // void resizeWindows(unsigned long length);

    int numWindows();

protected:
    // generate windows
    void generateWindows(unsigned long length);
    // window function prototypes
    void hanning(double *window, unsigned long length);
    // void trapezoid( double * window, unsigned long length );
    void triangle(double *window, unsigned long length);
    void expdec(double *forWin, double *revWin, unsigned long length);
    void sinc(double *window, unsigned long length, int numZeroCross = 6);


private:
    ~Window();
    Window(unsigned long length = 2048);
    double *hanningWin;
    // double * trapWin;
    double *triWin;
    double *expDecWin;
    double *rexpDecWin;
    double *sincWin;
};


#endif
