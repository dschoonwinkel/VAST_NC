#include "net_udpNC_handler.h"

namespace Vast
{
    net_udpNC_handler::net_udpNC_handler(ip::udp::endpoint local_endpoint):
        net_udp_handler(local_endpoint)
    {

    }

    size_t net_udpNC_handler::send(const char *msg, size_t n, ip::udp::endpoint remote_endpoint)
    {
        if (_udp == NULL)
        {
            std::cerr << "net_udpNC_handler::send trying to send before socket is ready" << std::endl;
            return -1;

        }

        RLNCHeader_factory header_factory(GENSIZE, generation);
        RLNCMessage message(header_factory.build());

        message.putMessage(msg, n);
        message.putPacketId(1);

        char *buf = new char[message.sizeOf()];

        int sending_len = message.serialize(buf);

        return _udp->send_to(buffer(buf, sending_len), remote_endpoint);
    }

    void net_udpNC_handler::start_receive()
    {
        _udp->async_receive_from(
            boost::asio::buffer(_buf, VAST_BUFSIZ), _remote_endpoint_,
            boost::bind(&net_udpNC_handler::handle_input, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }

    // handling incoming message
    int net_udpNC_handler::handle_input (const boost::system::error_code& error,
          std::size_t bytes_transferred)
    {
        size_t n = bytes_transferred;
    //    Vast::VASTHeader header;
        RLNCHeader header;

        if (!error)
        {
            //Store UDP messages

            std::cout << "Received a message!" << std::endl;

            char *p = _buf;

            //NOTE: there may be several valid UDP messages received at once -- is this really necessary?
            //while (n > sizeof (RLNCHeader))
    //        {
    //            //extract message header
    //            mempcpy (&header, p, sizeof (Vast::VASTHeader));
    //            n -= sizeof(Vast::VASTHeader);
    //            p += sizeof(Vast::VASTHeader);
            memcpy(&header, p, sizeof(RLNCHeader));

                //Check if it is really a VAST message: Start and end bytes of header should be correct
                if (!RLNCHeader_factory::isRLNCHeader(header))
                {
                    std::cout << "net_udp_handler::handle_input Non-RLNC message received on UDP socket" << std::endl;
    //                return -1;
                }
                else {
                    std::cout << "RLNC message received on the coding host" << std::endl;
                }


            //Restart waiting for new packets
            start_receive();
        }
        else {
            CPPDEBUG("Error on UDP socket receive: " << error.message() << std::endl;);
        }
        return -1;
    }

}
