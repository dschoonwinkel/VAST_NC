#include "abstract_rlnc_msg_receiver_testimpl.h"

namespace Vast
{
    AbstractRNLCMsgReceiverTestImpl::AbstractRNLCMsgReceiverTestImpl()
    {

    }

    void AbstractRNLCMsgReceiverTestImpl::RLNC_msg_received (RLNCMessage msg)
    {
        recv_msg = msg;
    }
}
