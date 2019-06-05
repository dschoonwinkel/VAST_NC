

lib_vastnet = ../lib/libvastnet.a
AR = ar
CC = g++

ACE_ROOT = /usr

CFLAGS = -static -c -Wall -Wextra -I../common -I$(ACE_ROOT)/include -I../coding_host -I../VASTStatLog -ggdb -std=c++14
#CFLAGS = -fPIC -c -Wall -I../common -I$(ACE_ROOT)

LIBS_INCKODO = -I../../kodo-rlnc/resolve_symlinks/kodo-core/src \
			-I../../kodo-rlnc/resolve_symlinks/endian/src \
			-I../../kodo-rlnc/kodo_build/include

LIBS_KODO = -Wl,-Bstatic -L/home/daniel/Development/kodo-rlnc/kodo_build -lkodo_rlnc -lfifi -lcpuid -Wl,-Bdynamic

#Note that ACE path is not included here, as we assume it's already properly installed on the system

sources_o = MessageHandler.cpp MessageQueue.cpp net_emu.cpp net_emubridge.cpp \
            net_manager.cpp VASTnet.cpp net_udp.cpp net_udp_handler.cpp dest_unr_listener.cpp \
	    net_udpNC_handler.cpp rlnc_packet_factory.cpp rlncmessage.cpp net_udpnc_mchandler.cpp \
	    rlncdecoder.cpp net_udpnc_consumer.cpp

sources_ace = net_ace.cpp net_ace_handler.cpp

ifeq ($(TARGET),noace)
  sources=$(sources_o)
else
  sources=$(sources_o) $(sources_ace)
endif

objects = $(subst .cpp,.o,$(sources))

all: $(lib_vastnet)	

# $@ refers $(lib_vast) and $^ refers to $(objects)
$(lib_vastnet): $(objects) abstract_input_processor.h
	$(AR) rcs $@ $^
	
.cpp.o:
	$(CC) $(CFLAGS) $(LIBS_INCKODO) $(LIBS_KODO) $^

clean:
	$(if $(objects), rm -f $(objects))
	$(if $(lib_vastnet), rm -f $(lib_vastnet))

noace:
	make TARGET=noace

