#ifndef ABSTRACT_RLNC_MSG_RECEIVER_H
#define ABSTRACT_RLNC_MSG_RECEIVER_H

#include "VASTTypes.h"
#include "rlncmessage.h"
#include <boost/asio.hpp>

using namespace boost::asio;

namespace Vast
{
    class AbstractRLNCMsgReceiver
    {
    public:
        virtual ~AbstractRLNCMsgReceiver() {}
        virtual void RLNC_msg_received(RLNCMessage input_message, IPaddr) = 0;
    };
}

#endif // ABSTRACT_RLNC_MSG_RECEIVER_H
