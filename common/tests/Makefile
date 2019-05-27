unittest_logger = ./unittest_logger
unittest_concurrentqueue = ./unittest_concurrentqueue
unittest_vastmessage = ./unittest_vastmessage
unittest_messageserialize = ./unittest_messageserialize

CC = g++

ACE_ROOT = /usr


#CFLAGS=-O2 -ftree-vectorize -std=c++14 -Wall -Wextra
CFLAGS=-ggdb -std=c++14 -Wall -Wextra

LIBS_COMMON = -lvastcommon
LIBS_THREAD = -lpthread
LIBS_DL	    = -ldl
LIBS_BOOST = -lboost_serialization

LIBS = $(LIBS_COMMON) $(LIBS_BOOST)

INC_PATHS   = -I../../common
LIB_PATHS   = -L../../lib

.PHONY: unittest_logger unittest_concurrentqueue unittest_vastmessage make_vastcommon unittest_messageserialize

all: unittest_logger unittest_concurrentqueue unittest_vastmessage unittest_messageserialize

unittest_logger: unittest_logger.cpp
	$(MAKE) make_vastcommon
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $^ $(LIBS) \
	-o $(unittest_logger)
	echo "Running: unittest_logger"
	$(unittest_logger)

unittest_concurrentqueue: unittest_concurrentqueue.cpp
	$(MAKE) make_vastcommon
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $^ $(LIBS) -lpthread \
	-o $(unittest_concurrentqueue)
	echo "Running: unittest_concurrentqueue"
	$(unittest_concurrentqueue)

unittest_vastmessage: unittest_vastmessage.cpp
	$(MAKE) make_vastcommon
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $^ $(LIBS) -lpthread \
	-o $(unittest_vastmessage)
	echo "Running: unittest_vastmessage"
	$(unittest_vastmessage)

unittest_messageserialize: unittest_messageserialize.cpp
	$(MAKE) make_vastcommon
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $^ $(LIBS) -lpthread \
	-o $(unittest_messageserialize)
	echo "Running: unittest_messageserialize"
	$(unittest_messageserialize)

make_vastcommon:
	$(MAKE) --directory=../

clean:
	-rm $(unittest_logger) $(unittest_concurrentqueue) $(unittest_vastmessage) $(unittest_messageserialize)
