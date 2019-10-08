lib_vaststatlog = ../lib/libvaststatlog.a
AR = ar

ACE_ROOT = /usr

CFLAGS = -static -c -Wall -Wextra -I../common -ggdb -std=c++14 $(COPTS) 
#CFLAGS = -fPIC -c -Wall -I../common -I../VASTnet  -I$(ACE_ROOT)

sources = vaststatlog_entry.cpp vaststatlog.cpp \
          vastnetstatlog_entry.cpp vastnetstatlog.cpp \
          vast_capturemsgs.cpp netudpnc_capturemsgs.cpp \
          vastlatencystatlog.cpp vastlatencystatlog_entry.cpp
 
objects = $(subst .cpp,.o,$(sources))

#VAST depends on VASTcommon ?
#$(lib_vast): $(VASTcommon) 
 
all: $(lib_vaststatlog)

# $@ refers $(lib_vast) and $^ refers to $(objects)
$(lib_vaststatlog): $(objects) messagewrapper.h ncwrapper.h
	$(AR) rcs $@ $^
	
.cpp.o:
	$(CC) $(CFLAGS) $^

clean:
	$(if $(objects), rm -f $(objects))
	$(if $(lib_vaststatlog), rm -f $(lib_vaststatlog))
