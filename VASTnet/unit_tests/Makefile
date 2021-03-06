CC=g++

unittest_net_udp_handler = ./unittest_net_udp_handler
unittest_net_udpnc_handler = ./unittest_net_udpnc_handler
unittest_udpncmessage = ./unittest_udpncmessage
unittest_net_udpnc_mchandler = ./unittest_net_udpnc_mchandler
unittest_udpnc_decoder = ./unittest_udpnc_decoder
unittest_net_udpnc_consumer = ./unittest_net_udpnc_consumer
unittest_vastnet = ./unittest_vastnet

ACE_ROOT = /usr


#CFLAGS=-O2 -ftree-vectorize -std=c++14 -Wall -Wextra
CFLAGS=-ggdb -std=c++14 -Wall -Wextra $(COPTS) 

LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon -lvastreal -lvaststatlog
#-lz -ldl -lpthread
LIBS_ACE    = -lACE
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization -lboost_iostreams -lboost_thread -lboost_chrono -lpthread -lboost_filesystem
LIBS_INCKODO = -I../../../kodo-rlnc/resolve_symlinks/kodo-core/src \
			-I../../../kodo-rlnc/resolve_symlinks/endian/src \
			-I../../../kodo-rlnc/kodo_build/include

LIBS_KODO = -Wl,-Bstatic -L$(HOME)/Development/kodo-rlnc/kodo_build -lkodo_rlnc -lfifi -lcpuid -Wl,-Bdynamic
LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL) $(LIBS_BOOST)

ACE_PATH    = /usr
INC_PATHS   = -I../../common -I../../VASTsim -I$(ACE_PATH)/include -I../../VAST -I../../VASTnet -I../../coding_host -I../../VASTStatLog
LIB_PATHS   = -L../../lib -L../../Dependencies/lib/Release -L$(ACE_PATH)/lib

.PHONY: unittest_net_udp_handler unittest_net_udpnc_handler unittest_udpncmessage unittest_net_udpnc_mchandler make_vastnet unittest_udpnc_decoder unittest_net_udpnc_consumer unittest_vastnet

all: unittest_udpncmessage unittest_net_udp_handler unittest_net_udpnc_mchandler unittest_udpnc_decoder unittest_net_udpnc_handler unittest_net_udpnc_consumer unittest_vastnet

unittest_net_udp_handler: unittest_net_udp_handler.cpp
	$(MAKE) make_vastnet
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_net_udp_handler)
	echo "Running: unittest_net_udp_handler"
	$(unittest_net_udp_handler)


unittest_net_udpnc_handler: unittest_net_udpnc_handler.cpp
	$(MAKE) make_vastnet
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_net_udpnc_handler)
	echo "Running: unittest_net_udpnc_handler"
	$(unittest_net_udpnc_handler)

unittest_udpncmessage: unittest_udpncmessage.cpp
	$(MAKE) make_vastnet
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_udpncmessage)
	echo "Running: unittest_udpncmessage"
	$(unittest_udpncmessage)

unittest_net_udpnc_mchandler: unittest_net_udpnc_mchandler.cpp abstract_udpnc_msg_receiver_testimpl.cpp ../../coding_host/udpncrecoder.cpp
	$(MAKE) make_vastnet
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_net_udpnc_mchandler)
	echo "Running: unittest_net_udpnc_mchandler"
	$(unittest_net_udpnc_mchandler)

unittest_udpnc_decoder: unittest_udpnc_decoder.cpp ../../coding_host/udpncrecoder.cpp abstract_udpnc_msg_receiver_testimpl.cpp
	$(MAKE) make_vastnet
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_udpnc_decoder)
	echo "Running: unittest_udpnc_decoder"
	$(unittest_udpnc_decoder)

unittest_net_udpnc_consumer: unittest_net_udpnc_consumer.cpp abstract_udpnc_msg_receiver_testimpl.cpp
	$(MAKE) make_vastnet
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_net_udpnc_consumer)
	echo "Running: unittest_net_udpnc_consumer"
	$(unittest_net_udpnc_consumer)

unittest_vastnet: unittest_vastnet.cpp
	$(MAKE) make_vastnet
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_vastnet)
	echo "Running: unittest_vastnet"
	$(unittest_vastnet)

make_vastnet:
	$(MAKE) --directory=../

clean:
	-rm $(unittest_net_udpnc_handler) $(unittest_udpncmessage) $(unittest_net_udpnc_mchandler) $(unittest_udpnc_decoder) $(unittest_net_udpnc_consumer) $(unittest_vastnet)

noace:
