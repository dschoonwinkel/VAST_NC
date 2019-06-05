#ifndef ABSTRACTRNLCMSGRECEIVERTESTIMPL_H
#define ABSTRACTRNLCMSGRECEIVERTESTIMPL_H

#include "abstract_rlnc_msg_receiver.h"
#include <boost/asio.hpp>
#include <vector>

using namespace boost::asio;

namespace Vast
{

    class AbstractRNLCMsgReceiverTestImpl : public AbstractRLNCMsgReceiver
    {
    public:
        AbstractRNLCMsgReceiverTestImpl();
        ~AbstractRNLCMsgReceiverTestImpl();

        void RLNC_msg_received (RLNCMessage input_message, IPaddr socket_addr);


        RLNCMessage recv_msg;
        std::vector<RLNCMessage> all_recv_msgs;
        size_t RLNC_msg_received_call_count = 0;
        IPaddr socket_addr;
    };
}

#endif // ABSTRACTRNLCMSGRECEIVERTESTIMPL_H
