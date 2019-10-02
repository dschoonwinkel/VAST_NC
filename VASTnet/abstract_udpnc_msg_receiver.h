#ifndef ABSTRACT_UDPNC_MSG_RECEIVER_H
#define ABSTRACT_UDPNC_MSG_RECEIVER_H

#include "VASTTypes.h"
#include "udpncmessage.h"
#include <boost/asio.hpp>

using namespace boost::asio;

namespace Vast
{
    class AbstractUDPNCMsgReceiver
    {
    public:
        virtual ~AbstractUDPNCMsgReceiver() {}
        virtual void UDPNC_msg_received(UDPNCMessage input_message, IPaddr) = 0;
    };
}

#endif // ABSTRACT_UDPNC_MSG_RECEIVER_H
