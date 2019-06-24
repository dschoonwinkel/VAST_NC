
#VASTcommon := ../common

lib_vast = ../lib/libvast.a
AR = ar

ACE_ROOT = /usr

CFLAGS = -static -c -Wall -Wextra -I../common -I../VASTnet  -I$(ACE_ROOT)/include -ggdb -std=c++14 $(COPTS) 
#CFLAGS = -fPIC -c -Wall -I../common -I../VASTnet ../VASTStatLog -I$(ACE_ROOT)

sources = VASTClient.cpp  VASTMatcher.cpp  VASTRelay.cpp  VASTVerse.cpp  VASTThread.cpp VONPeer.cpp  VSOPeer.cpp
 
objects = $(subst .cpp,.o,$(sources))

#VAST depends on VASTcommon ?
#$(lib_vast): $(VASTcommon) 
 
all: $(lib_vast) 	

# $@ refers $(lib_vast) and $^ refers to $(objects)
$(lib_vast): $(objects)
	$(AR) rcs $@ $^
	
.cpp.o:
	$(CC) $(CFLAGS) $^

clean:
	$(if $(objects), rm -f $(objects))
	$(if $(lib_vast), rm -f $(lib_vast))

noace:
