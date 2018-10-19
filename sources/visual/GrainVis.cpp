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
//  GrainVis.cpp
//  Frontières
//
//  Created by Christopher Carlson on 11/20/11.
//

#include "GrainVis.h"
#include "utility/GTime.h"

// graphics includes
#ifdef __MACOSX_CORE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <math.h>

// GRAPHICS--------------------------------------------------------------------------------------//

// destructor
GrainVis::~GrainVis()
{
}


// constructor
GrainVis::GrainVis(float x, float y)
{
    gX = x;
    gY = y;
    colR = 1.0;
    colB = 1.0;
    colG = 1.0;
    colA = 0.6;
    defG = colG;
    defB = colB;
    defSize = 10.0f;
    mySize = defSize;
    onSize = 30.0f;
    isOn = false;
    firstTrigger = false;
    startTime = GTime::instance().sec;
    triggerTime = 0.0;
    // TODO:  colors
}


// draw method
void GrainVis::draw()
{
    double t_sec = GTime::instance().sec - triggerTime;
    if (firstTrigger == true) {
        // slew size
        double mult = 0.0;
        if (isOn == true) {
            mult = exp(-t_sec / (0.8 * durSec));
            colG = mult * colG;
            colB = mult * colB;
            mySize = defSize + (1.0 - mult) * (onSize - defSize);
            if (colB < 0.001)
                isOn = false;
        }
        else {
            mult = 1.0 - exp(-t_sec / (0.2 * durSec));
            colG = mult * defG;
            colB = mult * defB;
            mySize = defSize + (1.0 - mult) * (onSize - defSize);
        }
    }


    glColor4f(colR, colG, colB, colA);

    // disk version - for graphics cards that don't support GL_POINT_SMOOTH
    //       glTranslatef((GLfloat)gX,(GLfloat)gY,0.0);
    //       gluDisk(gluNewQuadric(),0,mySize*0.5f, 64,1);

    // end disk version


    // point version (preferred - better quality)
    float prevSize = 1.0f;
    glGetFloatv(GL_POINT_SIZE, &prevSize);
    glPointSize(mySize);
    glBegin(GL_POINTS);
    glVertex3f(gX, gY, 0.0);
    glEnd();
    glPointSize(prevSize);
    //    //end point version
}

void GrainVis::trigger(float theDur)
{
    isOn = true;
    if (firstTrigger == false)
        firstTrigger = true;
    durSec = theDur * 0.001;
    triggerTime = GTime::instance().sec;
}

// move to
void GrainVis::moveTo(float x, float y)
{
    gX = x;
    gY = y;
}

float GrainVis::getX()
{
    return gX;
}
float GrainVis::getY()
{
    return gY;
}
