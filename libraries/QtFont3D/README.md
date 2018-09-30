# QtFont3D

3D font renderer based on QFont, OpenGL and GLU.

**Author**: Marcel Goldschen-Ohm  
**Email**:  <marcel.goldschen@gmail.com>  

## Disclaimer

This code is based largely on stuff I found online which I merely tweeked, so most of the credit does not belong with me. If you are or know the original author, let me know and I'll be happy to give you/them kudos here.

## INSTALL

Everything is in:

* `QtFont3D.h`
* `QtFont3D.cpp`

### Requires:

* [Qt](http://www.qt.io)
* OpenGL
    * GLU

## Usage

```cpp
#include "QtFont3D.h"
QtFont3D textRenderer;
```

:warning: Make sure you have a valid OpenGL context before building the font display list using `setFont(...)`.

```cpp
float glyphWidth = 3;
int numGlyphs = 256;
QFont font = QFont("Sans", 20, QFont::Normal);
textRenderer.setFont(font, glyphWidth, numGlyphs);
```

Render 3D text in an OpenGL scene using `print(...)`. Text color is the current OpenGL color.

```cpp
textRenderer.print("Hello World!");
```

Optional position, text alignment and scaling.

```cpp
QString text = "Hello World!";
// Offset text in y.
QVector3D position(0, 1, 0);
// Center text horizontally and vertically about position.
int align = Qt::AlignHCenter | Qt::AlignVCenter;
// Scale text x2.
float scale = 2;
textRenderer.print(text, position, align, scale);
```

## Example

See `test.h/cpp` for an example where 3D text is rendered within a QOpenGLWidget.

## TODO

* Remove dependence on GLU.
