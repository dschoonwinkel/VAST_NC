vaststat_replay = ../../bin/vaststat_replay

.PHONY: all noace

# IMPORTANT NOTE: the least dependent library must begin from the rightmost (often vastcommon) 
LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon
#-lz -ldl -lpthread
LIBS_ACE    = -lACE 
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization -lboost_filesystem
ACE_PATH    = /usr

LIBS_INCKODO = -I../../kodo-rlnc/resolve_symlinks/boost \
			-I../../kodo-rlnc/resolve_symlinks/kodo-core/src \
			-I../../kodo-rlnc/resolve_symlinks/endian/src \
			-I../../kodo-rlnc/kodo_build/include

LIBS_KODO = -Wl,-Bstatic -L/home/daniel/Development/kodo-rlnc/kodo_build -lkodo_rlnc -lfifi -lcpuid -Wl,-Bdynamic

# CFLAGS      = -Wall -static 
CFLAGS      = -Wall -fPIC -ggdb -std=c++14

INC_PATHS   = -I../../common -I../../VASTsim -I$(ACE_PATH)/include -I../../VAST -I../VASTStatReplayQt

LIB_PATHS   = -L../../lib -L../../Dependencies/lib/Release -L$(ACE_PATH)/lib

LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL) $(LIBS_BOOST) $(LIBS_INCKODO) $(LIBS_KODO)

all: vaststat_replay.cpp ../VASTStatReplayQt/replay_functions.cpp
	g++ $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $^ $(LIBS) \
	-o $(vaststat_replay)

clean:
	-rm $(vaststat_replay)
