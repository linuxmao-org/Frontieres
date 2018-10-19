//------------------------------------------------------------------------------
// FRONTIÈRES:  An interactive granular sampler.
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

#include <QMainWindow>
#include <QOpenGLWidget>
#include <memory>

class MyGLScreen;

class MyGLWindow : public QMainWindow {
public:
    MyGLWindow();
    ~MyGLWindow();
    void initialize();
    MyGLScreen *screen() const;

private:
    struct Impl;
    std::unique_ptr<Impl> P;
};

// the openGL screen
class MyGLScreen : public QOpenGLWidget {
public:
    MyGLScreen(QWidget *parent = nullptr);
    ~MyGLScreen();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePassiveMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    struct Impl;
    std::unique_ptr<Impl> P;
};
