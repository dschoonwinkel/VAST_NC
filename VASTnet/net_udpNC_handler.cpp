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



}
