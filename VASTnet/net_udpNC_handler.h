#ifndef NET_UDPNC_HANDLER_H
#define NET_UDPNC_HANDLER_H

#include "net_udp_handler.h"
#include "net_udpnc_mchandler.h"
#include "udpnc_packet_factory.h"
#include "udpncmessage.h"
#include "abstract_udpnc_msg_receiver.h"
#include "net_udpnc_consumer.h"
#include "timeouts.h"
#include "abstract_input_processor.h"

namespace Vast
{
    class net_udpNC_handler : public net_udp_handler, public AbstractUDPNCMsgReceiver, public abstract_input_processor
    {
    public:
        net_udpNC_handler(ip::udp::endpoint local_endpoint);
        virtual ~net_udpNC_handler();


        ///Default open function. Uses test case function with UDPNCsink = NULL parameter
        int open (io_service *io_service, abstract_net_udp *msghandler, bool startthread = true);

        ///Used in test cases where I need a different UDPNCsink output
        int open (io_service *io_service, abstract_net_udp *msghandler, AbstractUDPNCMsgReceiver *UDPNCsink = NULL, bool startthread = true);
        // call net_udp_handler close and mchandler close
        int close (void);

        size_t send(const char *msg, size_t n, ip::udp::endpoint remote_endpoint);
        size_t send_keepalive(ip::udp::endpoint remote_endpoint);

        // processing work done here
        void process_input (const char *buf, ip::udp::endpoint remote_endpoint,
                          std::size_t bytes_transferred);

        //send encoded packet to mchandler to process
        void process_encoded(const char *buf,
                             std::size_t bytes_transferred);

        //Hands input packet off to net_udp_handler
        void handoff_input (UDPNCMessage input_message, IPaddr socket_addr);

        void UDPNC_msg_received(UDPNCMessage input_message, IPaddr socket_addr);

        //Do nothing here, for now
        void tick();

    protected:
        //Start the receiving loop
        void start_receive ();

        // handling incoming message, pass to process_input
        int handle_input (const boost::system::error_code& error,
                          std::size_t bytes_transferred);

        size_t send_helper(const char *msg, size_t n, ip::udp::endpoint remote_endpoint);

    private:
        uint8_t generation = 0;
        size_t pkt_gen_count = 0;

        net_udpNC_MChandler mchandler;
        net_udpNC_consumer consumer;

        std::map<id_t, uint8_t> send_ordering;

        timestamp_t         _timeout_keepalive;      // timeout for re-attempt to join

        size_t packets_MChandler_handover = 0;


    };


}

#endif // NET_UDPNC_HANDLER_H
