CC=g++

unittest_vast_capturemsgs = ./unittest_vast_capturemsgs
unittest_nc_capturemsgs = ./unittest_nc_capturemsgs
unittest_vastnetstatlog = ./unittest_vastnetstatlog

LIBS_COMMON = -lvastcommon -lvaststatlog
LIBS_BOOST = -lboost_system -lboost_thread -lboost_serialization -lboost_filesystem

# CFLAGS      = -Wall -static
CFLAGS      = -Wall -fPIC -ggdb -std=c++14 $(COPTS) 

INC_PATHS   = -I../../common -I../VASTStatLog -I../

LIB_PATHS   = -L../../lib

LIBS = $(LIBS_COMMON) $(LIBS_BOOST)

.PHONY: unittest_vast_capturemsgs unittest_nc_capturemsgs unittest_vastnetstatlog make_vaststatlog

all: make_vaststatlog unittest_vast_capturemsgs unittest_nc_capturemsgs unittest_vastnetstatlog

unittest_vast_capturemsgs: unittest_vast_capturemsgs.cpp
	$(MAKE) make_vaststatlog
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_vast_capturemsgs)
	echo "Running: unittest_vast_capturemsgs"
	$(unittest_vast_capturemsgs)
	echo "Consider running make clean && make to clean ./logs"

unittest_nc_capturemsgs: unittest_nc_capturemsgs.cpp
	$(MAKE) make_vaststatlog
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_nc_capturemsgs)
	echo "Running: unittest_nc_capturemsgs"
	$(unittest_nc_capturemsgs)
	echo "Consider running make clean && make to clean ./logs"

unittest_vastnetstatlog: unittest_vastnetstatlog.cpp
	$(MAKE) make_vaststatlog
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIBS_INCKODO) $(LIB_PATHS) $^ $(LIBS) $(LIBS_KODO) \
	-o $(unittest_vastnetstatlog)
	echo "Running: unittest_vastnetstatlog"
	-$(unittest_vastnetstatlog)

make_vaststatlog:
	$(MAKE) --directory=../

clean:
	-rm $(unittest_vast_capturemsgs) $(unittest_nc_capturemsgs) $(unittest_vastnetstatlog)
	-rm ./logs/msgcap/*.txt

noace:
