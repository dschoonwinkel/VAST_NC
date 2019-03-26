#include "abstract_rlnc_msg_receiver_testimpl.h"

namespace Vast
{
    AbstractRNLCMsgReceiverTestImpl::AbstractRNLCMsgReceiverTestImpl()
    {

    }

    void AbstractRNLCMsgReceiverTestImpl::RLNC_msg_received (RLNCMessage msg)
    {
//        CPPDEBUG("AbstractRNLCMsgReceiverTestImpl::RLNC_msg_received" << std::endl);
        recv_msg = msg;
        RLNC_msg_received_call_count++;
    }
}
