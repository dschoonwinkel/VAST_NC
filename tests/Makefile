vaststatlog_test = ./vaststatlog_test

CC = g++

ACE_ROOT = /usr


LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon
#-lz -ldl -lpthread
LIBS_ACE    = -lACE
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization
LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_THREAD) $(LIBS_DL) $(LIBS_BOOST)

ACE_PATH    = /usr
INC_PATHS   = -I../common -I../VASTsim -I$(ACE_PATH)/include -I../VAST
LIB_PATHS   = -L../lib -L../Dependencies/lib/Release -L$(ACE_PATH)/lib

.PHONY: vaststatlog_test

vaststatlog_test: vaststatlog_test.cpp
	$(CC) -Wall -fPIC -g -std=c++14 $(INC_PATHS) $(LIB_PATHS) $< $(LIBS) \
	-o $(vaststatlog_test)

clean:
	-rm $(vaststatlog_test)

noace:

