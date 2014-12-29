##################################################
# nastaveni
CFLAGS_COMMON=-pipe -Wall -ggdb -std=c++11
CC=gcc
CXX=g++

##################################################
# nastaveni knihoven s OpenGL
ifeq ($(OS), Windows_NT)
	CFLAGS=$(CFLAGS_COMMON) -Wl,--enable-auto-import
	LIBS=-lopengl32 -lglew32 -lSDL -lglut -lGLU -lm -lSOIL -lOpenCL
	RM=del
else
#       USE_GLEE=1
	CFLAGS=$(CFLAGS_COMMON) `sdl-config --cflags` -I. -IGL #-DUSE_GLEE
	LIBS=-L/usr/lib `sdl-config --libs` -lGL -lGLEW -lglut -lGLU -lm -lSOIL -lOpenCL
	RM=rm -f
endif

CXXFLAGS=$(CFLAGS)

##################################################
# obecne cile

PROJ=terrainGen

DEPS=main.o ClFunctions.o

all: terrainGen

terrainGen: main.o ClFunctions.o
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS)

main.o:  main.cpp 
	$(CXX) -c $^ $(CFLAGS) $(LIBS)
	
ClFunctions.o:  ClFunctions.cpp 
	$(CXX) -c $^ $(CFLAGS) $(LIBS)

clean:
	$(RM) $(PROJ) $(DEPS)

