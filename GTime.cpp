//------------------------------------------------------------------------------
// BORDERLANDS:  An interactive granular sampler.
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
//  GTime.cpp
//  Borderlands
//
//  Created by Christopher Carlson on 12/2/11.
//

#include "GTime.h"

GTime::~GTime()
{
}

GTime::GTime()
{
    sec = (double)0.0;
}

GTime &GTime::instance()
{
    static GTime *theInst = NULL;
    if (theInst == NULL)
        theInst = new GTime();
    return *theInst;
}
