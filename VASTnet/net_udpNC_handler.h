#ifndef NET_UDPNC_HANDLER_H
#define NET_UDPNC_HANDLER_H

#include "net_udp_handler.h"

namespace Vast
{
    class net_udpNC_handler : public net_udp_handler
    {
    public:
        net_udpNC_handler(ip::udp::endpoint local_endpoint);
    };
}

#endif // NET_UDPNC_HANDLER_H
