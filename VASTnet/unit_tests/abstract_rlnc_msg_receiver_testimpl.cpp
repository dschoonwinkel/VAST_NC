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
    }
}
