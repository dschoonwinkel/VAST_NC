test_vastrealinitpara = ./bin/test_vastrealinitpara

.PHONY: all noace

# IMPORTANT NOTE: the least dependent library must begin from the rightmost (often vastcommon) 
LIBS_COMMON = -lvastreal -lvast -lvastnet -lvastcommon 
#-lz -ldl -lpthread
LIBS_ACE    = -lACE 
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization

ACE_PATH    = /usr

# CFLAGS      = -Wall -static 
CFLAGS      = -Wall -fPIC -g -std=c++14 -fsanitize=address

INC_PATHS   = -I../../common -I../../VASTreal -I$(ACE_PATH)/include -I../../VAST

LIB_PATHS   = -L../../lib -L../../Dependencies/lib/Release -L$(ACE_PATH)/lib 

LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL) $(LIBS_BOOST)

all: test_vastreal_initpara

test_vastreal_initpara: test_vastreal_initpara.cpp
	g++ $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $< $(LIBS) -o $(test_vastrealinitpara)

clean:
	rm -f $(test_vastrealinitpara)
