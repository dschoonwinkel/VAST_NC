#ifndef NET_UDPNC_HANDLER_H
#define NET_UDPNC_HANDLER_H

#include "net_udp_handler.h"
#include "net_udpnc_mchandler.h"
#include "rlnc_packet_factory.h"
#include "rlncmessage.h"
#include "abstract_rlnc_msg_receiver.h"
#include "net_udpnc_consumer.h"

namespace Vast
{
    class net_udpNC_handler : public net_udp_handler, public AbstractRLNCMsgReceiver
    {
    public:
        net_udpNC_handler(ip::udp::endpoint local_endpoint);
        virtual ~net_udpNC_handler();

        int open (io_service *io_service, abstract_net_udp *msghandler);
        // call net_udp_handler close and mchandler close
        int close (void);

        size_t send(const char *msg, size_t n, ip::udp::endpoint remote_endpoint);

        //Start the receiving loop
        void start_receive ();

        // handling incoming message
        int handle_input (const boost::system::error_code& error,
                          std::size_t bytes_transferred);

        //Hands input packet off to net_udp_handler
        void handoff_input (RLNCMessage input_message, ip::udp::endpoint* remote_endptr);



        void RLNC_msg_received(RLNCMessage input_message, ip::udp::endpoint* remote_endptr);

    private:
        uint8_t generation = 0;
        size_t pkt_gen_count = 0;

        net_udpNC_MChandler mchandler;
        net_udpNC_consumer consumer;

        size_t total_packets_recvd = 0;

        std::map<id_t, uint8_t> send_ordering;
    };


}

#endif // NET_UDPNC_HANDLER_H
