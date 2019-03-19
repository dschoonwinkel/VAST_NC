#ifndef ABSTRACTRNLCMSGRECEIVERTESTIMPL_H
#define ABSTRACTRNLCMSGRECEIVERTESTIMPL_H

#include "abstract_rlnc_msg_receiver.h"

namespace Vast
{

    class AbstractRNLCMsgReceiverTestImpl : public AbstractRLNCMsgReceiver
    {
    public:
        AbstractRNLCMsgReceiverTestImpl();

        void RLNC_msg_received(RLNCMessage msg);


        RLNCMessage recv_msg;
    };
}

#endif // ABSTRACTRNLCMSGRECEIVERTESTIMPL_H
