cmake_minimum_required(VERSION 3.3)

# TODO Windows & Mac

project(Frontieres LANGUAGES C CXX)

enable_language(C)
enable_language(CXX)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

# Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5OpenGL CONFIG REQUIRED)

add_executable(Frontieres WIN32
  sources/Frontieres.cpp
  sources/Ports.cpp
  sources/MyRtAudio.cpp
  sources/MyRtOsc.cpp
  sources/interface/MyGLApplication.cpp
  sources/interface/MyGLWindow.cpp
  sources/interface/StartDialog.cpp
  sources/interface/AdsrDialog.cpp
  sources/interface/CloudDialog.cpp
  sources/interface/OptionsDialog.cpp
  sources/interface/Node.cpp
  sources/interface/CombiDialog.cpp
  sources/interface/BankDialog.cpp
  sources/interface/InstrumDialog.cpp
  sources/dsp/Window.cpp
  sources/model/MidiBank.cpp
  sources/model/MidiCombi.cpp
  sources/model/MidiInstrum.cpp
  sources/model/Adsr.cpp
  sources/model/Sample.cpp
  sources/model/Grain.cpp
  sources/model/Cloud.cpp
  sources/model/Scene.cpp
  sources/model/ParamAdsr.cpp
  sources/visual/GrainVis.cpp
  sources/visual/CloudVis.cpp
  sources/visual/SampleVis.cpp
  sources/utility/GTime.cpp
  libraries/QtFont3D/QtFont3D.cpp
  libraries/Stk.cpp
  libraries/RtAudio.cpp
  libraries/RtMidi.cpp
  libraries/ring_buffer.cpp)
target_include_directories(Frontieres
  PRIVATE "sources" PRIVATE "libraries" PRIVATE "libraries/QtFont3D")

include(FindPkgConfig)

target_compile_definitions(Frontieres
  PRIVATE "__LINUX_ALSASEQ__=" "__UNIX_JACK__="
  PRIVATE "INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\""
  PRIVATE "DATA_ROOT_DIR=\"${CMAKE_INSTALL_FULL_DATAROOTDIR}\"")

set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
pkg_check_modules(JACK jack REQUIRED)
find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)

include(FindPkgConfig)
pkg_check_modules(SNDFILE sndfile REQUIRED)
pkg_check_modules(SOXR soxr REQUIRED)
pkg_check_modules(LIBLO liblo REQUIRED)

target_link_libraries(Frontieres
  PRIVATE ${OPENGL_LIBRARIES} ${JACK_LIBRARIES} ${ALSA_LIBRARIES} ${SNDFILE_LIBRARIES} ${SOXR_LIBRARIES} ${LIBLO_LIBRARIES} "${CMAKE_THREAD_LIBS_INIT}")
target_include_directories(Frontieres
  PRIVATE "${OPENGL_INCLUDE_DIR}" ${JACK_INCLUDE_DIRS} ${ALSA_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS} ${SOXR_INCLUDE_DIRS} ${LIBLO_INCLUDE_DIRS})

qt5_wrap_ui(Frontieres_UI_SOURCES
  sources/interface/CombiDialog.ui
  sources/interface/BankDialog.ui
  sources/interface/InstrumDialog.ui
  sources/interface/CloudDialog.ui
  sources/interface/OptionsDialog.ui
  sources/interface/AdsrDialog.ui
  sources/interface/MyGLWindow.ui
  sources/interface/StartDialog.ui
  sources/interface/AboutDialog.ui)

target_sources(Frontieres PRIVATE ${Frontieres_UI_SOURCES})
target_link_libraries(Frontieres PRIVATE Qt5::Widgets Qt5::OpenGL)

# RtOsc
add_library(RtOsc STATIC
  libraries/rtosc/src/dispatch.c
  libraries/rtosc/src/util.c
  libraries/rtosc/src/rtosc-time.c
  libraries/rtosc/src/pretty-format.c
  libraries/rtosc/src/rtosc.c
  libraries/rtosc/src/arg-val-math.c
  libraries/rtosc/src/arg-val-cmp.c
  libraries/rtosc/src/version.c
  libraries/rtosc/src/cpp/subtree-serialize.cpp
  libraries/rtosc/src/cpp/default-value.cpp
  libraries/rtosc/src/cpp/ports-runtime.cpp
  libraries/rtosc/src/cpp/midimapper.cpp
  libraries/rtosc/src/cpp/automations.cpp
  libraries/rtosc/src/cpp/ports.cpp
  libraries/rtosc/src/cpp/savefile.cpp
  libraries/rtosc/src/cpp/undo-history.cpp
  libraries/rtosc/src/cpp/miditable.cpp
  libraries/rtosc/src/cpp/thread-link.cpp)
target_include_directories(RtOsc PUBLIC
  libraries/rtosc/include)
target_link_libraries(Frontieres PRIVATE RtOsc)

# I18n
find_package(Qt5LinguistTools REQUIRED)
qt5_add_translation(TRANSLATIONS
  "translations/Frontieres_fr_FR.ts")
add_custom_target(Frontieres_translations ALL
  DEPENDS ${TRANSLATIONS})
install(FILES ${TRANSLATIONS}
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/Frontieres/translations")

# Install
install(TARGETS Frontieres DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(FILES
  loops/2_rhodes_high_2.wav
  loops/2_rhodes_refrain_2.wav
  loops/hidden_mechanics_stems_Frontieres_stereo.wav
  loops/pretty_rhodes_delay.wav
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/Frontieres/loops")
install(FILES
  resources/Frontieres.desktop
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
install(FILES
  resources/Frontieres.png
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/pixmaps")
install(FILES
  resources/Frontieres.1
  DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
set(MAN_TRANSLATIONS "fr")
foreach(translation ${MAN_TRANSLATIONS})
  install(FILES
    "resources/Frontieres.${translation}.1"
    DESTINATION "${CMAKE_INSTALL_MANDIR}/fr/man1")
endforeach()
