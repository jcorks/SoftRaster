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
	ar rcs ./lib/libSoftRaster-1.0.a $(OBJS)
	$(CC) -shared -o ./lib/libSoftRaster-1.0.so $(OBJS)

%.o:
	$(CC) $(CFLAGS) -fPIC -I./include/ -c $(patsubst %.o,%.cpp,$@) -o $@
	
clean:
	rm -f $(OBJS)

