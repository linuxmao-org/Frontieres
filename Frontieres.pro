QT += widgets gui opengl
CONFIG += c++11
QMAKE_CFLAGS += -std=c99

TEMPLATE = app
TARGET = Frontieres
INCLUDEPATH += $$PWD/sources
INCLUDEPATH += $$PWD/libraries

DEFINES += APP_VERSION=\\\"0.0\\\"

DEFINES += __LINUX_ALSASEQ__
DEFINES += __LINUX_ALSA__
DEFINES += __LINUX_PULSE__

INCLUDEPATH += $$PWD/libraries/QtFont3D
SOURCES += libraries/QtFont3D/QtFont3D.cpp
HEADERS += libraries/QtFont3D/QtFont3D.h \
    sources/Version.h \
    sources/interface/ScalesDialog.h

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
  LIBS += -ljack -lpulse-simple -lpulse -lasound
}

LIBS += -lsndfile -lsoxr -llo

SOURCES += \
  sources/Frontieres.cpp \
  sources/Ports.cpp \
  sources/MyRtAudio.cpp \
  sources/MyRtOsc.cpp \
  sources/interface/CombiDialog.cpp \
  sources/interface/BankDialog.cpp \
  sources/interface/InstrumDialog.cpp \
  sources/interface/MyGLApplication.cpp \
  sources/interface/MyGLWindow.cpp \
  sources/interface/StartDialog.cpp \
  sources/interface/AdsrDialog.cpp \
  sources/interface/CloudDialog.cpp \
  sources/interface/OptionsDialog.cpp \
  sources/interface/Node.cpp \
  sources/interface/MonitorWidget.cpp \
  sources/interface/TriggerDialog.cpp \
  sources/interface/ControlDialog.cpp \
  sources/interface/PhraseDialog.cpp \
  sources/interface/ScalesDialog.cpp \
  sources/dsp/Window.cpp \
  sources/model/MidiBank.cpp \
  sources/model/MidiCombi.cpp \
  sources/model/MidiInstrum.cpp \
  sources/model/Adsr.cpp \
  sources/model/Sample.cpp \
  sources/model/Grain.cpp \
  sources/model/Cloud.cpp \
  sources/model/Scene.cpp \
  sources/model/ParamAdsr.cpp \
  sources/model/Trigger.cpp \
  sources/model/Phrase.cpp \
  sources/model/Scale.cpp \
  sources/visual/SampleVis.cpp \
  sources/visual/GrainVis.cpp \
  sources/visual/CloudVis.cpp \
  sources/visual/Trajectory.cpp \
  sources/visual/Circular.cpp \
  sources/visual/Hypotrochoid.cpp \
  sources/visual/Recorded.cpp \
  sources/visual/TriggerVis.cpp \
  sources/utility/GTime.cpp \
  libraries/Stk.cpp \
  libraries/RtAudio.cpp \
  libraries/RtMidi.cpp \
  libraries/minimp3_custom.c \
  libraries/ring_buffer.cpp
 


HEADERS += \
  sources/theglobals.h \
  sources/Frontieres.h \
  sources/Ports.h \
  sources/MyRtAudio.h \
  sources/MyRtOsc.h \
  sources/interface/InstrumDialog.h \
  sources/interface/CombiDialog.h \
  sources/interface/BankDialog.h \
  sources/interface/MyGLApplication.h \
  sources/interface/MyGLWindow.h \
  sources/interface/StartDialog.h \
  sources/interface/AdsrDialog.h \
  sources/interface/CloudDialog.h \
  sources/interface/OptionsDialog.h \
  sources/interface/Node.h \
  sources/interface/MonitorWidget.h \
  sources/interface/TriggerDialog.h \
  sources/interface/ControlDialog.h \
  sources/interface/PhraseDialog.h \
  sources/interface/ScalesDialog.h \
  sources/dsp/Window.h \
  sources/model/MidiBank.h \
  sources/model/MidiCombi.h \
  sources/model/MidiInstrum.h \
  sources/model/Adsr.h \
  sources/model/Sample.h \
  sources/model/Cloud.h \
  sources/model/Grain.h \
  sources/model/Scene.h \
  sources/model/ParamAdsr.h \
  sources/model/ParamCloud.h \
  sources/model/Trigger.h \
  sources/model/Phrase.h \
  sources/model/Scale.h \
  sources/visual/SampleVis.h \
  sources/visual/GrainVis.h \
  sources/visual/CloudVis.h \
  sources/visual/Trajectory.h \ 
  sources/visual/Circular.h \
  sources/visual/Hypotrochoid.h \
  sources/visual/Recorded.h \
  sources/visual/TriggerVis.h \
  sources/utility/GTime.h \
  libraries/RtMidi.h \
  libraries/RtAudio.h \
  libraries/Stk.h \
  libraries/minimp3_custom.h \
  libraries/ring_buffer.h \
  libraries/ring_buffer.tcc \
  libraries/minimp3_custom.h

FORMS += \
  sources/interface/MyGLWindow.ui \
  sources/interface/StartDialog.ui \
  sources/interface/AdsrDialog.ui \
  sources/interface/CloudDialog.ui \
  sources/interface/OptionsDialog.ui \
  sources/interface/CombiDialog.ui \
  sources/interface/BankDialog.ui \
  sources/interface/InstrumDialog.ui \
  sources/interface/AboutDialog.ui \
  sources/interface/TriggerDialog.ui \
  sources/interface/ControlDialog.ui \
  sources/interface/PhraseDialog.ui \
  sources/interface/ScalesDialog.ui

TRANSLATIONS += \
  translations/Frontieres_fr_FR.ts

# RtOsc
INCLUDEPATH += $$PWD/libraries/rtosc/include

SOURCES += \
  libraries/rtosc/src/dispatch.c \
  libraries/rtosc/src/util.c \
  libraries/rtosc/src/rtosc-time.c \
  libraries/rtosc/src/pretty-format.c \
  libraries/rtosc/src/rtosc.c \
  libraries/rtosc/src/arg-val-math.c \
  libraries/rtosc/src/arg-val-cmp.c \
  libraries/rtosc/src/version.c \
  libraries/rtosc/src/cpp/subtree-serialize.cpp \
  libraries/rtosc/src/cpp/default-value.cpp \
  libraries/rtosc/src/cpp/ports-runtime.cpp \
  libraries/rtosc/src/cpp/midimapper.cpp \
  libraries/rtosc/src/cpp/automations.cpp \
  libraries/rtosc/src/cpp/ports.cpp \
  libraries/rtosc/src/cpp/savefile.cpp \
  libraries/rtosc/src/cpp/undo-history.cpp \
  libraries/rtosc/src/cpp/miditable.cpp \
  libraries/rtosc/src/cpp/thread-link.cpp

HEADERS += \
  libraries/rtosc/include \
  libraries/rtosc/include/rtosc \
  libraries/rtosc/include/rtosc/rtosc-time.h \
  libraries/rtosc/include/rtosc/typed-message.h \
  libraries/rtosc/include/rtosc/ports.h \
  libraries/rtosc/include/rtosc/subtree-serialize.h \
  libraries/rtosc/include/rtosc/typestring.hh \
  libraries/rtosc/include/rtosc/bundle-foreach.h \
  libraries/rtosc/include/rtosc/default-value.h \
  libraries/rtosc/include/rtosc/port-sugar.h \
  libraries/rtosc/include/rtosc/miditable.h \
  libraries/rtosc/include/rtosc/rtosc.h \
  libraries/rtosc/include/rtosc/savefile.h \
  libraries/rtosc/include/rtosc/automations.h \
  libraries/rtosc/include/rtosc/arg-val-cmp.h \
  libraries/rtosc/include/rtosc/ports-runtime.h \
  libraries/rtosc/include/rtosc/rtosc-version.h \
  libraries/rtosc/include/rtosc/undo-history.h \
  libraries/rtosc/include/rtosc/thread-link.h \
  libraries/rtosc/include/rtosc/arg-val-math.h \
  libraries/rtosc/include/rtosc/pretty-format.h

RESOURCES += \
  docs.qrc \
    resources/buttons.qrc
