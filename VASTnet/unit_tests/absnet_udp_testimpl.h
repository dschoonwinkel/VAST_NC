#ifndef ABSNET_UDP_TESTIMPL_H
#define ABSNET_UDP_TESTIMPL_H

#include "abstract_net_udp.h"

namespace Vast
{
    class absnet_udp_testimpl : public abstract_net_udp
    {
    public:
        bool msg_received(id_t fromhost, const char *message, size_t size, timestamp_t recvtime, bool in_front)
        {
            this->fromhost = fromhost;
            this->message = message;
            this->size = size;
            this->recvtime = recvtime;
            this->in_front = in_front;
        }
        bool socket_connected(id_t, void *, bool )
        {
            sock_conn_called++;
            return true;
        }
        bool socket_disconnected(id_t)
        {
            sock_disc_called++;
            return true;
        }

        size_t sock_conn_called = 0;
        size_t sock_disc_called = 0;
        id_t fromhost;
        char * message;
        size_t size;
        timestamp_t recvtime;
        bool in_front;
    };
}

#endif // ABSNET_UDP_TESTIMPL_H
