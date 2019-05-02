#include "abstract_rlnc_msg_receiver_testimpl.h"

namespace Vast
{
    AbstractRNLCMsgReceiverTestImpl::AbstractRNLCMsgReceiverTestImpl()
    {

    }

    void AbstractRNLCMsgReceiverTestImpl::RLNC_msg_received (RLNCMessage input_message, ip::udp::endpoint *remote_endptr)
    {
//        CPPDEBUG("AbstractRNLCMsgReceiverTestImpl::RLNC_msg_received" << std::endl);
        recv_msg = input_message;
        RLNC_msg_received_call_count++;
        endptr = remote_endptr;
    }
}
