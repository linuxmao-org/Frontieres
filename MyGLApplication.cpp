//------------------------------------------------------------------------------
// BORDERLANDS:  An interactive granular sampler.
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

#include "MyGLApplication.h"
#include "MyGLWindow.h"
#include <QTimer>
#include <QDebug>

struct MyGLApplication::Impl {
    MyGLWindow *window = nullptr;
    void onIdle();
};

MyGLApplication::MyGLApplication(int &argc, char *argv[])
    : QApplication(argc, argv),
      P(new Impl)
{
    MyGLWindow *window = new MyGLWindow;
    P->window = window;
}

MyGLApplication::~MyGLApplication()
{
}

MyGLWindow *MyGLApplication::GLwindow()
{
    return P->window;
}

void MyGLApplication::startIdleCallback(double fps)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]{ P->onIdle(); });
    timer->start(1e3 / fps);
}

void MyGLApplication::Impl::onIdle()
{
    window->screen()->update();
}
