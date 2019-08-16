#include "abstract_rlnc_msg_receiver_testimpl.h"

namespace Vast
{
    AbstractRNLCMsgReceiverTestImpl::AbstractRNLCMsgReceiverTestImpl()
    {

    }

    void AbstractRNLCMsgReceiverTestImpl::RLNC_msg_received (RLNCMessage input_message, IPaddr socket_addr)
    {
//        CPPDEBUG("AbstractRNLCMsgReceiverTestImpl::RLNC_msg_received" << std::endl);
        recv_msg = input_message;
        RLNC_msg_received_call_count++;
        this->socket_addr = socket_addr;
        all_recv_msgs.push_back(input_message);
    }

    AbstractRNLCMsgReceiverTestImpl::~AbstractRNLCMsgReceiverTestImpl()
    {
        std::cout << "~AbstractRNLCMsgReceiverTestImpl: all_recv_msgs.size() " << all_recv_msgs.size() << std::endl;
    }
}
