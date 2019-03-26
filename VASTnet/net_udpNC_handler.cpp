#include "net_udpNC_handler.h"

namespace Vast
{
    net_udpNC_handler::net_udpNC_handler(ip::udp::endpoint local_endpoint):
        net_udp_handler(local_endpoint)
    {
        CPPDEBUG("Starting net_udpNC_handler" << std::endl);

    }

    int net_udpNC_handler::open (io_service *io_service, abstract_net_udp *msghandler)
    {
        net_udp_handler::open(io_service, msghandler);
        mchandler.open (this);

        return 0;
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
        IPaddr to_addr(remote_endpoint.address().to_v4().to_ulong(), remote_endpoint.port ());
        message.putToAddr (to_addr);

        //Add to the decoder, if later used for decoding
        mchandler.putOtherRLNCMessage (message);


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
                    //Add uncoded messages to the packet pool to aid in decoding later
//                    mchandler.putOtherRLNCMessage (input_message);
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
//        CPPDEBUG(input_message.getToAddrs ()[0] << std::endl);
        if (_msghandler->getPublicIPaddr () == input_message.getToAddrs ()[0])
        {
            //All is well, continue
            //CPPDEBUG("net_udpNC_handler::process_input Message ToAddr was meant for me..." << std::endl);
        }
        else
        {
            CPPDEBUG("net_udpNC_handler::process_input Message ToAddr was not meant for me..." << std::endl);
            //Reject in some way - probably return
        }

        //Four cases - 1. We are recovering a lost packet: new pktid > current pktid
//        if (input_message.getOrdering () > recvd_ordering[input_message.getFirstFromId ()])
//        {
//            //Do nothing, this is correct, record the usage
//        }
//        // 2. We are receiving the last packet of the generation
//        else if (recvd_ordering[input_message.getFirstFromId ()] == MAX_GEN_NUM)
//        {
//            //Reset packet pool of decoder
//            CPPDEBUG("net_udpNC_handler::handle_input At MAX_GEN_NUM: Clearing packet pool" <<std::endl);
//            mchandler.clearPacketPool ();
//            recvd_ordering[input_message.getFirstFromId ()] = 0;
//        }
//        // 3. We have lost a couple of packets, and then overflow happened, meaning we should still reset
//        // the packet pool, we just missed the MAX_GEN_NUM packet
//        // NOTE: ASSUMING that the maximum number of consequtive packets lost == 10
//        else if ((recvd_ordering[input_message.getFirstFromId ()] - input_message.getOrdering ()) < -245)
//        {
//            CPPDEBUG("net_udpNC_handler::handle_input: Roll over happened after couple of lost packets" << std::endl);
//            mchandler.clearPacketPool ();
//            recvd_ordering[input_message.getFirstFromId ()] = 0;

//            std::abort ();
//        }

//        // 4. We have decoded a packet too late, and the ordering has moved on, discard
//        else if (recvd_ordering[input_message.getFirstFromId ()] > input_message.getOrdering ())
//        {
////            CPPDEBUG("net_udpNC_handler::handle_input: Decoded a packet too late, discarding" << std::endl);
//            return;
//        }

        //Save latest packet number
        recvd_ordering[input_message.getFirstFromId ()] = input_message.getOrdering ();


        net_udp_handler::process_input(input_message.getMessage (), input_message.getMessageSize ());

    }

    void net_udpNC_handler::RLNC_msg_received(RLNCMessage msg)
    {
        decoded_from_mchandler++;
//        CPPDEBUG("net_udpNC_handler::RLNC_msg_received Decoded from mc_handler: " << decoded_from_mchandler << std::endl);
//        CPPDEBUG("net_udpNC_handler::RLNC_msg_received first from_id: " << msg.getFirstFromId ()<< std::endl);

//        process_input (msg);
    }

    net_udpNC_handler::~net_udpNC_handler()
    {
        CPPDEBUG("~net_udpNC_handler: decoded_from_mchandler: " << std::endl);
        CPPDEBUG("~net_udpNC_handler: used_from_mchandler: " << std::endl);
    }

}
