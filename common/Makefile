
resolvehostid = /usr/local/bin/resolvehostid
lib_vastcommon = ../lib/libvastcommon.a
AR = ar
CC = g++

#CFLAGS = -static -c -Wall -I../Dependencies/include/ace -I../Dependencies/include

ACE_ROOT = /usr

#replace -fPIC with -static for static linking
CFLAGS = -static -c -Wall -I$(ACE_ROOT)/include -std=c++14 -fsanitize=address -fsanitize=thread
#CFLAGS = -fPIC -c -Wall -I$(ACE_ROOT) 
#-I../Dependencies/include

sources_o = Compressor.cpp Errout.cpp \
            SectionedFile.cpp StdIO_SectionedFile.cpp \
            VoronoiSF.cpp VoronoiSFAlgorithm.cpp \
            MovementGenerator.cpp \
	    VASTUtil.cpp \
	    logger.cpp \
	    ipaddr_warning.cpp \
	    timeouts.cpp \
	    autodestructortimer.cpp
            
sources=$(sources_o)

objects = $(subst .cpp,.o,$(sources))

all: $(lib_vastcommon)

# $@ refers $(lib_vast) and $^ refers to $(objects)
$(lib_vastcommon): $(objects) VASTTypes.h ConcurrentQueue.h timeouts.h
	$(AR) rcs $@ $^
	
.cpp.o:
	$(CC) $(CFLAGS) $^

resolvehostid: resolvehostid.cpp
	-rm $(resolvehostid)
	$(CC) $^ -o $(resolvehostid)

clean:
	$(if $(objects), rm -f $(objects))
	$(if $(lib_vastcommon), rm -f $(lib_vastcommon))
	-rm $(resolvehostid)


noace:
	make TARGET=noace


