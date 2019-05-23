#include "packet_listener.h"
#include <iostream>
#include "rlnc_packet_factory.h"
#include "VASTnet.h"

packet_listener::packet_listener (ip::udp::endpoint local_endpoint):
    _MC_address(ip::address::from_string("239.255.0.1"), 1037)
{
    _recv_udp = NULL;
    _recv_io_service = NULL;
    _iosthread_recv = NULL;
    _local_endpoint = local_endpoint;

}



int packet_listener::open (io_service *io_service)
{
    _recv_io_service = io_service;
    _send_io_service = new boost::asio::io_service();
    //Required to keep the send_io_service running
    _send_io_service_work = new boost::asio::io_service::work(*_send_io_service);

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
        _iosthread_send_ioservice = new boost::thread(boost::bind(&packet_listener::start_send, this));
    }

    return 0;
}

// close connection & unregister from io_service
int packet_listener::close (void)
{
    return this->handle_close();
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
        process_input(bytes_transferred);

        //Restart waiting for new packets
        start_receive();
    }
    else {
        std::cerr << "Error on UDP socket receive: " << error.message() << std::endl;
    }
    return -1;
}

int packet_listener::process_input (std::size_t bytes_transferred)
{
    RLNCHeader header;
    char *p = _buf;

    memcpy(&header, p, sizeof(RLNCHeader));

        //Check if it is really a VAST message: Start and end bytes of header should be correct
        if (!RLNCHeader_factory::isRLNCHeader(header))
        {
            CPPDEBUG("packet_listener::handle_input Non-RLNC message received on UDP socket" << std::endl);
            return 0;
        }
        else if (header.enc_packet_count > 1)
        {
            CPPDEBUG("packet_listener::handle_input Already encoded packet received on UDP socket, ignoring" << std::endl);
            return 0;
        }
        else {
//          CPPDEBUG("RLNC message received on the coding host" << std::endl);
            RLNCMessage message;

            message.deserialize(_buf, bytes_transferred);

            //Do not add NET_ID_UNASSIGNED packets to coding packets
            if (message.getFirstFromId () == NET_ID_UNASSIGNED)
                return 0;

            //Do not code empty RLNCMessage packets, used as synchronise packets, only unicast
            if (message.getMessageSize() == 0)
            {
                CPPDEBUG("packet_listener::process_input Empty RLNC Keep alive packet found " << std::endl);
                return 0;
            }

            recoder.addRLNCMessage(message);

            msgs_mutex.lock();
            RLNCMessage *temp_msg = recoder.produceRLNCMessage();


            if (temp_msg != NULL)
            {
                msgs.push_back(RLNCMessage(*temp_msg));
//                    CPPDEBUG("packet_listener::handle_input: Adding RLNCmessage to queue" << std::endl);
                delete temp_msg;
            }

            msgs_mutex.unlock();
        }
        return 1;
}
int packet_listener::handle_close ()
{
    if (_recv_io_service != NULL) {
        if (_recv_udp != NULL && _recv_udp->is_open())
        {
            _recv_udp->close();
        }

        _recv_io_service->stop();
        std::cout << "Waiting for _iosthread_recv" << std::endl;
        _iosthread_recv->join();
    }

    running = false;

    if (_send_io_service != NULL)
    {
        if (_send_udp != NULL && _send_udp->is_open ())
        {
            _send_udp->close ();
        }
        _send_io_service->stop();
        std::cout << "Waiting for _iosthread_send_ioservice" << std::endl;
        _iosthread_send_ioservice->join();
    }

    std::cout << "Waiting for _iosthread_send_startsend" << std::endl;
    _iosthread_send_startsend->join ();


    return 0;
}

void packet_listener::start_send()
{

    std::cout << "packet_listener::start_sending Starting" << std::endl;

    while(running)
    {
        if (msgs.size() < 1)
            continue;

//        std::cout << "packet_listener::start_send Msgs size: " << msgs.size () << std::endl;
        msgs_mutex.lock();


        //There is too much of a backlog, discard packets
        if (msgs.size () > PACKET_DEQUE_TOO_LONG)
        {
            msgs.clear ();
            _send_udp->cancel ();
            msgs_mutex.unlock();
            continue;
        }

        RLNCMessage message(msgs.front());
        msgs.pop_front();
        msgs_mutex.unlock();

        size_t sendlen = message.serialize(_sendbuf);
        send(_sendbuf, sendlen, ip::udp::endpoint(ip::address::from_string("239.255.0.1"), 1037));

    }

}

size_t packet_listener::send (const char *buf, size_t n, ip::udp::endpoint remote_endpoint)
{
    if (_recv_udp == NULL)
    {
        std::cerr << "packet_listener::send trying to send before socket is ready" << std::endl;
        return -1;
    }



//    CPPDEBUG("packet_listener::send size of sent packet: " << n << std::endl);
    _recv_udp->async_send_to(buffer(buf, n), remote_endpoint,
                               boost::bind(&packet_listener::handle_send_to, this,
                               boost::asio::placeholders::error,
                               boost::asio::placeholders::bytes_transferred));

    return n;

}

void packet_listener::handle_send_to(const boost::system::error_code& errcode, std::size_t)
{

    if (!errcode)
    {
        coded_msgs_sent++;
        total_coded_msgs_sent++;

        if (total_coded_msgs_sent % 100 == 0)
        {
          CPPDEBUG(total_coded_msgs_sent << " packets sent async " << std::endl);
        }
    }
    else if (errcode == boost::asio::error::operation_aborted)
    {
        total_resets++;
        std::cout << "Total resets: " << total_resets << std::endl;
    }

//        coded_msgs_sent = 0;
    //CPPDEBUG(total_coded_msgs_sent << " packets sent async, " << coded_msgs_sent << " this timeslot" << std::endl);
}

packet_listener::~packet_listener ()
{
    delete _recv_udp;
    _recv_udp = NULL;
    delete _send_udp;
    _send_udp = NULL;

    delete _send_io_service;
    _send_io_service = NULL;

    std::cout << "Recv packet count: " << getRecvMsgsCount () << std::endl;
    std::cout << "Total resets: " << total_resets << std::endl;

}

size_t packet_listener::getRecvMsgsCount ()
{
    return recv_msgs_count;
}
