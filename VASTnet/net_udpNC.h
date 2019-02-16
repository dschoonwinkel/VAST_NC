#ifndef NET_UDPNC_H
#define NET_UDPNC_H

#include "net_udp.h"

namespace Vast {
    class net_udpNC : public Vast::net_udp
    {
    public:
        net_udpNC (uint16_t port, const char* bindAddress = "");
    };
}

#endif // NET_UDPNC_H
