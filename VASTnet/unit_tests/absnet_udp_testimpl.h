#ifndef ABSNET_UDP_TESTIMPL_H
#define ABSNET_UDP_TESTIMPL_H

#include "abstract_net_udp.h"

namespace Vast
{
    class absnet_udp_testimpl : public abstract_net_udp
    {
    public:
        absnet_udp_testimpl(): ip("127.0.0.1", 1037)
        {}
        absnet_udp_testimpl(IPaddr ip_addr) : ip(ip_addr)
        {}

        bool msg_received(id_t fromhost, const char *message, size_t size, timestamp_t recvtime, bool in_front)
        {
            this->fromhost = fromhost;
            this->message = std::string(message, size);
            this->size = size;
            this->recvtime = recvtime;
            this->in_front = in_front;

            return true;
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

        timestamp_t getTimestamp()
        {
            return timestamp++;
        }

        const IPaddr getPublicIPaddr()
        {
            return ip;
        }
        net_udp *getReal_net_udp()
        {
            return NULL;
        }

        size_t sock_conn_called = 0;
        size_t sock_disc_called = 0;
        id_t fromhost;
        std::string message;
        size_t size;
        timestamp_t recvtime;
        bool in_front;
        timestamp_t timestamp = 0;
        IPaddr ip;
    };
}

#endif // ABSNET_UDP_TESTIMPL_H
