
CXX=g++
INCLUDES=

UNAME := $(shell uname)


ifeq ($(UNAME), Linux)
FLAGS=-D__LINUX_ALSASEQ__ -D__UNIX_JACK__  -DOSC_HOST_LITTLE_ENDIAN -c
LIBS=-lasound -lpthread -ljack -lstdc++ -lglut -lGL -lGLU -lm -lsndfile
endif
ifeq ($(UNAME), Darwin)
FLAGS=-D__MACOSX_CORE__ -c
LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon  -framework OpenGL \
	-framework GLUT -framework Foundation \
	-framework AppKit -lstdc++ -lm -lsndfile
endif

# this is needed by some oscpack sources
INC_PATH = -I./


# executable
TARGET = Borderlands


# build binaries from sources in all these directories
VPATH = osc ip ip/posix


# the binary objects to compile 
# (only the first one should contain a main() )
OBJ_FILES = \
	Borderlands.o \
    SoundRect.o \
    GTime.o\
    AudioFileSet.o \
	MyRtAudio.o \
    Window.o \
    GrainVoice.o \
    GrainCluster.o \
	Stk.o \
	Thread.o \
    RtAudio.o \
	RtMidi.o \


# Build executable
${TARGET} : ${OBJ_FILES}
	${CXX} -o ${TARGET} ${OBJ_FILES} ${LIBS}

# Build C objects (uses substitution)
%.o: %.c
	${CXX} ${FLAGS} ${INC_PATH} -c $< -o $@

# Build C++objects (uses substitution)
%.o: %.cpp
	${CXX} ${FLAGS} ${INC_PATH} -c $< -o $@


# Clean up build
clean :
	rm -f ${OBJ_FILES}
	rm -f ${TARGET}
	rm -f *~
