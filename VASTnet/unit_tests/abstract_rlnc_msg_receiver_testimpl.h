#ifndef ABSTRACTRNLCMSGRECEIVERTESTIMPL_H
#define ABSTRACTRNLCMSGRECEIVERTESTIMPL_H

#include "abstract_rlnc_msg_receiver.h"
#include <boost/asio.hpp>

using namespace boost::asio;

namespace Vast
{

    class AbstractRNLCMsgReceiverTestImpl : public AbstractRLNCMsgReceiver
    {
    public:
        AbstractRNLCMsgReceiverTestImpl();

        void RLNC_msg_received (RLNCMessage input_message, ip::udp::endpoint *remote_endptr);


        RLNCMessage recv_msg;
        size_t RLNC_msg_received_call_count = 0;
        ip::udp::endpoint *endptr;
    };
}

#endif // ABSTRACTRNLCMSGRECEIVERTESTIMPL_H
