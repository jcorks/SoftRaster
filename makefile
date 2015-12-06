# makefile for g++: SoftRaster

CC := g++

CFLAGS := -std=c++11 -g


SRCS :=./src/Pipeline.cpp \
       ./src/Texture.cpp \
       ./src/StageProcedure.cpp \
       ./src/CoreProcedures.cpp



OBJS := $(patsubst %.cpp,%.o, $(SRCS))

all: $(OBJS) 
	ar cs ./lib/SoftRaster.o $(OBJS)

%.o:
	$(CC) $(CFLAGS) -I./include/ -c $(patsubst %.o,%.cpp,$@)
	
clean:
	rm -f $(OBJS)

