##################################################
# nastaveni
CFLAGS_COMMON=-pipe -Wall -ggdb -std=c++11
CC=gcc
CXX=g++
#zapnutie texturovania pomocou SOILu <>WIN
#TEX=1

##################################################
# nastaveni knihoven s OpenGL
ifeq ($(OS), Windows_NT)
	CFLAGS=$(CFLAGS_COMMON) -Wl,--enable-auto-import
	LIBS=-lopengl32 -lglut -lm -lOpenCL -lGLU #-lSOIL 
	RM=del
else
	ifeq ($(TEX), 1)
		USE_SOIL=1
		CFLAGS=$(CFLAGS_COMMON) -I. -IGL -DUSE_SOIL
		LIBS=-L/usr/lib -lGL -lglut -lm -lOpenCL -lGLU -lSOIL
	else
		CFLAGS=$(CFLAGS_COMMON) -I. -IGL
		LIBS=-L/usr/lib -lGL -lglut -lm -lOpenCL -lGLU
	endif
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

