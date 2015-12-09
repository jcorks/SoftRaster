# makefile for g++: SoftRaster

CC := g++

CFLAGS := -g -std=c++11 


SRCS :=./src/Pipeline.cpp \
       ./src/Texture.cpp \
       ./src/StageProcedure.cpp \
       ./src/CoreProcedures.cpp \
       ./src/Context.cpp



OBJS := $(patsubst %.cpp,%.o, $(SRCS))

all: $(OBJS) 
	ar rcs ./lib/libSoftRaster.a $(OBJS)

%.o:
	$(CC) $(CFLAGS) -I./include/ -c $(patsubst %.o,%.cpp,$@) -o $@
	
clean:
	rm -f $(OBJS)

