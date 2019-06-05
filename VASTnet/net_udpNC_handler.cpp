#include "net_udpNC_handler.h"
#include "net_udp.h"
#include "netudpnc_capturemsgs.h"
#include "pthread.h"

namespace Vast
{
    net_udpNC_handler::net_udpNC_handler(ip::udp::endpoint local_endpoint):
        net_udp_handler(local_endpoint), mchandler(local_endpoint)
    {
        CPPDEBUG("Starting net_udpNC_handler" << std::endl);

    }

    int net_udpNC_handler::open (io_service *io_service, abstract_net_udp *msghandler,
                                 AbstractRLNCMsgReceiver *RLNCsink, bool startthread)
    {
        if (RLNCsink == NULL)
            RLNCsink = this;

        CPPDEBUG("net_udpNC_handler::open" << std::endl);
        _timeout_keepalive = msghandler->getTimestamp();
        net_udp_handler::open(io_service, msghandler, startthread);
        mchandler.open (&consumer, startthread);
        consumer.open (RLNCsink, msghandler, &mchandler, startthread);

        return 0;
    }

    size_t net_udpNC_handler::send(const char *msg, size_t n, ip::udp::endpoint remote_endpoint)
    {
        if (_msghandler->getTimestamp() > _timeout_keepalive + _KEEPALIVE_UDPNC_TIMEOUT_)
        {
            //Send keep alive packet to syncronise receiver
            send_helper(NULL, n, remote_endpoint);
            _timeout_keepalive = _msghandler->getTimestamp();
        }

        return send_helper(msg, n, remote_endpoint);
    }

    size_t net_udpNC_handler::send_helper(const char *msg, size_t n, ip::udp::endpoint remote_endpoint)
    {
        if (_udp == NULL)
        {
            std::cerr << "net_udpNC_handler::send_helper trying to send before socket is ready" << std::endl;
            return -1;
        }

        RLNCHeader_factory header_factory(GENSIZE, generation);
        RLNCMessage message(header_factory.build());

        if (n != 0 && msg != NULL)
        {
            message.putMessage(msg, n);
        }
        else
        {
            CPPDEBUG("net_udpNC_handler::send_helper Sending keepalive packet" << std::endl);
            if (message.getMessageSize() != 0)
                throw std::logic_error("net_udpNC_handler::send_helper keepalive packet should have 0 length");
        }

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

#ifdef NETUDPNC_CAPTUREMSG
        netudpnc_capturemsgs::saveNCMessage(_msghandler->getTimestamp(),
                                            std::string(buf.data(), sending_len),
                                            remote_endpoint,
                                            _local_endpoint,
                                            net_manager::resolveHostID(&_msghandler->getReal_net_udp()->getAddress().publicIP));
#endif

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
        total_packets_recvd++;

        if (!error)
        {
            //Store UDP messages
    //        CPPDEBUG("net_udpNC_handler::handle_input Received a message!" << std::endl);
            process_input(_buf, _remote_endpoint_, bytes_transferred);
            //Restart waiting for new packets
            pthread_setname_np(pthread_self(), "net_udpNC_handler:receive_thread");
            start_receive();
        }
        else {
            CPPDEBUG("Error on UDP socket receive: " << error.message() << std::endl);
        }
        return -1;
    }

    void net_udpNC_handler::process_input(const char *buf, ip::udp::endpoint remote_endpoint, std::size_t bytes_transferred)
    {
        RLNCHeader header;

        memcpy(&header, buf, sizeof(RLNCHeader));

        //Check if it is really a VAST message: Start and end bytes of header should be correct
        if (!RLNCHeader_factory::isRLNCHeader(header))
        {
//                    CPPDEBUG("net_udp_handler::handle_input Non-RLNC message received on UDP socket" << std::endl);
        }
        else if (RLNCHeader_factory::isRLNCHeader (header) && header.enc_packet_count > 1)
        {
//            throw std::logic_error("net_udpNC_handler::handle_input: Encoded packet received in unicast handler\n");
            CPPDEBUG("net_udpNC_handler::handle_input: Encoded packet received in unicast handler\n");
            process_encoded(buf, bytes_transferred);

        }
        else {
//                    CPPDEBUG("net_udpNC_handler::handle_input RLNC message received" << std::endl);

            RLNCMessage input_message;
            input_message.deserialize (buf, bytes_transferred);
            //Add uncoded messages to the packet pool to aid in decoding later
            mchandler.putOtherRLNCMessage (input_message);
            IPaddr remote_addr(remote_endpoint.address().to_v4().to_ulong(), remote_endpoint.port());
//                CPPDEBUG("net_udpNC_handler handle_input: IPaddr " << remote_addr << std::endl);
            consumer.RLNC_msg_received (input_message, remote_addr);

        }
    }

    /**
     * @brief Send the encoded packet to net_udpNC_mchandler for processing
     * @param buf
     * @param bytes_transferred
     */
    void net_udpNC_handler::process_encoded(const char *buf, std::size_t bytes_transferred)
    {
        mchandler.process_encoded(buf, bytes_transferred);
    }


    void net_udpNC_handler::handoff_input (RLNCMessage input_message, IPaddr socket_addr)
    {
        id_t fromhost = input_message.getFirstFromId();
        net_udp_handler::process_input(input_message.getMessage (), input_message.getMessageSize (), socket_addr, fromhost);
    }

    void net_udpNC_handler::RLNC_msg_received(RLNCMessage input_message, IPaddr socket_addr)
    {
//        CPPDEBUG("net_udpNC_handler::RLNC_msg_received Decoded from mc_handler: " << decoded_from_mchandler << std::endl);
//        CPPDEBUG("net_udpNC_handler::RLNC_msg_received first from_id: " << msg.getFirstFromId ()<< std::endl);

        //MC message are not associated with a specific endpoint
        handoff_input (input_message, socket_addr);
    }

    int net_udpNC_handler::close ()
    {
        CPPDEBUG("net_udpNC_handler::close()" << std::endl);
        net_udp_handler::close();
        consumer.close();
        mchandler.close();

        return 0;
    }

    net_udpNC_handler::~net_udpNC_handler()
    {
        if (_udp != NULL)
            close();

        CPPDEBUG("~net_udpNC_handler: total_packets_recvd: " << total_packets_recvd << std::endl);
    }

}
