#ifndef NET_UDPNC_MCHANDLERMOCK_H
#define NET_UDPNC_MCHANDLERMOCK_H

#include "net_udpnc_mchandler.h"

namespace Vast
{
    class net_udpNC_MChandlerMock : public net_udpNC_MChandler
    {
    public:
        net_udpNC_MChandlerMock()
        {

        }

        ~net_udpNC_MChandlerMock()
        {

        }

        void clearPacketPool ()
        {
            clearPacketPoolCalled++;
        }

        size_t clearPacketPoolCalled = 0;
    };
}

#endif // NET_UDPNC_MCHANDLERMOCK_H
