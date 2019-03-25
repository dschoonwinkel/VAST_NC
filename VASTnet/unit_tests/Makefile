unittest_net_udpnc_handler = ./unittest_net_udpnc_handler
unittest_rlncmessage = ./unittest_rlncmessage
unittest_net_udpnc_mchandler = ./unittest_net_udpnc_mchandler

CC = g++

ACE_ROOT = /usr


#CFLAGS=-O2 -ftree-vectorize -std=c++14 -Wall -Wextra
CFLAGS=-ggdb -std=c++14 -Wall -Wextra

LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon
#-lz -ldl -lpthread
LIBS_ACE    = -lACE
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization -lboost_iostreams -lboost_thread -lboost_chrono -lpthread
LIBS_INCKODO = -I../../../kodo-rlnc/resolve_symlinks/kodo-core/src \
			-I../../../kodo-rlnc/resolve_symlinks/endian/src \
			-I../../../kodo-rlnc/kodo_build/include

LIBS_KODO = -Wl,-Bstatic -L/home/daniel/Development/kodo-rlnc/kodo_build -lkodo_rlnc -lfifi -lcpuid -Wl,-Bdynamic
LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL) $(LIBS_BOOST)

ACE_PATH    = /usr
INC_PATHS   = -I../../common -I../../VASTsim -I$(ACE_PATH)/include -I../../VAST -I../../VASTnet -I../../coding_host
LIB_PATHS   = -L../../lib -L../../Dependencies/lib/Release -L$(ACE_PATH)/lib

.PHONY: unittest_net_udpnc_handler unittest_rlncmessage unittest_net_udpnc_mchandler

all: unittest_net_udpnc_handler unittest_rlncmessage unittest_net_udpnc_mchandler

unittest_net_udpnc_handler: unittest_net_udpnc_handler.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $< $(LIBS) $(LIBS_KODO) \
	-o $(unittest_net_udpnc_handler)
	echo "Running: unittest_net_udpnc_handler"
	$(unittest_net_udpnc_handler)

unittest_rlncmessage: unittest_rlncmessage.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $< $(LIBS) $(LIBS_KODO) \
	-o $(unittest_rlncmessage)
	echo "Running: unittest_rlncmessage"
	$(unittest_rlncmessage)

unittest_net_udpnc_mchandler: unittest_net_udpnc_mchandler.cpp abstract_rlnc_msg_receiver_testimpl.cpp ../../coding_host/rlncrecoder.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_net_udpnc_mchandler)
	echo "Running: unittest_net_udpnc_mchandler"
	$(unittest_net_udpnc_mchandler)


clean:
	-rm $(unittest_net_udpnc_handler) $(unittest_rlncmessage) $(unittest_net_udpnc_mchandler)

noace:
