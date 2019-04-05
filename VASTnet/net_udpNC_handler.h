#ifndef NET_UDPNC_HANDLER_H
#define NET_UDPNC_HANDLER_H

#include "net_udp_handler.h"
#include "net_udpnc_mchandler.h"
#include "rlnc_packet_factory.h"
#include "rlncmessage.h"
#include "abstract_rlnc_msg_receiver.h"

#define LOWEST_RESET_PACKET_ORDERING_NUMBER 240
#define HIGHEST_RESET_ACCEPTING_ORDERING_NUMBER 10

namespace Vast
{
    class net_udpNC_handler : public net_udp_handler, public AbstractRLNCMsgReceiver
    {
    public:
        net_udpNC_handler(ip::udp::endpoint local_endpoint);
        virtual ~net_udpNC_handler();

        int open (io_service *io_service, abstract_net_udp *msghandler);

        size_t send(const char *msg, size_t n, ip::udp::endpoint remote_endpoint);

        //Start the receiving loop
        void start_receive ();

        // handling incoming message
        int handle_input (const boost::system::error_code& error,
                          std::size_t bytes_transferred);

        //General processing, calls filter_input
        void process_input (RLNCMessage input_message, ip::udp::endpoint* remote_endptr);
        //Filters input based on toAddrs, passes to order_input
        void filter_input (RLNCMessage input_message, ip::udp::endpoint* remote_endptr);
        //Ensures order of incoming packets remains correct, passes to handoff_input
        void order_input(RLNCMessage input_message, ip::udp::endpoint* remote_endptr);
        //Hands input packet off to net_udp_handler
        void handoff_input (RLNCMessage input_message, ip::udp::endpoint* remote_endptr);



        void RLNC_msg_received(RLNCMessage msg);

    private:
        uint8_t generation = 0;
        size_t pkt_gen_count = 0;

        net_udpNC_MChandler mchandler;
        size_t decoded_from_mchandler = 0;
        size_t total_packets_recvd = 0;
        size_t total_packets_processed = 0;
        size_t total_not_meantforme = 0;
        size_t total_toolate_packets = 0;
        size_t total_usedpackets = 0;

        std::map<id_t, uint8_t> recvd_ordering;
        std::map<id_t, uint8_t> send_ordering;
    };


}

#endif // NET_UDPNC_HANDLER_H
