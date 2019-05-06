unittest_logger = ./unittest_logger
unittest_concurrentqueue = ./unittest_concurrentqueue


CC = g++

ACE_ROOT = /usr


#CFLAGS=-O2 -ftree-vectorize -std=c++14 -Wall -Wextra
CFLAGS=-ggdb -std=c++14 -Wall -Wextra

LIBS_COMMON = -lvastcommon
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl

LIBS = $(LIBS_COMMON)

INC_PATHS   = -I../../common
LIB_PATHS   = -L../../lib

.PHONY: unittest_logger unittest_concurrentqueue

all: unittest_logger unittest_concurrentqueue

unittest_logger: unittest_logger.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $^ $(LIBS) \
	-o $(unittest_logger)
	echo "Running: unittest_logger"
	$(unittest_logger)

unittest_concurrentqueue: unittest_concurrentqueue.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $^ $(LIBS) -lpthread \
	-o $(unittest_concurrentqueue)
	echo "Running: unittest_concurrentqueue"
	$(unittest_concurrentqueue)

clean:
	-rm $(unittest_logger) $(unittest_concurrentqueue)
