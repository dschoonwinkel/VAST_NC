
my_demo = ../../bin/my_demo

.PHONY: all noace

# IMPORTANT NOTE: the least dependent library must begin from the rightmost (often vastcommon) 
LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon -lboost_system -lboost_thread
#-lz -ldl -lpthread
LIBS_ACE    = -lACE 
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl

ACE_PATH    = /usr

# CFLAGS      = -Wall -static 
CFLAGS      = -Wall -fPIC

INC_PATHS   = -I../../common -I../../VASTsim -I$(ACE_PATH)/include 

LIB_PATHS   = -L../../lib -L../../Dependencies/lib/Release -L$(ACE_PATH)/lib 

LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL)

all: my_demo.cpp my_demo_helper.cpp
	g++ $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $< $(LIBS) \
	-o $(my_demo) my_demo_helper.cpp

noace:
	make TARGET=noace

clean:
	rm -f $(my_demo)
