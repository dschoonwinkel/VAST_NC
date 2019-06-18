coding_host = ../bin/coding_host
mc_listener_main = ../bin/mc_listener_main

CC = g++


#CFLAGS=-O2 -ftree-vectorize -std=c++14 -Wall -Wextra -Wno-expansion-to-defined
CFLAGS=-ggdb -std=c++14 -Wall -Wextra -fsanitize=address

LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon
#-lz -ldl -lpthread
LIBS_ACE    = -lACE
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization -lboost_filesystem -lboost_iostreams -lboost_thread -lboost_chrono -lpthread
LIBS_INCKODO = -I../../kodo-rlnc/resolve_symlinks/kodo-core/src \
			-I../../kodo-rlnc/resolve_symlinks/endian/src \
			-I../../kodo-rlnc/kodo_build/include

LIBS_KODO = -Wl,-Bstatic -L/home/daniel/Development/kodo-rlnc/kodo_build -lkodo_rlnc -lfifi -lcpuid -Wl,-Bdynamic
LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL) $(LIBS_BOOST)

ACE_PATH    = /usr
INC_PATHS   = -I../common -I../VASTsim -I$(ACE_PATH)/include -I../VAST -I../VASTnet
LIB_PATHS   = -L../lib -L../Dependencies/lib/Release -L$(ACE_PATH)/lib

all: coding_host

coding_host: main.cpp packet_listener.cpp rlncrecoder.cpp mc_packet_listener.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(coding_host)

mc_listener_main: mc_listener_main.cpp packet_listener.cpp rlncrecoder.cpp mc_packet_listener.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(mc_listener_main)

clean:
	-rm $(coding_host) $(mc_listener_main)


