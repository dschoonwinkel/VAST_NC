#ifndef NET_UDPNC_MCHANDLER_H
#define NET_UDPNC_MCHANDLER_H

#include "VASTTypes.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "rlncmessage.h"
#include "customrlncdecoder.h"
#include "abstract_rlnc_msg_receiver.h"

using namespace boost::asio;

namespace Vast
{

    class net_udpNC_MChandler
    {
    public:
        net_udpNC_MChandler(ip::udp::endpoint local_endpoint);
        virtual ~net_udpNC_MChandler();

        //MChandler will run its own io_service
        int open (AbstractRLNCMsgReceiver *msghandler, bool startthread = true);

        // close connection & unregister from io_service
        int close (void);

        void process_input(const char *buf, std::size_t bytes_transferred);
        void process_encoded(const char *buf, std::size_t bytes_transferred);

        void putOtherRLNCMessage(RLNCMessage other);

        virtual void clearPacketPool();
        size_t getPacketPoolSize();

        bool toAddrForMe(RLNCMessage msg);

    protected:

        //Only called by mocking class
        net_udpNC_MChandler() {}

        //Start the receiving loop
        void start_receive ();

        // handling incoming message
        int handle_input (const boost::system::error_code& error,
                          std::size_t bytes_transferred);

        // if handle_input() returns -1, reactor would call handle_close()
        int handle_close ();

    private:
        ip::udp::socket             *_udp = NULL;
        ip::udp::endpoint           _remote_endpoint_;
        ip::udp::endpoint           _local_endpoint;
        ip::udp::endpoint           MC_address;
        char                        _buf[VAST_BUFSIZ];
        AbstractRLNCMsgReceiver     *_msghandler = NULL;


        // the same io_service as net_udp
        io_service                  *_io_service = NULL;
        boost::thread               *_iosthread = NULL;
        customrlncdecoder                 decoder;

        size_t packets_received = 0;
        size_t toaddrs_pkts_ignored = 0;
    };

}



#endif // NET_UDPNC_MCHANDLER_H
