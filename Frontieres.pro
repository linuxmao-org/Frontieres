QT += widgets opengl
CONFIG += c++11

TEMPLATE = app
TARGET = Frontieres
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/libraries

DEFINES += __LINUX_ALSASEQ__
DEFINES += __UNIX_JACK__

INCLUDEPATH += $$PWD/libraries/QtFont3D
SOURCES += libraries/QtFont3D/QtFont3D.cpp
HEADERS += libraries/QtFont3D/QtFont3D.h

unix: {
  isEmpty(INSTALL_PREFIX) {
    INSTALL_PREFIX = /usr/local
  }
  DEFINES += DATA_ROOT_DIR=\\\"$$INSTALL_PREFIX/share\\\"
}

macx: {
  LIBS += -framework OpenGL
}
else {
  win32: {
    LIBS += -lopengl32
  }
  else {
    LIBS += -lGLU -lGL
  }
}

linux: {
  LIBS += -ljack
}

LIBS += -lsndfile -lsoxr

SOURCES += \
  Frontieres.cpp \
  SoundRect.cpp \
  GTime.cpp \
  AudioFileSet.cpp \
  MyRtAudio.cpp \
  Window.cpp \
  GrainVoice.cpp \
  GrainCluster.cpp \
  Thread.cpp \
  MyGLApplication.cpp \
  MyGLWindow.cpp \
  libraries/Stk.cpp \
  libraries/RtAudio.cpp \
  libraries/RtMidi.cpp \
  libraries/ring_buffer.cpp \
  Scene.cpp

HEADERS += \
  MyGLApplication.h \
  theglobals.h \
  libraries/RtMidi.h \
  libraries/RtAudio.h \
  libraries/Stk.h \
  libraries/ring_buffer.h \
  libraries/ring_buffer.tcc \
  I18n.h \
  SoundRect.h \
  GrainCluster.h \
  Frontieres.h \
  MyGLWindow.h \
  gpl-3.0-standalone.html \
  GTime.h \
  AudioFileSet.h \
  Window.h \
  MyRtAudio.h \
  GrainVoice.h \
  Thread.h \
  Scene.h
