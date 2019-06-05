#ifndef NET_UDPNC_CONSUMER_H
#define NET_UDPNC_CONSUMER_H

#include "abstract_rlnc_msg_receiver.h"
#include "abstract_net_udp.h"
#include "ConcurrentQueue.h"
#include <boost/thread.hpp>
#include <mutex>

#define LOWEST_RESET_PACKET_ORDERING_NUMBER 240
#define HIGHEST_RESET_ACCEPTING_ORDERING_NUMBER 10

#include <boost/asio.hpp>

using namespace boost::asio;

namespace Vast{

    class net_udpNC_MChandler;

    class net_udpNC_consumer : public AbstractRLNCMsgReceiver
    {
    public:
        net_udpNC_consumer();
        ~net_udpNC_consumer();

        void open(AbstractRLNCMsgReceiver *RLNCsink,
                  abstract_net_udp *abs_netudp,
                  net_udpNC_MChandler* mchandler, bool startthread = true);

        void close();
        void RLNC_msg_received (RLNCMessage msg, IPaddr socket_addr);

    protected:
        //General processing, calls filter_input
        void process_input (RLNCMessage input_message, IPaddr socket_addr);
        friend class unittest_net_udpnc_consumer;

    private:
        void start_consuming();
        void consume();



        //Filters input based on toAddrs, passes to order_input
        void filter_input (RLNCMessage input_message, IPaddr socket_addr);
        //Ensures order of incoming packets remains correct, passes to handoff_input
        void order_input(RLNCMessage input_message, IPaddr socket_addr);

        AbstractRLNCMsgReceiver                 *RLNCsink = NULL;
        abstract_net_udp                        *abs_netudp = NULL;
        net_udpNC_MChandler                     *mchandler = NULL;

        std::map<id_t, uint8_t>                 recvd_ordering;
        ConcurrentQueue<RLNCMessage>            _msg_queue;

        bool                                    running = true;
        boost::thread                           *_consumerthread = NULL;

        size_t                                  total_packets_processed = 0;
        size_t                                  total_not_meantforme_unicast = 0;
        size_t                                  total_not_meantforme_multicast = 0;
        size_t                                  total_toolate_packets = 0;
        size_t                                  total_usedpackets = 0;
        std::mutex                              mutex_;
    };
}

#endif // NET_UDPNC_CONSUMER_H
