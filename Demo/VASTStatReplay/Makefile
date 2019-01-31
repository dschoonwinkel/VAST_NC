vaststat_replay = ../../bin/vaststat_replay

.PHONY: all noace

# IMPORTANT NOTE: the least dependent library must begin from the rightmost (often vastcommon) 
LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon
#-lz -ldl -lpthread
LIBS_ACE    = -lACE 
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization -lboost_filesystem -ljsoncpp
ACE_PATH    = /usr

# CFLAGS      = -Wall -static 
CFLAGS      = -Wall -fPIC -ggdb -std=c++14

INC_PATHS   = -I../../common -I../../VASTsim -I$(ACE_PATH)/include -I../../VAST -I../VASTStatReplayQt

LIB_PATHS   = -L../../lib -L../../Dependencies/lib/Release -L$(ACE_PATH)/lib

LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL) $(LIBS_BOOST)

all: vaststat_replay.cpp ../VASTStatReplayQt/replay_functions.cpp
	g++ $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $? $(LIBS) \
	-o $(vaststat_replay)

clean:
	-rm $(vaststat_replay)