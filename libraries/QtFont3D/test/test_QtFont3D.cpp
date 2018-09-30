/* --------------------------------------------------------------------------------
 * Example test for QtFont3D.
 *
 * Author: Marcel Paz Goldschen-Ohm
 * Email: marcel.goldschen@gmail.com
 * -------------------------------------------------------------------------------- */

#include "test_QtFont3D.h"

#include <cmath>

#include <QApplication>
#include <QMouseEvent>

Viewer::Viewer(QWidget *parent) :
QOpenGLWidget(parent),
_eye(0, 0, 20),
_center(0, 0, 0),
_up(0, 1, 0)
{
}

void Viewer::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.75, 0.75, 0.75, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    
    // We need a valid OpenGL context before building the 3D font display lists.
    float glyphWidth = 3;
    int numGlyphs = 256;
    QFont font = QFont("Sans", 20, QFont::Normal);
    _text3dRenderer.setFont(font, glyphWidth, numGlyphs);
}

void Viewer::resizeGL(int w, int h)
{
    glViewport(0, 0, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    perspective(10, float(w) / h, 2, 200);
    glMatrixMode(GL_MODELVIEW);
}

void Viewer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    lookAt(_eye, _center, _up);
    
    // Draw 3D text.
    QString text = "Rotate with Mouse!";
    QVector3D position(0, 0, 0);
    int align = Qt::AlignHCenter | Qt::AlignVCenter;
    float scale = float(20) / 1200; // This just happens to be appropriate for this particular scene's perspective view.
    glColor3f(0, 0, 1); // Text is rendered using the current color.
    _text3dRenderer.print(text, position, align, scale);
}

void Viewer::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        _lastPos = event->pos();
    QOpenGLWidget::mousePressEvent(event);
}

void Viewer::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton) {
        // Rotate scene.
        float dx = event->pos().x() - _lastPos.x();
        float dy =event->pos().y() - _lastPos.y();
        QVector3D zhat = _eye - _center;
        zhat.normalize();
        QVector3D yhat = _up;
        QVector3D xhat = QVector3D::crossProduct(yhat, zhat);
        yhat = QVector3D::crossProduct(zhat, xhat);
        QVector3D rotationAxis = yhat * dx + xhat * dy;
        float n = rotationAxis.length();
        if(n > 1e-5) {
            rotationAxis.normalize();
            float radians = n / width() * M_PI;
            // Rotate eye about center around rotation axis.
            float a = cos(radians / 2);
            float s = -sin(radians / 2);
            float b = rotationAxis.x() * s;
            float c = rotationAxis.y() * s;
            float d = rotationAxis.z() * s;
            float rotationMatrix[9] = {
                a*a+b*b-c*c-d*d, 2*(b*c-a*d), 2*(b*d+a*c),
                2*(b*c+a*d), a*a+c*c-b*b-d*d, 2*(c*d-a*b),
                2*(b*d-a*c), 2*(c*d+a*b), a*a+d*d-b*b-c*c};
            _eye -= _center; // Shift center to origin so can rotate eye about axis through the origin.
            // Rotate eye around rotation axis.
            float ex = _eye.x() * rotationMatrix[0] + _eye.y() * rotationMatrix[1] + _eye.z() * rotationMatrix[2];
            float ey = _eye.x() * rotationMatrix[3] + _eye.y() * rotationMatrix[4] + _eye.z() * rotationMatrix[5];
            float ez = _eye.x() * rotationMatrix[6] + _eye.y() * rotationMatrix[7] + _eye.z() * rotationMatrix[8];
            _eye = QVector3D(ex, ey, ez);
            _eye += _center; // shift back to center.
            repaint();
        }
        _lastPos = event->pos();
        return;
    }
    QOpenGLWidget::mouseMoveEvent(event);
}

void Viewer::wheelEvent(QWheelEvent *event)
{
    // Zoom scene.
#if QT_VERSION >= 0x050000
    float degrees = event->angleDelta().y() / 8;
#else
    float degrees = event->delta() / 8;
#endif
    float steps = -degrees / 15;  // Most mouse types work in steps of 15 degrees.
    QVector3D view = _center - _eye;
    float n = view.length();
    view.normalize();
    if(steps > 0)
        n *= (steps * 0.75);
    else if(steps < 0)
        n /= (-steps * 0.75);
    if(n < 1)
        n = 1;
    else if(n > 120)
        n = 120;
    view *= n;
    _eye = _center - view;
    repaint();
}

void Viewer::perspective(double fovy, double aspect, double zNear, double zFar)
{
    double ymax = zNear * tan(fovy * M_PI / 360.0);
    double ymin = -ymax;
    double xmin = ymin * aspect;
    double xmax = ymax * aspect;
    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

void Viewer::lookAt(const QVector3D &eye, const QVector3D &center, const QVector3D &up)
{
    QVector3D forward = (center - eye).normalized();
    QVector3D side = QVector3D::crossProduct(forward, up).normalized();
    QVector3D nup = QVector3D::crossProduct(side, forward).normalized();
    
    float m[4][4];
    m[0][0] = side.x(); m[0][1] = nup.x(); m[0][2] = -forward.x(); m[0][3] = 0;
    m[1][0] = side.y(); m[1][1] = nup.y(); m[1][2] = -forward.y(); m[1][3] = 0;
    m[2][0] = side.z(); m[2][1] = nup.z(); m[2][2] = -forward.z(); m[2][3] = 0;
    m[3][0] = 0;        m[3][1] = 0;       m[3][2] = 0;            m[3][3] = 1;
    
    glMultMatrixf(&m[0][0]);
    glTranslated(-eye.x(), -eye.y(), -eye.z());
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    Viewer viewer;
    viewer.show();
    
    return app.exec();
}
