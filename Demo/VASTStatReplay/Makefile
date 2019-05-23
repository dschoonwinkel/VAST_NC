vaststat_replay = ../../bin/vaststat_replay
read_vaststatlog = ../../bin/read_vaststatlog

.PHONY: all noace

# IMPORTANT NOTE: the least dependent library must begin from the rightmost (often vastcommon) 
LIBS_COMMON = -lvastcommon -lvaststatlog
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization -lboost_filesystem

# CFLAGS      = -Wall -static 
CFLAGS      = -Wall -fPIC -ggdb -std=c++14

INC_PATHS   = -I../../common -I../../VASTStatLog -I../VASTStatReplayQt

LIB_PATHS   = -L../../lib

LIBS = $(LIBS_COMMON) $(LIBS_BOOST)

.PHONY: vaststat_replay read_vaststatlog

all: vaststat_replay

vaststat_replay: vaststat_replay.cpp ../VASTStatReplayQt/replay_functions.cpp
	g++ $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $^ $(LIBS) \
	-o $(vaststat_replay)

read_vaststatlog: read_vaststatlog.cpp ../VASTStatReplayQt/replay_functions.cpp
	g++ $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $^ $(LIBS) \
	-o $(read_vaststatlog)


clean:
	-rm $(vaststat_replay) $(read_vaststatlog)
