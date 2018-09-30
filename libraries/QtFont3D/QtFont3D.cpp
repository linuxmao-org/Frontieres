/* --------------------------------------------------------------------------------
 * Author: Marcel Paz Goldschen-Ohm
 * Email: marcel.goldschen@gmail.com
 * -------------------------------------------------------------------------------- */

#include "QtFont3D.h"

#include <stdexcept>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <QList>
#include <QPainter>
#include <QRectF>

void QtFont3D::setFont(const QFont &font, float glyphThickness, int numGlyphs)
{
    if(_displayListBase)
        glDeleteLists(_displayListBase, _numGlyphs);
    _displayListBase = 0;
    _numGlyphs = 0;
    if(numGlyphs == 0)
        return;
    _displayListBase = glGenLists(numGlyphs);
    if(!_displayListBase)
        throw std::runtime_error("Text3D::Text3D: Cannot create display lists.");
    _numGlyphs = numGlyphs;
    _font = font;
    QFontMetricsF *fontMetrics = new QFontMetricsF(font);
    for(int i = 0; i < _numGlyphs; ++i)
        _buildGlyph(i, _displayListBase, font, fontMetrics, glyphThickness);
}

void QtFont3D::print(const QString &text, const QVector3D &position, int flags, float scale) const
{
    glPushMatrix();
    glTranslatef(position.x(), position.y(), position.z());
    QFontMetricsF fm(_font);
    QRectF bbox = fm.tightBoundingRect(text);
    float x = bbox.bottomLeft().x() * scale;
    float y = bbox.bottomLeft().y() * scale;
    float w = bbox.width() * scale;
    float h = bbox.height() * scale;
    float x0 = x;
    float y0 = y;
    if(flags & Qt::AlignCenter) {
        x0 = x - w / 2;
        y0 = y - h / 2;
    }
    if(flags & Qt::AlignHCenter)
        x0 = x - w / 2;
    if(flags & Qt::AlignVCenter)
        y0 = y - h / 2;
    if(flags & Qt::AlignLeft)
        x0 = x;
    if(flags & Qt::AlignRight)
        x0 = x - w;
    if(flags & Qt::AlignTop)
        y0 = y - h;
    if(flags & Qt::AlignBottom)
        y0 = y;
    if(x0 || y0)
        glTranslatef(x0, y0, 0);
    glScalef(scale, scale, scale);
    glPushAttrib(GL_LIST_BIT);
    glListBase(_displayListBase);
    glCallLists(text.length(), GL_UNSIGNED_BYTE, text.toLocal8Bit());
    glPopAttrib(); // GL_LIST_BIT
    glPopMatrix();
}

void QtFont3D::_buildGlyph(int glyph, GLuint displayListBase, const QFont &font, QFontMetricsF *fontMetrics, float glyphThickness)
{
    QPainterPath path;
    path.addText(QPointF(0, 0), font, QString(static_cast<char>(glyph)));
    QList<QPolygonF> polygons = path.toSubpathPolygons();
    // Set up the tesselation.
    GLUtriangulatorObj *tesselator = gluNewTess();
    gluTessCallback(tesselator, GLU_TESS_BEGIN, (void (*)())(glBegin));
    gluTessCallback(tesselator, GLU_TESS_VERTEX, (void (*)())(glVertex3dv));
    gluTessCallback(tesselator, GLU_TESS_END, (void (*)())(glEnd));
    gluTessProperty(tesselator, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
    int numVertices = 0;
    foreach(const QPolygonF &polygon, polygons)
        numVertices += polygon.size();
    GLdouble *vertices = static_cast<GLdouble*>(malloc(numVertices * 3 * sizeof(GLdouble)));
    // Display list.
    glNewList(displayListBase + glyph, GL_COMPILE);
    glShadeModel(GL_FLAT);
    // Tesselate front face.
    gluTessBeginPolygon(tesselator, 0);
    int i = 0;
    foreach(QPolygonF polygon, polygons) {
        gluTessBeginContour(tesselator);
        foreach(QPointF pt, polygon) {
            vertices[i + 0] = pt.rx();
            vertices[i + 1] = -pt.ry();
            vertices[i + 2] = 0;
            gluTessVertex(tesselator, &vertices[i], &vertices[i]);
            i += 3;
        }
        gluTessEndContour(tesselator);
    }
    gluTessEndPolygon(tesselator);
    // Tesselate back face.
    gluTessBeginPolygon(tesselator, 0);
    i = 0;
    foreach(QPolygonF polygon, polygons) {
        gluTessBeginContour(tesselator);
        foreach(QPointF pt, polygon) {
            vertices[i + 0] = pt.rx();
            vertices[i + 1] = -pt.ry();
            vertices[i + 2] = -glyphThickness;
            gluTessVertex(tesselator, &vertices[i], &vertices[i]);
            i += 3;
        }
        gluTessEndContour(tesselator);
    }
    gluTessEndPolygon(tesselator);
    // Clear malloced vertices.
    free(vertices);
    // Side between front and back faces.
    foreach(QPolygonF polygon, polygons) {
        glBegin(GL_QUAD_STRIP);
        foreach(QPointF pt, polygon) {
            glVertex3f(pt.rx(), -pt.ry(), 0);
            glVertex3f(pt.rx(), -pt.ry(), -glyphThickness);
        }
        QPointF pt = *polygon.begin(); // first point to close the loop
        glVertex3f(pt.rx(), -pt.ry(), 0);
        glVertex3f(pt.rx(), -pt.ry(), -glyphThickness);
        glEnd();
    }
    // Add translation to advance to next charachter to display list.
    GLfloat glyphWidth = fontMetrics->width(glyph);
    glTranslatef(glyphWidth, 0, 0);
    // End display list.
    glEndList();
    // Clear tesselator.
    gluDeleteTess(tesselator);
}
