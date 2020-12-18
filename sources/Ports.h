//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
// Copyright (C) 2018  Jean Pierre Cimalando
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

#pragma once
#include <rtosc/ports.h>

namespace Ports {

// the root of input OSC message dispatch
extern rtosc::Ports rtRoot;
// the root of input OSC message dispatch [Non-RT]
extern rtosc::Ports nonRtRoot;

// the subpath /cloud
extern rtosc::Ports rtCloud;
// the subpath /cloud [Non-RT]
extern rtosc::Ports nonRtCloud;

}  // namespace Ports
