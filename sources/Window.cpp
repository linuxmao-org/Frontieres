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
//  Window.cpp
//  Frontières
//
//  Created by Christopher Carlson on 11/13/11.
//

#include "Window.h"


// destructor
Window::~Window()
{
    delete[] hanningWin;
    delete[] triWin;
    // delete [] trapWin;
    delete[] sincWin;
    delete[] rexpDecWin;
    delete[] expDecWin;
}


// constructor
Window::Window(unsigned long length)
{
    hanningWin = new double[length];
    triWin = new double[length];
    // trapWin = new double[length];
    expDecWin = new double[length];
    rexpDecWin = new double[length];
    sincWin = new double[length];

    // create
    generateWindows(length);
}


int Window::numWindows()
{
    return 6;
}

Window &Window::Instance()
{
    static Window *theWindow = NULL;
    if (theWindow == NULL)
        theWindow = new Window(WINDOW_LEN);

    return *theWindow;
}

//
////resize windows
// void Window::resizeWindows(unsigned long length){
//
//    if (hanningWin != NULL){
//        delete [] hanningWin;
//        hanningWin = new double[length];
//    }
//
//    if (triWin != NULL){
//        delete [] triWin;
//        triWin = new double[length];
//    }
//
//
//    if (trapWin != NULL){
//        delete [] trapWin;
//        trapWin = new double[length];
//
//
//    }
//    if (sincWin != NULL){
//        delete [] sincWin;
//        sincWin = new double[length];
//
//    }
//
//    if (rexpDecWin != NULL){
//        delete [] rexpDecWin;
//        rexpDecWin = new double[length];
//
//    }
//    if (expDecWin != NULL){
//        delete [] expDecWin;
//        expDecWin = new double[length];
//    }
//
//
//
//}

// create all windows
void Window::generateWindows(unsigned long length)
{
    // clear all windows
    for (int i = 0; i < length; i++) {
        hanningWin[i] = (double)0.0;
        triWin[i] = (double)0.0;
        // trapWin[i] = (double)0.0;
        sincWin[i] = (double)0.0;
        expDecWin[i] = (double)0.0;
        rexpDecWin[i] = (double)0.0;
    }
    hanning(hanningWin, length);
    triangle(triWin, length);
    // trapezoid(trapWin, length);
    sinc(sincWin, length, 8);
    expdec(expDecWin, rexpDecWin, length);
}


// window generation functions

//-------------------------------------------------------------------------------
// hanning / raised cosine window
//-------------------------------------------------------------------------------
void Window::hanning(double *window, unsigned long length)
{
    assert(length > 0);
    unsigned long i;
    double pi, phase = 0, delta;

    delta = 2 * PI / (double)length;

    for (i = 0; i < length; i++) {
        window[i] = (double)(0.5 * (1.0 - cos(phase)));
        phase += delta;
    }
}


//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
// void Window::trapezoid( double * window, unsigned long length )
//{
//    assert(length > 0);
//
//}


//-------------------------------------------------------------------------------
// triangle window
//-------------------------------------------------------------------------------
void Window::triangle(double *window, unsigned long length)
{
    assert(length > 0);

    double norm = (2.0 / ((double)length - 1));
    double invnorm = 1.0 / norm;

    for (int i = 0; i < length; i++) {
        window[i] = norm * (invnorm - fabs(i - invnorm));
    }
}


//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
void Window::expdec(double *forWin, double *revWin, unsigned long length)
{
    assert(length > 0);

    // create decaying exponential (forward and reverse)

    // exp time constant (in samples)
    double tau = 512.0;
    double tauInv = 1.0 / tau;
    double eps = 0.0005;
    unsigned long transition = length - 48;

    for (int i = 0; i < length; i++) {
        forWin[i] = exp(-((double)i) * tauInv);
        if (revWin)
            revWin[length - i - 1] = forWin[i];
    }
}


//-------------------------------------------------------------------------------
// SINC window with flexible number of zero crossings
//-------------------------------------------------------------------------------
void Window::sinc(double *window, unsigned long length, int numZeroCross)
{
    // note - numZeroCross should be even number, otherwise window will shift.
    // note also - numZeroCross = 1 is Lanczos window - main lobe
    double inc = numZeroCross / (double)length;
    double x = -(numZeroCross) / 2.0;
    // std::cout << numZeroCross << endl;

    for (int i = 0; i < length; i++) {
        if (x != 0) {
            sincWin[i] = fabs(sin(PI * x) / (PI * x));
        }
        else {
            sincWin[i] = (double)1.0;
        }

        x += inc;
    }
}


// return pointer to required window
double *Window::getWindow(unsigned int windowType)
{
    switch (windowType) {
    case HANNING:
        return hanningWin;
        break;
    case TRIANGLE:
        // cout << "triangle-selected"<<endl;
        return triWin;
        break;
        //        case TRAPEZOID:
        //            return trapWin;
        //            break;
    case EXPDEC:
        return expDecWin;
        break;
    case REXPDEC:
        return rexpDecWin;
        break;
    case SINC:
        return sincWin;
        break;
    default:
        return hanningWin;
        break;
    }
}
