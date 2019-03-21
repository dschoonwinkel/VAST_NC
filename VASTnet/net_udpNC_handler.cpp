#include "net_udpNC_handler.h"

namespace Vast
{
    net_udpNC_handler::net_udpNC_handler(ip::udp::endpoint local_endpoint):
        net_udp_handler(local_endpoint)
    {
        CPPDEBUG("Starting net_udpNC_handler" << std::endl);

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
        ordering += 1;
        //TODO: put ordering into packet header


        id_t myID = ((net_manager*)_msghandler)->getID ();

        message.putPacketId(RLNCMessage::generatePacketId (myID, ordering));
        message.putOrdering (ordering);
        message.putFromId (myID);
        std::vector<char> buf(message.sizeOf());

        int sending_len = message.serialize(buf.data ());

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
        RLNCHeader header;

        if (!error)
        {
            //Store UDP messages
//            CPPDEBUG("net_udpNC_handler::handle_input Received a message!" << std::endl);

            char *p = _buf;

            memcpy(&header, p, sizeof(RLNCHeader));

                //Check if it is really a VAST message: Start and end bytes of header should be correct
                if (!RLNCHeader_factory::isRLNCHeader(header))
                {
//                    CPPDEBUG("net_udp_handler::handle_input Non-RLNC message received on UDP socket" << std::endl);
                }
                else if (RLNCHeader_factory::isRLNCHeader (header) && header.enc_packet_count > 1)
                {
                    CPPDEBUG("net_udpNC_handler::handle_input: Encoded packet received -- THIS IS NOT SUPPOSED TO HAPPEN" << std::endl);

                }
                else {
//                    CPPDEBUG("net_udpNC_handler::handle_input RLNC message received on the coding host" << std::endl);

                    RLNCMessage input_message;
                    input_message.deserialize (_buf, bytes_transferred);
                    process_input (input_message);

                }


            //Restart waiting for new packets
            start_receive();
        }
        else {
            CPPDEBUG("Error on UDP socket receive: " << error.message() << std::endl;);
        }
        return -1;
    }

    void net_udpNC_handler::process_input (RLNCMessage input_message)
    {
//        CPPDEBUG("net_udpNC_handler::handle_input: received packet ordering: " << (int)input_message.getOrdering ()
//                 << "\nprevious ordered packet: "
//                 << (int)recvd_ordering[input_message.getFirstFromId ()] << std::endl);

//}
        net_udp_handler::process_input(input_message.getMessage (), input_message.getMessageSize ());

    }

    void net_udpNC_handler::RLNC_msg_received(RLNCMessage msg)
    {
//        CPPDEBUG("net_udpNC_handler::RLNC_msg_received Decoded from mc_handler: " << decoded_from_mchandler << std::endl);
//        CPPDEBUG("net_udpNC_handler::RLNC_msg_received first from_id: " << msg.getFirstFromId ()<< std::endl);

        process_input (msg);
    }

    net_udpNC_handler::~net_udpNC_handler()
    {
        CPPDEBUG("~net_udpNC_handler: decoded_from_mchandler: " << std::endl);
        CPPDEBUG("~net_udpNC_handler: used_from_mchandler: " << std::endl);
    }

}
