
random_walkertalker = ../../bin/random_walkertalker
test_helper = ./test_helper

.PHONY: all noace

# IMPORTANT NOTE: the least dependent library must begin from the rightmost (often vastcommon) 
LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon 
#-lz -ldl -lpthread
LIBS_ACE    = -lACE 
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization

LIBS_INCKODO = -I../../kodo-rlnc/resolve_symlinks/kodo-core/src \
			-I../../kodo-rlnc/resolve_symlinks/endian/src \
			-I../../kodo-rlnc/kodo_build/include

LIBS_KODO = -Wl,-Bstatic -L$(HOME)/Development/kodo-rlnc/kodo_build -lkodo_rlnc -lfifi -lcpuid -Wl,-Bdynamic

ACE_PATH    = /usr

# CFLAGS      = -Wall -static 
CFLAGS      = -Wall -fPIC -g -std=c++14

INC_PATHS   = -I../../common -I../../VASTsim -I$(ACE_PATH)/include -I../../VAST

LIB_PATHS   = -L../../lib -L../../Dependencies/lib/Release -L$(ACE_PATH)/lib 

LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL) $(LIBS_BOOST) $(LIBS_INCKODO) $(LIBS_KODO)

all: random_walkertalker.cpp random_walkertalker_helper.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $< $(LIBS) \
        -o $(random_walkertalker) random_walkertalker_helper.cpp

test: test_helper.cpp random_walkertalker_helper.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $< $(LIBS) \
        -o $(test_helper) random_walkertalker_helper.cpp

clean:
	-rm $(random_walkertalker) $(test_helper)
