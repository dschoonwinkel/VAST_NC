#ifndef NET_UDPNC_HANDLER_H
#define NET_UDPNC_HANDLER_H

#include "net_udp_handler.h"
#include "rlnc_packet_factory.h"
#include "rlncmessage.h"
#include "abstract_rlnc_msg_receiver.h"

namespace Vast
{
    class net_udpNC_handler : public net_udp_handler, public AbstractRLNCMsgReceiver
    {
    public:
        net_udpNC_handler(ip::udp::endpoint local_endpoint);
        ~net_udpNC_handler();

        size_t send(const char *msg, size_t n, ip::udp::endpoint remote_endpoint);

        //Start the receiving loop
        void start_receive ();

        // handling incoming message
        int handle_input (const boost::system::error_code& error,
                          std::size_t bytes_transferred);

        void process_input (RLNCMessage input_message);



        void RLNC_msg_received(RLNCMessage msg);

    private:
        int generation = 0;
        int ordering = 0;

    };


}

#endif // NET_UDPNC_HANDLER_H
