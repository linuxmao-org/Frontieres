//------------------------------------------------------------------------------
// FRONTIERES:  An interactive granular sampler.
//------------------------------------------------------------------------------
// More information is available at
//     http::/ccrma.stanford.edu/~carlsonc/256a/Borderlands/index.html
//
//
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

#include <QCoreApplication>

#if !defined(FRONTIERES_DISABLE_I18N)

// translate to QString
#define _Q(c, x) (QCoreApplication::translate(c, x))
// translate to std::string
#define _S(c, x) (_Q(c, x).toStdString())

#else  // !defined(FRONTIERES_DISABLE_I18N)

#define _Q(c, x) QString(x)
#define _S(c, x) std::string(x)

#endif  // !defined(FRONTIERES_DISABLE_I18N)
