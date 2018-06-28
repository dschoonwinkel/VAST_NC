
myVASTsim = ../../bin/myVASTsim

.PHONY: all noace

LIBS_COMMON = -lvastsim -lvast -lvastnet -lvastcommon -lz -lpthread
LIBS_ACE    = -lACE
LIBS_BOOST = -lboost_system -lboost_thread

ACE_ROOT = /usr

CFLAGS = -static -c -Wall -I../common -I$(ACE_ROOT)/include -g

ifeq ($(TARGET),noace)
    LIBS = $(LIBS_COMMON) 
else
    LIBS = $(LIBS_COMMON) $(LIBS_ACE) $(LIBS_BOOST)
endif

all: myVASTsim.cpp
	g++ -Wall -I../../common -I../../VASTsim -L../../lib -L.. $< $(LIBS) \
	-o $(myVASTsim)

noace:
	make TARGET=noace

clean:
	rm -f $(myVASTsim)
