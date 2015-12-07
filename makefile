# makefile for g++: SoftRaster

CC := g++

CFLAGS := -O2 -std=c++11 


SRCS :=./src/Pipeline.cpp \
       ./src/Texture.cpp \
       ./src/StageProcedure.cpp \
       ./src/CoreProcedures.cpp



OBJS := $(patsubst %.cpp,%.o, $(SRCS))

all: $(OBJS) 
	ar rcs ./lib/SoftRaster.a $(OBJS)

%.o:
	$(CC) $(CFLAGS) -I./include/ -c $(patsubst %.o,%.cpp,$@)
	
clean:
	rm -f $(OBJS)

