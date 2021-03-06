#include "net_udpnc_mchandler.h"
#include "net_udp.h"
#include "udpncmessage.h"
#include "VASTnet.h"
#include "vastnetstatlog_entry.h"


namespace Vast
{
    net_udpNC_MChandler::net_udpNC_MChandler(ip::udp::endpoint local_endpoint) :
        MC_address(ip::address::from_string("239.255.0.1"), 1037)
    {
        _io_service = new io_service();
        _local_endpoint = local_endpoint;
    }

    int net_udpNC_MChandler::open(AbstractUDPNCMsgReceiver *consumer, abstract_net_udp *udp_manager,
                                  bool startthread) {
        CPPDEBUG("net_udpNC_MChandler::open" << std::endl);
        _msghandler = consumer;
        _udp_manager = udp_manager;

        std::cout << "net_udpNC_MChandler::open: _local_endpoint: " << _local_endpoint << std::endl;
        IPaddr host_addr(_local_endpoint.address().to_v4().to_ulong(), _local_endpoint.port ());
        id_t HostID = net_manager::resolveHostID(&host_addr);
        pNetStatlog = std::make_unique<VASTNetStatLogEntry>(HostID);

        if (_udpsocket == NULL) {
            _udpsocket = new ip::udp::socket(*_io_service);
            _udpsocket->open(ip::udp::v4());
            _udpsocket->set_option(ip::udp::socket::reuse_address(true));
            _udpsocket->set_option(ip::multicast::join_group(MC_address.address ()));

            boost::system::error_code ec;
            uint16_t port = _local_endpoint.port();

            MC_address.port(port);
            _udpsocket->bind(MC_address, ec);

            std::cout << "net_udpnc_mchandler::open " + ec.message() << std::endl;

            if (ec)
            {
                std::cout << "net_udpnc_mchandler:: open MC address failed" << ec.message() << std::endl;
            }

            //Add async receive to io_service queue
            start_receive();

            std::cout << "net_udpnc_mchandler::open _udp->_local_endpoint: " << _udpsocket->local_endpoint() << " _local_endpoint" << _local_endpoint << std::endl;

            if (startthread)
            {
                //Start the thread handling async receives
                _iosthread = new boost::thread(boost::bind(&boost::asio::io_service::run, _io_service));
            }
        }

        return 0;
    }

    void net_udpNC_MChandler::start_receive()
    {
        _udpsocket->async_receive_from(
            boost::asio::buffer(_buf, VAST_BUFSIZ), _remote_endpoint_,
            boost::bind(&net_udpNC_MChandler::handle_input, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
    }

    // handling incoming message
    int net_udpNC_MChandler::handle_input (const boost::system::error_code& error,
          std::size_t bytes_transferred)
    {
        if (!error)
        {
            //Check if there is another packet waiting
            if (_udpsocket->available() > 0)
                stacked_packets_received++;

            //Store UDP messages
            packets_received++;

            process_input(_buf, bytes_transferred);

            Logger::registerThreadName(std::this_thread::get_id(), "net_udpnc_mchandler");

            //Restart waiting for new packets
            start_receive();
        }
        else {
            CPPDEBUG("Error on UDP socket receive: " << error.message() << std::endl;);
        }
        return -1;
    }

    void net_udpNC_MChandler::process_input(const char *buf, std::size_t bytes_transferred)
    {
        UDPNCHeader header;
        memcpy(&header, buf, sizeof(UDPNCHeader));

            //Check if it is really a VAST message: Start and end bytes of header should be correct
            if (!UDPNCHeader_factory::isUDPNCHeader(header))
            {
                    CPPDEBUG("net_udp_handler::process_input Non-UDPNC message received on UDP socket" << std::endl);
            }
            else if (UDPNCHeader_factory::isUDPNCHeader (header) && header.enc_packet_count > 1)
            {
//                CPPDEBUG("net_udpnc_mchandler::process_input: Encoded packet received" << std::endl);
                process_encoded (buf, bytes_transferred);
            }
            else {
                CPPDEBUG("net_udpnc_mchandler::process_input uncoded UDPNC message received" << std::endl);
//                    throw std::logic_error("net_udpnc_mchandler::process_input Received unencoded packet in MC handler\n");
                UDPNCMessage other;
                other.deserialize (buf, bytes_transferred);
                putOtherUDPNCMessage (other);
                unicastpackets_processed++;
            }
    }

    void net_udpNC_MChandler::process_encoded (const char *buf, std::size_t bytes_transferred)
    {
            UDPNCMessage message1;
            message1.deserialize (buf, bytes_transferred);
            raw_interval_MCrecv_bytes+= bytes_transferred;
            packets_processed++;

            if (toAddrForMe (message1))
            {
                used_interval_MCrecv_bytes+= bytes_transferred;
                putOtherUDPNCMessage (message1);
                std::shared_ptr<UDPNCMessage> decoded_msg = decoder.produceDecodedUDPNCMessage();


                if (decoded_msg != NULL)
                {
                    if (_msghandler != NULL)
                    {
                        //MC loses original source socket addr, pass empty IPaddr
                        _msghandler->UDPNC_msg_received(*decoded_msg, IPaddr());
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

//    void net_udpNC_MChandler::process_message(char *buf, std::size_t bytes_transferred)
//    {
//        //Process UDP messages
//        size_t n = bytes_transferred;
//        VASTHeader header;
//        id_t remote_id;

//        char *p = buf;

//        //NOTE: there may be several valid UDP messages received at once -- is this really necessary?
//        while (n > sizeof (VASTHeader))
//        {
//            //extract message header
//            mempcpy (&header, p, sizeof (VASTHeader));
//            n -= sizeof(VASTHeader);
//            p += sizeof(VASTHeader);

//            //Check if it is really a VAST message: Start and end bytes of header should be correct
//            if (!(header.start == 10 && header.end == 5))
//            {
//                CPPDEBUG("net_udp_handler::handle_input Non-VAST message received on UDP socket" << std::endl);
//                return;
//            }

//            Message *msg = new Message(0);
//            if (0 == msg->deserialize (p, header.msg_size))
//            {
//                printf("net_udp_handler::handle_input deserialize message fail: size = %u\n", header.msg_size);
//            }
//            remote_id = msg->from;

//            //Break up messages into VASTMessage sizes
//            //msg start at p - 4, i.e. start of header
//            //msgsize = header.msg_size + 4 for header

//            if (remote_id != NET_ID_UNASSIGNED) //Only allow encoded packets from hosts I have heard from before
//            {
//                ((net_udp*)_msghandler)->msg_received(remote_id, p - sizeof(VASTHeader), header.msg_size + sizeof(VASTHeader));
//            }

//            //Next message
//            p += header.msg_size;
//            n -= header.msg_size;
//        }
//    }

    void net_udpNC_MChandler::putOtherUDPNCMessage(UDPNCMessage other)
    {
        decoder.addUDPNCMessage (other);
    }

    void net_udpNC_MChandler::clearPacketPool ()
    {
        decoder.clearPacketPool ();
    }

    size_t net_udpNC_MChandler::getPacketPoolSize ()
    {
        return decoder.getPacketPoolSize ();
    }

    bool net_udpNC_MChandler::toAddrForMe(UDPNCMessage msg)
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

    void net_udpNC_MChandler::tick()
    {
        //Record stats
        //Do stats recording here

        raw_MCRecvBytes.addRecord (raw_interval_MCrecv_bytes);
        used_MCRecvBytes.addRecord (used_interval_MCrecv_bytes);
        pNetStatlog->recordMCStat(_udp_manager->getTimestamp(), raw_MCRecvBytes, used_MCRecvBytes);

        raw_interval_MCrecv_bytes = 0;
        used_interval_MCrecv_bytes = 0;

        Logger::debug("net_udpNC_MChandler::tick Packet pool size on "
                     + std::to_string(_udp_manager->getReal_net_udp()->getID()) +
                     ": " + std::to_string(decoder.getPacketPoolSize()), true);
    }

    int net_udpNC_MChandler::close() {
        CPPDEBUG("net_udpNC_MChandler: close()" << std::endl);
        return this->handle_close();
    }

    int net_udpNC_MChandler::handle_close()
    {
        if (_udpsocket != NULL && _udpsocket->is_open())
        {
            _udpsocket->close();
        }

        if (_io_service != NULL)
        {
            _io_service->stop();
        }

        if (_iosthread != NULL)
        {
            _iosthread->join ();
        }

        return 0;
    }

    net_udpNC_MChandler::~net_udpNC_MChandler()
    {
        if (_udpsocket != NULL)
        {
            delete _udpsocket;
            _udpsocket = NULL;
        }
        if (_io_service != NULL)
        {
            delete _io_service;
            _io_service = NULL;
        }

        if (_iosthread != NULL)
        {
            delete _iosthread;
            _iosthread = NULL;
        }

        std::cout << "\n~net_udpNC_MChandler packets_recvd: " << packets_received << std::endl;
        std::cout << "~net_udpNC_MChandler stacked_packets_received: " << stacked_packets_received << std::endl;
        if (packets_received > 0)
        {
            std::cout << "~net_udpNC_MChandler stacked_packets_perc: " << (float)(stacked_packets_received) / packets_received * 100 << std::endl;
        }

        std::cout << "~net_udpNC_MChandler toaddrs_pkts_ignored: " << toaddrs_pkts_ignored << std::endl;
        std::cout << "~net_udpNC_MChandler packets_processed: " << packets_processed << std::endl;
        std::cout << "~net_udpNC_MChandler unicastpackets_processed: " << unicastpackets_processed << std::endl;
    }
}
