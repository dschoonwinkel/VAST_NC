#ifndef ABSTRACT_NET_UDP_H
#define ABSTRACT_NET_UDP_H

#include "VASTTypes.h"

namespace Vast
{
    class net_udp;

    class abstract_net_udp
    {
    public:
        virtual bool msg_received (id_t fromhost, const char *message, size_t size, timestamp_t recvtime = 0, bool in_front = false) = 0;

        virtual bool socket_connected (id_t id, void *stream, bool is_secure) = 0;
        virtual bool socket_disconnected (id_t id) = 0;
        virtual const IPaddr getPublicIPaddr() = 0;
        virtual net_udp* getReal_net_udp()
        {
            return NULL;
        }
    };
}



#endif // ABSTRACT_NET_UDP_H
