lib_vastsim = ../lib/libvastreal.a
AR = ar
CC = g++
CFLAGS = -static -c -Wall -I../common -I../VAST -I../VASTStatLog -O -ggdb -g3 -std=c++14 -fsanitize=address -fsanitize=thread
#CFLAGS = -fPIC -c -Wall -I../common


sources = RealNode.cpp  VASTreal.cpp
objects = $(subst .cpp,.o,$(sources))

noace:
		make all

all: $(lib_vastsim)

# $@ refers $(lib_vast) and $^ refers to $(objects)
$(lib_vastsim): $(objects)
		$(AR) rcs $@ $^

.cpp.o:
		$(CC) $(CFLAGS) $^

clean:
		$(if $(objects), rm -f $(objects))
		$(if $(lib_vastsim), rm -f $(lib_vastsim))

