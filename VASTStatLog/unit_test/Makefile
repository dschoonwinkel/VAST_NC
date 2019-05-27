unittest_vast_capturemsgs = ./unittest_vast_capturemsgs

CC = g++

LIBS_COMMON = -lvastcommon -lvaststatlog
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization -lboost_filesystem

# CFLAGS      = -Wall -static
CFLAGS      = -Wall -fPIC -ggdb -std=c++14

INC_PATHS   = -I../../common -I../VASTStatLog -I../

LIB_PATHS   = -L../../lib

LIBS = $(LIBS_COMMON) $(LIBS_BOOST)

.PHONY: unittest_vast_capturemsgs make_vaststatlog

all: make_vaststatlog unittest_vast_capturemsgs

unittest_vast_capturemsgs: unittest_vast_capturemsgs.cpp
	$(MAKE) make_vaststatlog
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_vast_capturemsgs)
	echo "Running: unittest_vast_capturemsgs"
	$(unittest_vast_capturemsgs)
	echo "Consider running make clean && make to clean ./logs"

make_vaststatlog:
	$(MAKE) --directory=../

clean:
	-rm $(unittest_vast_capturemsgs)
	-rm ./logs/*.txt

noace:
