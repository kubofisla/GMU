##################################################
# nastaveni
CFLAGS_COMMON=-pipe -Wall -ggdb -std=c++11
CC=gcc
CXX=g++

##################################################
# nastaveni knihoven s OpenGL
ifeq ($(OS), Windows_NT)
	CFLAGS=$(CFLAGS_COMMON) -Wl,--enable-auto-import
	LIBS=-lopengl32 -lglut -lm -lOpenCL #-lSOIL 
	RM=del
else
#       USE_GLEE=1
	CFLAGS=$(CFLAGS_COMMON) -I. -IGL #-DUSE_GLEE
	LIBS=-L/usr/lib -lGL -lglut -lm -lOpenCL #-lSOIL
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

