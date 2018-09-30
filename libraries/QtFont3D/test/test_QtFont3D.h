/* --------------------------------------------------------------------------------
 * Example test for QtFont3D.
 *
 * Author: Marcel Paz Goldschen-Ohm
 * Email: marcel.goldschen@gmail.com
 * -------------------------------------------------------------------------------- */

#ifndef __test_QtFont3D_H__
#define __test_QtFont3D_H__

#include "QtFont3D.h"

#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QVector3D>

class Viewer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
    
public:
    Viewer(QWidget *parent = 0);
    ~Viewer() { makeCurrent(); }
    
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE; // Set _lastPos.
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE; // Rotate text.
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE; // Zoom text.
    
    void perspective(double fovy, double aspect, double zNear, double zFar);
    void lookAt(const QVector3D &eye, const QVector3D &center, const QVector3D &up);
    
    QVector3D _eye, _center, _up;
    QPoint _lastPos;
    QtFont3D _text3dRenderer;
};

#endif // __test_QtFont3D_H__
