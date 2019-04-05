#include "net_udpnc_mchandler.h"
#include "net_udp.h"
#include "rlncmessage.h"
#include "VASTnet.h"


namespace Vast
{
    net_udpNC_MChandler::net_udpNC_MChandler(ip::udp::endpoint local_endpoint) :
        MC_address(ip::address::from_string("239.255.0.1"), 1037)
    {
        _io_service = new io_service();
        _local_endpoint = local_endpoint;
    }

    net_udpNC_MChandler::~net_udpNC_MChandler()
    {
        delete _udp;
        _udp = NULL;
        delete _io_service;
        _io_service = NULL;

        CPPDEBUG("~net_udpNC_MChandler packets_recvd: " << packets_received << std::endl);
        CPPDEBUG("~net_udpNC_MChandler toaddrs_pkts_ignored: " << toaddrs_pkts_ignored << std::endl);
    }

    int net_udpNC_MChandler::open(AbstractRLNCMsgReceiver *msghandler) {
        _msghandler = msghandler;

        if (_udp == NULL) {
            _udp = new ip::udp::socket(*_io_service);
            _udp->open(ip::udp::v4());
            _udp->set_option(ip::udp::socket::reuse_address(true));
            _udp->set_option(ip::multicast::join_group(MC_address.address ()));

            boost::system::error_code ec;
            uint16_t port = _local_endpoint.port();

            MC_address.port(port);
            _udp->bind(MC_address, ec);

            std::cout << "net_udpnc_mchandler::open " + ec.message() << std::endl;

            if (ec)
            {
                std::cout << "net_udpnc_mchandler:: open MC address failed" << ec.message() << std::endl;
            }

            //Add async receive to io_service queue
            start_receive();

            std::cout << "net_udpnc_mchandler::open _udp->_local_endpoint: " << _udp->local_endpoint() << " _local_endpoint" << _local_endpoint << std::endl;

            //Start the thread handling async receives
            _iosthread = new boost::thread(boost::bind(&boost::asio::io_service::run, _io_service));
        }

        return 0;
    }

    void net_udpNC_MChandler::start_receive()
    {
        _udp->async_receive_from(
            boost::asio::buffer(_buf, VAST_BUFSIZ), _remote_endpoint_,
            boost::bind(&net_udpNC_MChandler::handle_input, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }

    // handling incoming message
    int net_udpNC_MChandler::handle_input (const boost::system::error_code& error,
          std::size_t bytes_transferred)
    {

        RLNCHeader header;

        if (!error)
        {
            //Store UDP messages
            packets_received++;
//            CPPDEBUG("net_udpnc_mchandler::handle_input Received " << packets_received << " messages!" << std::endl);

            char *p = _buf;

            memcpy(&header, p, sizeof(RLNCHeader));

                //Check if it is really a VAST message: Start and end bytes of header should be correct
                if (!RLNCHeader_factory::isRLNCHeader(header))
                {
                        CPPDEBUG("net_udp_handler::handle_input Non-RLNC message received on UDP socket" << std::endl);
                }
                else if (RLNCHeader_factory::isRLNCHeader (header) && header.enc_packet_count > 1)
                {
//                    CPPDEBUG("net_udpnc_mchandler::handle_input: Encoded packet received" << std::endl);
                    process_encoded (bytes_transferred);
                }
                else {
                    CPPDEBUG("net_udpnc_mchandler::handle_input RLNC message received" << std::endl);
                    RLNCMessage other;
                    other.deserialize (p, bytes_transferred);
                    putOtherRLNCMessage (other);
                }

            //Restart waiting for new packets
            start_receive();
        }
        else {
            CPPDEBUG("Error on UDP socket receive: " << error.message() << std::endl;);
        }
        return -1;
    }

    //Used for testing purposes
    int net_udpNC_MChandler::handle_buffer (char *buf, std::size_t bytes_transferred)
    {
        memcpy(_buf, buf, bytes_transferred);

        boost::system::error_code errcode;
        return handle_input(errcode, bytes_transferred);
    }

    void net_udpNC_MChandler::process_encoded (std::size_t bytes_transferred)
    {
//            CPPDEBUG("net_udpNC_MChandler::process_encoded: processing message" << std::endl);
            RLNCMessage message1;
            message1.deserialize (_buf, bytes_transferred);
            if (toAddrForMe (message1))
            {
                putOtherRLNCMessage (message1);
                RLNCMessage *decoded_msg = decoder.produceDecodedRLNCMessage();


                if (decoded_msg != NULL)
                {
                    if (_msghandler != NULL)
                    {
    //                    CPPDEBUG("net_udpNC_MChandler::process_encoded processing decoded message" << std::endl);
                        _msghandler->RLNC_msg_received(*decoded_msg);
                        delete decoded_msg;
                    }
                    else {
                        std::cerr << "net_udpNC_MChandler::process_encoded: _msghandler was NULL" << std::endl;
                    }
                }
            }
            else
            {
//                CPPDEBUG("None of the toAddrs was for me" << std::endl);
                toaddrs_pkts_ignored++;

            }

    }

    void net_udpNC_MChandler::process_message(char *buf, std::size_t bytes_transferred)
    {
        //Process UDP messages
        size_t n = bytes_transferred;
        VASTHeader header;
        id_t remote_id;

        char *p = buf;

        //NOTE: there may be several valid UDP messages received at once -- is this really necessary?
        while (n > sizeof (VASTHeader))
        {
            //extract message header
            mempcpy (&header, p, sizeof (VASTHeader));
            n -= sizeof(VASTHeader);
            p += sizeof(VASTHeader);

            //Check if it is really a VAST message: Start and end bytes of header should be correct
            if (!(header.start == 10 && header.end == 5))
            {
                CPPDEBUG("net_udp_handler::handle_input Non-VAST message received on UDP socket" << std::endl);
                return;
            }

            Message *msg = new Message(0);
            if (0 == msg->deserialize (p, header.msg_size))
            {
                printf("net_udp_handler::handle_input deserialize message fail: size = %u\n", header.msg_size);
            }
            remote_id = msg->from;

            //Break up messages into VASTMessage sizes
            //msg start at p - 4, i.e. start of header
            //msgsize = header.msg_size + 4 for header

            if (remote_id != NET_ID_UNASSIGNED) //Only allow encoded packets from hosts I have heard from before
            {
                ((net_udp*)_msghandler)->msg_received(remote_id, p - sizeof(VASTHeader), header.msg_size + sizeof(VASTHeader));
            }

            //Next message
            p += header.msg_size;
            n -= header.msg_size;
        }
    }

    void net_udpNC_MChandler::putOtherRLNCMessage(RLNCMessage other)
    {
        decoder.addRLNCMessage (other);
    }

    void net_udpNC_MChandler::clearPacketPool ()
    {
        decoder.clearPacketPool ();
    }

    size_t net_udpNC_MChandler::getPacketPoolSize ()
    {
        return decoder.getPacketPoolSize ();
    }

    int net_udpNC_MChandler::close() {
        CPPDEBUG("net_udpNC_MChandler: close()" << std::endl);
        return this->handle_close();
    }

    int net_udpNC_MChandler::handle_close()
    {
        if (_udp != NULL && _udp->is_open())
        {
            _udp->close();
        }

        _io_service->stop();
        _iosthread->join();

        return 0;
    }

    bool net_udpNC_MChandler::toAddrForMe(RLNCMessage msg)
    {
        IPaddr local_addr(_local_endpoint.address().to_v4().to_ulong(), _local_endpoint.port ());
        for (auto &addr : msg.getToAddrs ())
        {

            //One of the addresses was meant for me
            if (addr == local_addr)
            {
                return true;
            }
        }

        return false;
    }
}
