
myVASTsim = ../../bin/myVASTsim

.PHONY: all noace

LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon -lz -lpthread
LIBS_ACE    = -lACE
LIBS_BOOST = -lboost_system -lboost_thread

ACE_ROOT = /usr

CFLAGS = -static -c -Wall -std=c++14 -g -I../common -I$(ACE_ROOT)/include -I../../common -I../../VASTsim -L../../lib -L..

ifeq ($(TARGET),noace)
	LIBS = $(LIBS_COMMON) 
else
	LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_BOOST)
endif

all: myVASTsim.cpp
	g++ $(CFLAGS) $< $(LIBS) -o $(myVASTsim)

noace:
	make TARGET=noace

clean:
	rm -f $(myVASTsim)
