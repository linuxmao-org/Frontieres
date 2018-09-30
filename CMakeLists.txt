cmake_minimum_required(VERSION 3.3)

# TODO Windows & Mac

project(Borderlands LANGUAGES C CXX)

enable_language(C)
enable_language(CXX)
include(GNUInstallDirs)

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_executable(Borderlands
  Borderlands.cpp
  SoundRect.cpp
  GTime.cpp
  AudioFileSet.cpp
  MyRtAudio.cpp
  Window.cpp
  GrainVoice.cpp
  GrainCluster.cpp
  Thread.cpp
  libraries/Stk.cpp
  libraries/RtAudio.cpp
  libraries/RtMidi.cpp)
target_include_directories(Borderlands
  PRIVATE "." PRIVATE "libraries")

include(FindPkgConfig)

target_compile_definitions(Borderlands
  PRIVATE "__LINUX_ALSASEQ__=" "__UNIX_JACK__="
  PRIVATE "INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\""
  PRIVATE "DATA_ROOT_DIR=\"${CMAKE_INSTALL_FULL_DATAROOTDIR}\"")

set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
pkg_check_modules(JACK jack REQUIRED)
find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)
find_library(SNDFILE_LIBRARY sndfile)
message(STATUS "Sndfile: ${SNDFILE_LIBRARY}")
find_library(SOXR_LIBRARY soxr)
message(STATUS "Soxr: ${SOXR_LIBRARY}")

target_link_libraries(Borderlands
  PRIVATE ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${JACK_LIBRARIES} ${ALSA_LIBRARIES} "${SNDFILE_LIBRARY}" "${SOXR_LIBRARY}" "${CMAKE_THREAD_LIBS_INIT}")
target_include_directories(Borderlands
  PRIVATE ${GLUT_INCLUDE_DIRS} "${OPENGL_INCLUDE_DIR}" ${JACK_INCLUDE_DIRS} ${ALSA_INCLUDE_DIRS})

# I18n
set(I18N_TRANSLATIONS "fr")
find_package(Intl REQUIRED)
find_package(Gettext REQUIRED)
target_link_libraries(Borderlands PRIVATE ${Intl_LIBRARIES})
foreach(translation ${I18N_TRANSLATIONS})
  gettext_process_po_files("${translation}" ALL
    INSTALL_DESTINATION "${CMAKE_INSTALL_LOCALEDIR}"
    PO_FILES
    "po/${translation}/Borderlands.po")
endforeach()


# Install
install(TARGETS Borderlands DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(FILES
  loops/2_rhodes_high_2.wav
  loops/2_rhodes_refrain_2.wav
  loops/hidden_mechanics_stems_borderlands_stereo.wav
  loops/pretty_rhodes_delay.wav
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/Borderlands/loops")
install(FILES
  resources/borderlands.desktop
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
install(FILES
  resources/borderlands.png
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/pixmaps")
install(FILES
  resources/Borderlands.1
  DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
set(MAN_TRANSLATIONS "fr")
foreach(translation ${MAN_TRANSLATIONS})
  install(FILES
    "resources/Borderlands.${translation}.1"
    DESTINATION "${CMAKE_INSTALL_MANDIR}/fr/man1")
endforeach()
