# makefile for g++: SoftRaster

CC := g++

CFLAGS := -g -std=c++11 


SRCS := ../base/basics.cpp ../base/TransformMatrix.cpp main.cpp



OBJS := $(patsubst %.cpp,%.o, $(SRCS))

all: $(OBJS) 
	$(CC) $(OBJS) -DSOFTRASTER_RT_CHECKS -L../../lib/ -o console -lSoftRaster-1.0 -lncurses

%.o:
	$(CC) $(CFLAGS) -I../../include -c $(patsubst %.o,%.cpp,$@) -o $@
	
clean:
	rm -f $(OBJS)

