VASTreal_console = ../../bin/VASTreal_console

.PHONY: all noace

# IMPORTANT NOTE: the least dependent library must begin from the rightmost (often vastcommon) 
LIBS_COMMON = -lvastreal -lvast -lvastnet -lvastcommon -lvaststatlog
#-lz -ldl -lpthread
LIBS_ACE    = -lACE 
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization -lboost_filesystem

ACE_PATH    = /usr

# CFLAGS      = -Wall -static 
CFLAGS      = -Wall -Wextra -fPIC -ggdb -g3 -std=c++14 -fsanitize=address -fsanitize=thread

LIBS_INCKODO = -I../../kodo-rlnc/resolve_symlinks/boost \
			-I../../kodo-rlnc/resolve_symlinks/kodo-core/src \
			-I../../kodo-rlnc/resolve_symlinks/endian/src \
			-I../../kodo-rlnc/kodo_build/include

LIBS_KODO = -Wl,-Bstatic -L/home/daniel/Development/kodo-rlnc/kodo_build -lkodo_rlnc -lfifi -lcpuid -Wl,-Bdynamic


INC_PATHS   = -I../../common -I../../VASTreal -I$(ACE_PATH)/include -I../../VAST -I../../VASTStatLog

LIB_PATHS   = -L../../lib -L../../Dependencies/lib/Release -L$(ACE_PATH)/lib

LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL) $(LIBS_BOOST) $(LIBS_INCKODO) $(LIBS_KODO)

all: VASTreal_console.cpp
	g++ $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $< $(LIBS) -o $(VASTreal_console)

clean:
	rm -f $(VASTreal_console)
