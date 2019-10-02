#ifndef ABSTRACTUDPNCMSGRECEIVERTESTIMPL_H
#define ABSTRACTUDPNCMSGRECEIVERTESTIMPL_H

#include "abstract_udpnc_msg_receiver.h"
#include <boost/asio.hpp>
#include <vector>

using namespace boost::asio;

namespace Vast
{

    class AbstractUDPNCMsgReceiverTestImpl : public AbstractUDPNCMsgReceiver
    {
    public:
        AbstractUDPNCMsgReceiverTestImpl();
        ~AbstractUDPNCMsgReceiverTestImpl();

        void UDPNC_msg_received (UDPNCMessage input_message, IPaddr socket_addr);


        UDPNCMessage recv_msg;
        std::vector<UDPNCMessage> all_recv_msgs;
        size_t UDPNC_msg_received_call_count = 0;
        IPaddr socket_addr;
    };
}

#endif // ABSTRACTUDPNCMSGRECEIVERTESTIMPL_H
