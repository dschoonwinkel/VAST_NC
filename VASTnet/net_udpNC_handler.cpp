#include "net_udpNC_handler.h"
#include "net_udp.h"

namespace Vast
{
    net_udpNC_handler::net_udpNC_handler(ip::udp::endpoint local_endpoint):
        net_udp_handler(local_endpoint), mchandler(local_endpoint)
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

        IPaddr to_addr(remote_endpoint.address().to_v4().to_ulong(), remote_endpoint.port ());

        //Put / get and increase ordering number of this id_t
        if (send_ordering.find(net_manager::resolveHostID (&to_addr)) == send_ordering.end ())
        {
            send_ordering[net_manager::resolveHostID (&to_addr)] = 0;
        }
        uint8_t ordering = send_ordering[net_manager::resolveHostID (&to_addr)];
        send_ordering[net_manager::resolveHostID (&to_addr)]++;

        id_t myID = -1;
        if (_msghandler->getReal_net_udp ())
        {
            myID = _msghandler->getReal_net_udp ()->getID ();
        }
        else
        {
            CPPDEBUG("net_udpNC_handler::send _msghandler->getReal_net_udp was NULL" << std::endl);
        }

        message.putPacketId(RLNCMessage::generatePacketId (myID, pkt_gen_count++));
        message.putOrdering (ordering);
        message.putFromId (myID);
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
        total_packets_processed++;
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
                    mchandler.putOtherRLNCMessage (input_message);
                    ip::udp::endpoint* remote_endptr = &_remote_endpoint_;
                    process_input (input_message, remote_endptr);



                }


            //Restart waiting for new packets
            start_receive();
        }
        else {
            CPPDEBUG("Error on UDP socket receive: " << error.message() << std::endl;);
        }
        return -1;
    }

    void net_udpNC_handler::process_input (RLNCMessage input_message, ip::udp::endpoint* remote_endptr)
    {

        if (input_message.getPacketIds ().size() > 1)
            throw std::logic_error("net_udpNC_handler::process_input: Should not have encoded packets here\n");

        filter_input (input_message, remote_endptr);
    }

    void net_udpNC_handler::filter_input (RLNCMessage input_message, ip::udp::endpoint* remote_endptr)
    {
        if (input_message.getToAddrs ().size () > 0)
        {
            if (_msghandler->getPublicIPaddr () == input_message.getToAddrs ()[0])
            {
                //All is well, continue
                //CPPDEBUG("net_udpNC_handler::process_input Message ToAddr was meant for me..." << std::endl);
                order_input(input_message, remote_endptr);
                return;
            }
            else
            {
                CPPDEBUG("net_udpNC_handler::process_input Message ToAddr was not meant for me..." << std::endl);
                //Reject in some way - probably return
                return;
            }
        }
        else
        {
            CPPDEBUG("net_udpNC_handler::process_input input_message has no toAddrs" << std::endl);
            return;
        }
    }

    void net_udpNC_handler::order_input(RLNCMessage input_message, ip::udp::endpoint* remote_endptr)
    {
        id_t firstFromID = input_message.getFirstFromId ();

        if (firstFromID == NET_ID_UNASSIGNED)
        {
//            CPPDEBUG("net_udpNC_handler::process_input NET_ID_UNASSIGNED in fromID, processing" << std::endl);
        }
        //If we have never heard from this ID before, simply store the recvd_ordering
        else if (recvd_ordering.find(firstFromID) == recvd_ordering.end())
        {
            recvd_ordering[firstFromID] = input_message.getOrdering ();
            CPPDEBUG("net_udpNC_handler::process_input Adding new from_id to recvd_ordering" << std::endl);
        }
        //First message of new chain, reset ordering to accept
        else if (recvd_ordering[firstFromID] >= LOWEST_RESET_PACKET_ORDERING_NUMBER
                 && input_message.getOrdering() < HIGHEST_RESET_ACCEPTING_ORDERING_NUMBER)
        {
            CPPDEBUG("net_udpNC_handler::process_input Resetting chain, ordering = 0" << std::endl);
            recvd_ordering[firstFromID] = 0;
            mchandler.clearPacketPool ();
        }
        //This is an old or the same message, ignore
        else if (recvd_ordering[firstFromID] >= input_message.getOrdering ())
        {
            CPPDEBUG("net_udpNC_handler::process_input Old or same message, ignoring" << std::endl);
            CPPDEBUG("net_udpNC_handler::process_input " << firstFromID << " " << (size_t)recvd_ordering[firstFromID] << " " << (size_t)input_message.getOrdering() << std::endl);
            return;
        }

        //Save latest packet number
        recvd_ordering[input_message.getFirstFromId ()] = input_message.getOrdering ();

        handoff_input (input_message, remote_endptr);
    }


    void net_udpNC_handler::handoff_input (RLNCMessage input_message, ip::udp::endpoint *remote_endptr)
    {
        net_udp_handler::process_input(input_message.getMessage (), input_message.getMessageSize (), remote_endptr);
    }

    void net_udpNC_handler::RLNC_msg_received(RLNCMessage msg)
    {
        decoded_from_mchandler++;
//        CPPDEBUG("net_udpNC_handler::RLNC_msg_received Decoded from mc_handler: " << decoded_from_mchandler << std::endl);
//        CPPDEBUG("net_udpNC_handler::RLNC_msg_received first from_id: " << msg.getFirstFromId ()<< std::endl);

        //MC message are not associated with a specific endpoint
//        process_input (msg, NULL);
    }

    net_udpNC_handler::~net_udpNC_handler()
    {
        CPPDEBUG("~net_udpNC_handler: decoded_from_mchandler: " << decoded_from_mchandler << std::endl);
        CPPDEBUG("~net_udpNC_handler: used_from_mchandler: " << total_packets_processed - decoded_from_mchandler <<  std::endl);
        mchandler.close();
    }

}
