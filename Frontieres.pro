QT += widgets opengl
CONFIG += c++11

TEMPLATE = app
TARGET = Frontieres
INCLUDEPATH += $$PWD/sources
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
  sources/Frontieres.cpp \
  sources/AudioFileSet.cpp \
  sources/MyRtAudio.cpp \
  sources/Window.cpp \
  sources/GrainVoice.cpp \
  sources/GrainCluster.cpp \
  sources/MyGLApplication.cpp \
  sources/MyGLWindow.cpp \
  sources/Scene.cpp \
  sources/visual/GrainVis.cpp \
  sources/visual/GrainClusterVis.cpp \
  sources/visual/SoundRect.cpp \
  sources/utility/GTime.cpp \
  libraries/Stk.cpp \
  libraries/RtAudio.cpp \
  libraries/RtMidi.cpp \
  libraries/ring_buffer.cpp

HEADERS += \
  sources/MyGLApplication.h \
  sources/theglobals.h \
  sources/I18n.h \
  sources/GrainCluster.h \
  sources/Frontieres.h \
  sources/MyGLWindow.h \
  sources/AudioFileSet.h \
  sources/Window.h \
  sources/MyRtAudio.h \
  sources/GrainVoice.h \
  sources/Scene.h \
  sources/visual/GrainVis.h \
  sources/visual/GrainClusterVis.h \
  sources/visual/SoundRect.h \
  sources/utility/GTime.h \
  libraries/RtMidi.h \
  libraries/RtAudio.h \
  libraries/Stk.h \
  libraries/ring_buffer.h \
  libraries/ring_buffer.tcc \
