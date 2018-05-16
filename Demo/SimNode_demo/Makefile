
SimNode_demo = ../../bin/SimNode_demo

.PHONY: all noace

LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon -lz -lpthread
LIBS_ACE    = -lACE

ACE_ROOT = /usr

CFLAGS = -static -c -Wall -I../common -I$(ACE_ROOT)/include -g

ifeq ($(TARGET),noace)
    LIBS = $(LIBS_COMMON) 
else
    LIBS = $(LIBS_COMMON) $(LIBS_ACE)
endif

all: SimNode_demo.cpp
	g++ -Wall -I../../common -I../../VASTsim -L../../lib -L.. $< $(LIBS) \
	-o $(SimNode_demo)

noace:
	make TARGET=noace

clean:
	rm -f $(SimNode_demo)
