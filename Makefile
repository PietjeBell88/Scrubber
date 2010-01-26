all: default
# Sources
SRCS  = ./external/getopt_pp.cpp \
        ./src/Particles/Particle.cpp ./src/Particles/ParticleArray.cpp \
        ./src/Channel/CPModel.cpp ./src/Channel/Channel.cpp \
        ./src/Particles/Mover.cpp \
        ./src/Emitter/Emitter.cpp ./src/Emitter/GridEmitter.cpp ./src/Emitter/GridOnceEmitter.cpp ./src/Emitter/RandomEmitter.cpp \
        ./src/InOut/InOut.cpp ./src/InOut/ByteInOut.cpp ./src/InOut/TextInOut.cpp  \
        ./src/Scrubber.cpp

CXXFLAGS = -O2 -DNDEBUG

default:
    g++ $(CXXFLAGS) -I../Include/blitz-0.9 -I. -I./external -I./src $(SRCS) -o scrubber
