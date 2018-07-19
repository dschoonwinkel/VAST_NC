
VASTsim_console = ../../bin/VASTsim_console

.PHONY: all noace

LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon -lz -lpthread
LIBS_ACE    = -lACE
LIBS_BOOST = -lboost_system -lboost_thread

ACE_ROOT = /usr

CFLAGS = -static -c -Wall -g -std=c++14 -I$(ACE_ROOT)/include -I../../common -I../../VASTsim -L../../lib -L..

ifeq ($(TARGET),noace)
	LIBS = $(LIBS_COMMON) 
else
	LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_BOOST)
endif

all: VASTsim_console.cpp
	g++ $(CFLAGS) $< $(LIBS) -o $(VASTsim_console)

noace:
	make TARGET=noace

clean:
	rm -f $(VASTsim_console)
