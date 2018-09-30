/* --------------------------------------------------------------------------------
 * 3D text using OpenGL and QFont.
 *
 * Author: Marcel Paz Goldschen-Ohm
 * Email: marcel.goldschen@gmail.com
 * -------------------------------------------------------------------------------- */

#ifndef __QtFont3D_H__
#define __QtFont3D_H__

#include <QFont>
#include <QFontMetricsF>
#include <QOpenGLFunctions>
#include <QString>
#include <QVector3D>

#ifdef DEBUG
#include <iostream>
#include <QDebug>
#endif

/* --------------------------------------------------------------------------------
 * 3D text using OpenGL and QFont.
 *
 * !!! WARNING !!! You must have a valid current GL context before calling setFont().
 *                 Otherwise, the display list will not be generated.
 * -------------------------------------------------------------------------------- */
class QtFont3D
{
public:
    QtFont3D() : _font(QFont()), _displayListBase(0), _numGlyphs(0) {}
    ~QtFont3D() { if(_displayListBase) glDeleteLists(_displayListBase, _numGlyphs); }

    QFont font() const { return _font; }
    void setFont(const QFont &font = QFont(), float glyphThickness = 1, int numGlyphs = 256);

    void print(const QString &text, const QVector3D &position = QVector3D(0, 0, 0), int flags = Qt::AlignLeft, float scale = 1) const;

protected:
    QFont _font;
    GLuint _displayListBase;
    int _numGlyphs;

    void _buildGlyph(int glyph, GLuint displayListBase, const QFont &font, QFontMetricsF *fontMetrics, float glyphThickness = 1);
};

#endif
