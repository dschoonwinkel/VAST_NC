CC=g++

coding_host = ./test_recoding_decoding
basic_encoder_test = ./basic_encoder_test
test_uncoded_coded_mixed = ./test_uncoded_coded_mixed
test_uncoded_coded_mixed2 = ./test_uncoded_coded_mixed2
unittest_udpncrecoder= ./unittest_udpncrecoder


#CFLAGS=-O2 -ftree-vectorize -std=c++14 -Wall -Wextra -Wno-expansion-to-defined
CFLAGS=-ggdb -std=c++14 -Wall -Wextra $(COPTS) 

LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon
#-lz -ldl -lpthread
LIBS_ACE    = -lACE
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization -lboost_iostreams -lboost_thread -lboost_chrono -lpthread
LIBS_INCKODO = -I../../../kodo-rlnc/resolve_symlinks/kodo-core/src \
			-I../../../kodo-rlnc/resolve_symlinks/endian/src \
			-I../../../kodo-rlnc/kodo_build/include

LIBS_KODO = -Wl,-Bstatic -L$(HOME)/Development/kodo-rlnc/kodo_build -lkodo_rlnc -lfifi -lcpuid -Wl,-Bdynamic
LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL) $(LIBS_BOOST)

ACE_PATH    = /usr
INC_PATHS   = -I.. -I../../common -I../../VASTsim -I$(ACE_PATH)/include -I../../VAST -I../../VASTnet
LIB_PATHS   = -L../../lib -L../../Dependencies/lib/Release -L$(ACE_PATH)/lib

.PHONY: unittest_udpncrecoder

all: coding_host basic_encoder_test test_uncoded_coded_mixed test_uncoded_coded_mixed2 unittest_udpncrecoder

coding_host: test_recoding_decoding.cpp ../packet_listener.cpp ../udpncrecoder.cpp ../../VASTnet/udpnc_decoder.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(coding_host)

basic_encoder_test: basic_encoder_test.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(basic_encoder_test)

test_uncoded_coded_mixed: test_uncoded_coded_mixed.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(test_uncoded_coded_mixed)

test_uncoded_coded_mixed2: test_uncoded_coded_mixed2.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(test_uncoded_coded_mixed2)

unittest_udpncrecoder: unittest_udpncrecoder.cpp ../packet_listener.cpp ../udpncrecoder.cpp ../../VASTnet/udpnc_decoder.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_udpncrecoder)
	$(unittest_udpncrecoder)

clean:
	-rm $(coding_host) $(test_uncoded_coded_mixed) $(test_uncoded_coded_mixed2) $(basic_encoder_test) $(unittest_udpncrecoder)


