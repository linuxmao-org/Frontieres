cmake_minimum_required(VERSION 3.3)

# TODO Windows & Mac

project(Frontieres LANGUAGES C CXX)

enable_language(C)
enable_language(CXX)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_executable(Frontieres WIN32
  Frontieres.cpp
  SoundRect.cpp
  GTime.cpp
  AudioFileSet.cpp
  MyRtAudio.cpp
  Window.cpp
  GrainVoice.cpp
  GrainCluster.cpp
  Thread.cpp
  MyGLApplication.cpp
  MyGLWindow.cpp
  libraries/QtFont3D/QtFont3D.cpp
  libraries/Stk.cpp
  libraries/RtAudio.cpp
  libraries/RtMidi.cpp
  libraries/ring_buffer.cpp)
target_include_directories(Frontieres
  PRIVATE "." PRIVATE "libraries" PRIVATE "libraries/QtFont3D")

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

target_link_libraries(Frontieres
  PRIVATE ${OPENGL_LIBRARIES} ${JACK_LIBRARIES} ${ALSA_LIBRARIES} ${SNDFILE_LIBRARIES} ${SOXR_LIBRARIES} "${CMAKE_THREAD_LIBS_INIT}")
target_include_directories(Frontieres
  PRIVATE "${OPENGL_INCLUDE_DIR}" ${JACK_INCLUDE_DIRS} ${ALSA_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS} ${SOXR_INCLUDE_DIRS})

# Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5OpenGL CONFIG REQUIRED)
target_link_libraries(Frontieres PRIVATE Qt5::Widgets Qt5::OpenGL)

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
