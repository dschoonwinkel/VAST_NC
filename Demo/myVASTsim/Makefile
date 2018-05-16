
myVASTsim = ../../bin/myVASTsim

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

all: myVASTsim.cpp
	g++ -Wall -I../../common -I../../VASTsim -L../../lib -L.. $< $(LIBS) \
	-o $(myVASTsim)

noace:
	make TARGET=noace

clean:
	rm -f $(myVASTsim)
