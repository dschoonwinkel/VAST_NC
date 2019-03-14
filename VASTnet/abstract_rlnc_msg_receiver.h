#ifndef ABSTRACT_RLNC_MSG_RECEIVER_H
#define ABSTRACT_RLNC_MSG_RECEIVER_H

#include "VASTTypes.h"
#include "rlncmessage.h"

namespace Vast
{
    class AbstractRLNCMsgReceiver
    {
    public:
        virtual void RLNC_msg_received(RLNCMessage msg) = 0;
    };
}

#endif // ABSTRACT_RLNC_MSG_RECEIVER_H
