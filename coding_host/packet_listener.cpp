#include "packet_listener.h"
#include <iostream>
#include "udpnc_packet_factory.h"
#include "VASTnet.h"
#include "logger.h"

packet_listener::packet_listener (ip::udp::endpoint local_endpoint):
    _MC_address(ip::address::from_string("239.255.0.1"), 1037)
{
    _recv_udp = NULL;
    _recv_io_service = NULL;
    _iosthread_recv = NULL;
    _local_endpoint = local_endpoint;

}



int packet_listener::open (io_service *io_service, abstract_sender *sender)
{
    _recv_io_service = io_service;
    _send_io_service = new boost::asio::io_service();
    //Required to keep the send_io_service running
    _send_io_service_work = new boost::asio::io_service::work(*_send_io_service);

    //Used to dispatch message
    _sender = sender;

    //Open the UDP socket for listening
    if (_recv_udp == NULL) {
        _recv_udp = new ip::udp::socket(*_recv_io_service);
        _recv_udp->open(ip::udp::v4());
        _recv_udp->set_option(ip::udp::socket::reuse_address(true));

        boost::system::error_code ec;
        _recv_udp->bind(_local_endpoint, ec);

        std::cout << "packet_listener::open " + ec.message() << std::endl;

        if (ec)
        {
            std::cout << "packet_listener:: open unicast address failed" << ec.message() << std::endl;
        }
        //Add async receive to io_service queue
        start_receive();

        //Start the thread handling async receives
        _iosthread_recv = new boost::thread(boost::bind(&boost::asio::io_service::run, _recv_io_service));
    }

    //Open the UDP socket for sending
    if (_send_udp == NULL) {
        _send_udp = new ip::udp::socket(*_send_io_service);
        _send_udp->open(ip::udp::v4());
        _send_udp->set_option(ip::udp::socket::reuse_address(true));
        _send_udp->set_option(ip::multicast::join_group(_MC_address.address ()));

        boost::system::error_code ec;
        _send_udp->bind(_MC_address, ec);

        std::cout << "packet_listener::open MC address" + ec.message() << std::endl;

        if (ec)
        {
            std::cout << "packet_listener:: open MC address failed" << ec.message() << std::endl;
        }

        //Start the thread handling async receives
        _iosthread_send_ioservice = new boost::thread(boost::bind(&boost::asio::io_service::run, _send_io_service));
        _iosthread_send_startsend = new boost::thread(boost::bind(&packet_listener::start_send, this));
    }

    return 0;
}

//Start the receiving loop
void packet_listener::start_receive ()
{
    _recv_udp->async_receive_from(
        boost::asio::buffer(_buf, BUFSIZ), _remote_endpoint_,
        boost::bind(&packet_listener::handle_input, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}

// handling incoming message
int packet_listener::handle_input (const boost::system::error_code& error,
                  std::size_t bytes_transferred)
{
    recv_msgs_count++;

    if (!error)
    {
        process_input(_buf, bytes_transferred);

        //Restart waiting for new packets
        start_receive();
    }
    else {
        std::cerr << "Error on UDP socket receive: " << error.message() << std::endl;
    }
    return -1;
}

void packet_listener::process_input (const char *buf,
                                     std::size_t bytes_transferred)
{
    process_msg_count++;
    UDPNCHeader header;

    memcpy(&header, buf, sizeof(UDPNCHeader));

    //Check if it is really a VAST message: Start and end bytes of header should be correct
    if (!UDPNCHeader_factory::isUDPNCHeader(header))
    {
        Logger::debugPeriodic("packet_listener::handle_input Non-UDPNC message received on UDP socket", 1000, 1e6);
        return;
    }
    else if (header.enc_packet_count > 1)
    {
        CPPDEBUG("packet_listener::handle_input Already encoded packet received on UDP socket, ignoring" << std::endl);
        return;
    }
    else {
//          CPPDEBUG("UDPNC message received on the coding host" << std::endl);
        UDPNCMessage message;

        message.deserialize(buf, bytes_transferred);

        //Do not add NET_ID_UNASSIGNED packets to coding packets
        if (message.getFirstFromId () == NET_ID_UNASSIGNED)
            return;

        //Do not code empty UDPNCMessage packets, used as synchronise packets, only unicast
        if (message.getMessageSize() == 0)
        {
//            CPPDEBUG("packet_listener::process_input Empty UDPNC Keep alive packet found " << std::endl);
            return;
        }

        recvfrommap_count[message.getFirstFromId()]++;

        recoder.addUDPNCMessage(message);

        msgs_mutex.lock();
        std::shared_ptr<UDPNCMessage> temp_msg = recoder.produceUDPNCMessage();


        if (temp_msg)
        {
            msgs.push_back(UDPNCMessage(*temp_msg));
//            CPPDEBUG("packet_listener::handle_input: Adding UDPNCmessage to queue" << std::endl);
        }

        msgs_mutex.unlock();
    }
    return;
}

void packet_listener::start_send()
{
    

    if (_sender == NULL)
    {
        _sender = this;
    }

    CPPDEBUG("packet_listener::start_sending Starting" << std::endl);

    while(running)
    {
        if (msgs.size() < 1)
            continue;

//        CPPDEBUG("packet_listener::start_send Msgs size: " << msgs.size () << std::endl);
        msgs_mutex.lock();


        //There is too much of a backlog, discard packets
        if (msgs.size () > PACKET_DEQUE_TOO_LONG)
        {
            msgs.clear ();
            _send_udp->cancel ();
            msgs_mutex.unlock();
            total_msgstoolong_cleared++;
            continue;
        }

        UDPNCMessage message(msgs.front());
        msgs.pop_front();
        msgs_mutex.unlock();

        size_t sendlen = message.serialize(_sendbuf);
//        _sender->send(_sendbuf, sendlen,
//                      ip::udp::endpoint(ip::address::from_string("239.255.0.1"), 1037));


        //Send to each recipient seperately
        _sender->send(_sendbuf, sendlen,
                      ip::udp::endpoint(ip::address_v4(message.getToAddrs()[0].host), message.getToAddrs()[0].port));
        _sender->send(_sendbuf, sendlen,
                      ip::udp::endpoint(ip::address_v4(message.getToAddrs()[1].host), message.getToAddrs()[0].port));
        total_coded_msgs_sent+= 2;

    }

}

size_t packet_listener::send (const char *buf, size_t n, ip::udp::endpoint remote_endpoint)
{
    if (_send_udp == NULL)
    {
        std::cerr << "packet_listener::send trying to send before socket is ready" << std::endl;
        return -1;
    }



//    CPPDEBUG("packet_listener::send size of sent packet: " << n << std::endl);
    _send_udp->async_send_to(buffer(buf, n), remote_endpoint,
                               boost::bind(&packet_listener::handle_send_to, this,
                               boost::asio::placeholders::error,
                               boost::asio::placeholders::bytes_transferred));

    return n;

}

void packet_listener::handle_send_to(const boost::system::error_code& errcode, std::size_t)
{

    if (!errcode)
    {
        total_coded_msgs_sent++;

        if (total_coded_msgs_sent % 100 == 0)
        {
          Logger::debugPeriodic(std::to_string(total_coded_msgs_sent) + " packets sent async", 1000, 1e6);
        }
    }
    else if (errcode == boost::asio::error::operation_aborted)
    {
        total_resets++;
    }
}

// close connection & unregister from io_service
int packet_listener::close (void)
{
    std::cout << "packet_listener::close()" << std::endl;
    return this->handle_close();
}

int packet_listener::handle_close ()
{
    if (_recv_io_service != NULL) {
        if (_recv_udp != NULL && _recv_udp->is_open())
        {
            _recv_udp->close();
            _recv_io_service->stop();
        }


        if (_iosthread_recv)
        {
            std::cout << "Waiting for _iosthread_recv" << std::endl;
            _iosthread_recv->join();
        }
    }

    running = false;

    if (_send_io_service != NULL)
    {
        if (_send_udp != NULL && _send_udp->is_open ())
        {
            _send_udp->close ();

            //Work needs to be destroyed before _io_service can properly exit
            delete _send_io_service_work;

            _send_io_service->stop();
        }

        if (_iosthread_send_ioservice)
        {
            std::cout << "Waiting for _iosthread_send_ioservice" << std::endl;
            _iosthread_send_ioservice->join();
        }
    }

    if (_iosthread_send_startsend)
    {
        std::cout << "Waiting for _iosthread_send_startsend" << std::endl;
        _iosthread_send_startsend->join ();
    }


    return 0;
}

packet_listener::~packet_listener ()
{
    if (_recv_udp)
    {
        delete _recv_udp;
        _recv_udp = NULL;
    }
    if (_send_udp)
    {
        delete _send_udp;
        _send_udp = NULL;
    }

    if (_iosthread_recv)
    {
        delete _iosthread_recv;
        _iosthread_recv = NULL;
    }

    if (_send_io_service)
    {
        delete _send_io_service;
        _send_io_service = NULL;
    }

    if (_iosthread_send_ioservice)
    {
        delete _iosthread_send_ioservice;
        _iosthread_send_ioservice = NULL;
    }

    if (_iosthread_send_startsend)
    {
        delete _iosthread_send_startsend;
        _iosthread_send_startsend = NULL;
    }

    std::cout << std::endl << "~packet_listener: Recv packet count: " << getRecvMsgsCount () << std::endl;
    std::cout << "~packet_listener: process_msg_count: " << process_msg_count << std::endl;
    std::cout << "~packet_listener: total_coded_msgs_sent: " << total_coded_msgs_sent << std::endl;
    std::cout << "~packet_listener: Total resets: " << total_resets << std::endl << std::endl;
    std::cout << "~packet_listener: Total msgs deque too long: " << total_msgstoolong_cleared << std::endl << std::endl;

    for (auto it = recvfrommap_count.begin(); it != recvfrommap_count.end(); it++)
    {
        std::cout << "~packet_listener: " << it->first << ":" << it->second << std::endl;
    }

}


size_t packet_listener::getRecvMsgsCount ()
{
    return recv_msgs_count;
}
