#ifndef NET_UDPNC_HANDLER_H
#define NET_UDPNC_HANDLER_H

#include "net_udp_handler.h"
#include "rlnc_packet_factory.h"
#include "rlncmessage.h"

#define GENSIZE 2

namespace Vast
{
    class net_udpNC_handler : public net_udp_handler
    {
    public:
        net_udpNC_handler(ip::udp::endpoint local_endpoint);

        size_t send(const char *msg, size_t n, ip::udp::endpoint remote_endpoint);

        //Start the receiving loop
        void start_receive ();

        // handling incoming message
        int handle_input (const boost::system::error_code& error,
                          std::size_t bytes_transferred);



    private:
        int generation = 0;
        int ordering = 0;

    };


}

#endif // NET_UDPNC_HANDLER_H
