unittest_logger = ./unittest_logger


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

.PHONY: unittest_logger

all: unittest_logger

unittest_logger: unittest_logger.cpp
	$(CC) $(CFLAGS) $(INC_PATHS) $(LIB_PATHS) $^ $(LIBS) \
	-o $(unittest_logger)
	echo "Running: unittest_logger"
	$(unittest_logger)

clean:
	-rm $(unittest_logger)
