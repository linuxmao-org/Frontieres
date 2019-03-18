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

#ifndef MYGLWINDOW_H
#define MYGLWINDOW_H

#include <QMainWindow>
#include <QOpenGLWidget>
#include <memory>

class MyGLScreen;

class MyGLWindow : public QMainWindow {
    Q_OBJECT

public:
    MyGLWindow();
    ~MyGLWindow();
    void initialize();
    void closeEvent(QCloseEvent *e);
    void setMenuBarVisible(bool visible);
    MyGLScreen *screen() const;

public slots:
    void on_actionAbout_triggered();
    void on_actionUserManual_triggered();

private:
    struct Impl;
    std::unique_ptr<Impl> P;
};

// the openGL screen
class MyGLScreen : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit MyGLScreen(QWidget *parent = nullptr);
    ~MyGLScreen();

public slots:
    void keyAction_FullScreen();
    void keyAction_SpatialMode(int dir = +1);
    void keyAction_Overlap(int dir = +1);
    void keyAction_MotionX();
    void keyAction_MotionY();
    void keyAction_MotionXY();
    void keyAction_Direction(int dir = +1);
    void keyAction_Window(int dir = +1);
    void keyAction_Volume(int dir = +1);
    void keyAction_Cloud(int dir = +1);
    void keyAction_Grain(int dir = +1);
    void keyAction_Duration(int dir = +1);
    void keyAction_LfoFreq(int dir = +1);
    void keyAction_LfoAmount(int dir = +1);
    void keyAction_Pitch(int dir = +1);
    void keyAction_Active();
    void keyAction_EditEnvelope();
    void keyAction_EditCloud();

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

#endif
