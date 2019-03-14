#ifndef ABSTRACT_VAST_MSG_RECEIVER_H
#define ABSTRACT_VAST_MSG_RECEIVER_H

#include "VASTTypes.h"

namespace Vast
{

    class AbstractVastMsgReceiver
    {
    public:
        virtual bool msg_received (id_t fromhost, const char *message, size_t size, timestamp_t recvtime, bool in_front = false) = 0;
    };
}

#endif // ABSTRACT_VAST_MSG_RECEIVER_H
