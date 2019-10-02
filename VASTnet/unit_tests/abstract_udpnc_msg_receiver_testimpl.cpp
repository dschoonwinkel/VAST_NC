#include "abstract_udpnc_msg_receiver_testimpl.h"

namespace Vast
{
    AbstractUDPNCMsgReceiverTestImpl::AbstractUDPNCMsgReceiverTestImpl()
    {

    }

    void AbstractUDPNCMsgReceiverTestImpl::UDPNC_msg_received (UDPNCMessage input_message, IPaddr socket_addr)
    {
//        CPPDEBUG("AbstractUDPNCMsgReceiverTestImpl::UDPNC_msg_received" << std::endl);
        recv_msg = input_message;
        UDPNC_msg_received_call_count++;
        this->socket_addr = socket_addr;
        all_recv_msgs.push_back(input_message);
    }

    AbstractUDPNCMsgReceiverTestImpl::~AbstractUDPNCMsgReceiverTestImpl()
    {
        std::cout << "~AbstractUDPNCMsgReceiverTestImpl: all_recv_msgs.size() " << all_recv_msgs.size() << std::endl;
    }
}
