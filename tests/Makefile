CC=g++

vaststatlog_test = ./vaststatlog_test
udp_buffertest = ./udp_buffertest

ACE_ROOT = /usr


#CFLAGS=-O2 -ftree-vectorize -std=c++14 -Wall -Wextra
CFLAGS=-ggdb -std=c++14 -Wall -Wextra $(COPTS) 

LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon -lvaststatlog
#-lz -ldl -lpthread
LIBS_ACE    = -lACE
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization -lboost_filesystem -lboost_iostreams -lboost_thread -lboost_chrono -lpthread
LIBS_INCKODO = -I../../kodo-rlnc/resolve_symlinks/kodo-core/src \
			-I../../kodo-rlnc/resolve_symlinks/endian/src \
			-I../../kodo-rlnc/kodo_build/include

LIBS_KODO = -Wl,-Bstatic -L$(HOME)/Development/kodo-rlnc/kodo_build -lkodo_rlnc -lfifi -lcpuid -Wl,-Bdynamic
LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL) $(LIBS_BOOST)

ACE_PATH    = /usr
INC_PATHS   = -I../common -I../VASTsim -I$(ACE_PATH)/include -I../VAST -I../VASTnet -I../coding_host -I../VASTStatLog
LIB_PATHS   = -L../lib -L../Dependencies/lib/Release -L$(ACE_PATH)/lib

.PHONY: vaststatlog_test udp_buffertest

all: vaststatlog_test udp_buffertest

vaststatlog_test: vaststatlog_test.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $< $(LIBS) $(LIBS_KODO) \
	-o $(vaststatlog_test)

udp_buffertest: udp_buffertest.cpp
	$(CC) $(CFLAGS) $^ $(LIBS_BOOST) \
	-o $(udp_buffertest)


clean:
	-rm $(vaststatlog_test) $(udp_buffertest)

noace:
