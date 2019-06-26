udpnc_integration_test = ./udpnc_integration_test
rlncdissector=./rlncdissector
differentlen_codingtest = ./differentlen_codingtest

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
INC_PATHS   = -I../../common -I../../VASTsim -I$(ACE_PATH)/include -I../../VAST -I../../VASTnet -I../../coding_host -I../../VASTStatLog -I../unit_tests
LIB_PATHS   = -L../../lib -L../../Dependencies/lib/Release -L$(ACE_PATH)/lib

.PHONY: udpnc_integration_test rlncdissector differentlen_codingtest

all: udpnc_integration_test differentlen_codingtest

udpnc_integration_test: udpnc_integration_test.cpp ../unit_tests/absnet_udp_testimpl.h ../unit_tests/abstract_rlnc_msg_receiver_testimpl.cpp ../../coding_host/packet_listener.cpp ../../coding_host/rlncrecoder.cpp
	$(MAKE) make_vastnet
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(udpnc_integration_test)
#	echo "Running: udpnc_integration_test"
#	$(udpnc_integration_test)

rlncdissector: rlncdissector.cpp
	$(MAKE) make_vastnet
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $^ $(LIBS) \
	-o $(rlncdissector)

differentlen_codingtest: differentlen_codingtest.cpp ../../coding_host/rlncrecoder.cpp
	$(MAKE) make_vastnet
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(differentlen_codingtest)
	echo "Running: differentlen_codingtest"
	$(differentlen_codingtest)

make_vastnet:
	$(MAKE) --directory=../

clean:
	-rm $(udpnc_integration_test) $(differentlen_codingtest)
