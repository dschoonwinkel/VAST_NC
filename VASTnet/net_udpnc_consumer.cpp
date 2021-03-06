#include "net_udpnc_consumer.h"
#include "VASTnet.h"
#include "net_udpnc_mchandler.h"
#include "logger.h"
#include "pthread.h"
#include "net_udp.h"

namespace Vast
{

    net_udpNC_consumer::net_udpNC_consumer()
    {
    }

    void net_udpNC_consumer::open(AbstractUDPNCMsgReceiver *UDPNCsink,
                                  abstract_net_udp *abs_netudp,
                                  net_udpNC_MChandler* mchandler, bool startthread)
    {
        CPPDEBUG("net_udpNC_consumer::open" << std::endl);
        this->udpNC_handler = UDPNCsink;
        this->abs_netudp = abs_netudp;
        this->mchandler = mchandler;

        if (startthread)
        {
            start_consuming();
        }
    }

    void net_udpNC_consumer::UDPNC_msg_received (UDPNCMessage msg, IPaddr socket_addr)
    {
        msg.socket_addr = socket_addr;
        _msg_queue.push (msg);

    }

    void net_udpNC_consumer::tick()
    {
        //Do stats recording here
        Logger::debug("net_udpNC_consumer::tick consumer_queue size on " +
                      std::to_string(abs_netudp->getReal_net_udp()->getID()) +
                      ": " + std::to_string(_msg_queue.size()), true);
    }

    void net_udpNC_consumer::start_consuming()
    {
        CPPDEBUG("net_udpNC_consumer::start_consuming" << std::endl);
        _consumerthread = new boost::thread(boost::bind(&net_udpNC_consumer::consume, this));
    }

    void net_udpNC_consumer::consume()
    {
        if (running)
            CPPDEBUG("net_udpNC_consumer::consume Starting processing message on thread" << std::endl);

        UDPNCMessage msg;

        while(running)
        {

            _msg_queue.pop (msg);

            //Check if we are still running, or just been cancelled
            if (running)
            {
//                CPPDEBUG("net_udpNC_consumer::consume\n" << msg << std::endl);
                process_input(msg, msg.socket_addr);

            }
        }

    }

    void net_udpNC_consumer::process_input (UDPNCMessage input_message, IPaddr socket_addr)
    {

        total_packets_processed++;

        if (input_message.getPacketIds ().size() > 1)
            throw std::logic_error("net_udpNC_consumer::process_input: Should not have encoded packets here\n");
        filter_input (input_message, socket_addr);
    }

    void net_udpNC_consumer::filter_input (UDPNCMessage input_message, IPaddr socket_addr)
    {
        if (input_message.getToAddrs ().size () > 0)
        {
//            CPPDEBUG("abs_netudp->getPublicIPaddr (): " << abs_netudp->getPublicIPaddr () << std::endl);
            if (abs_netudp->getPublicIPaddr () == input_message.getToAddrs ()[0])
            {
                //All is well, continue
                //CPPDEBUG("net_udpNC_consumer::process_input Message ToAddr was meant for me..." << std::endl);
//                CPPDEBUG("input_message.getToAddrs ()[0]: " << input_message.getToAddrs ()[0] << std::endl);
                order_input(input_message, socket_addr);
                return;
            }
            else
            {
//                CPPDEBUG("net_udpNC_consumer::process_input Message ToAddr was not meant for me..." << std::endl);
                //Reject in some way - probably return
                if (!(socket_addr.host == 0 && socket_addr.port == 0))
                {
                    total_not_meantforme_unicast++;
                }
                else
                {
                    total_not_meantforme_multicast++;
                }
                return;
            }
        }
        else
        {
            CPPDEBUG("net_udpNC_consumer::process_input input_message has no toAddrs" << std::endl);
            return;
        }
    }

    void net_udpNC_consumer::order_input(UDPNCMessage input_message, IPaddr socket_addr)
    {
        id_t firstFromID = input_message.getFirstFromId ();

        if (input_message.getMessageSize() == 0)
        {
//            CPPDEBUG("net_udpNC_consumer::process_input Empty UDPNC packet received, using as sync packet" << std::endl);
            recvd_ordering[input_message.getFirstFromId ()] = input_message.getOrdering ();

            //NOTE: why does this clear cause such a big improvement in performance?
            //Effects include - packet pool not large enough to be practical for any decoding
            //Am I simply not using multicasts packet?
//            mchandler->clearPacketPool();
        }

        else if (firstFromID == NET_ID_UNASSIGNED)
        {
//            CPPDEBUG("net_udpNC_consumer::process_input NET_ID_UNASSIGNED in fromID, processing" << std::endl);
        }
        //If we have never heard from this ID before, simply store the recvd_ordering
        else if (recvd_ordering.find(firstFromID) == recvd_ordering.end())
        {
            recvd_ordering[firstFromID] = input_message.getOrdering ();
            CPPDEBUG("net_udpNC_consumer::process_input Adding new from_id to recvd_ordering" << std::endl);
        }
        //First message of new chain, reset ordering to accept
        else if (recvd_ordering[firstFromID] >= LOWEST_RESET_PACKET_ORDERING_NUMBER
                 && input_message.getOrdering() < HIGHEST_RESET_ACCEPTING_ORDERING_NUMBER)
        {
            Logger::debug("net_udpNC_consumer::process_input Resetting chain for " + std::to_string(firstFromID), true);
            recvd_ordering[firstFromID] = 0;
            mchandler->clearPacketPool ();
        }
        //This is an old or the same message, ignore
        else if (recvd_ordering[firstFromID] >= input_message.getOrdering ())
        {
            CPPDEBUG("net_udpNC_consumer::process_input Old or same message, ignoring" << std::endl);
            CPPDEBUG("net_udpNC_consumer::process_input " << firstFromID << " "
                     << (size_t)recvd_ordering[firstFromID]
                     << " " << (size_t)input_message.getOrdering()
                     << std::endl);
            total_toolate_packets++;
            return;
        }

        total_usedpackets++;

        //Save latest packet number
        recvd_ordering[input_message.getFirstFromId ()] = input_message.getOrdering ();

        udpNC_handler->UDPNC_msg_received (input_message, socket_addr);
//        size_t msgqueuesize = _msg_queue.size();
//        if (msgqueuesize > 0)
//            Logger::debugPeriodic("net_udpNC_consumer::order_input Size of _msg_queue, working on reducing it " + std::to_string(msgqueuesize), 100, 10, true);
    }

    void net_udpNC_consumer::close()
    {
        CPPDEBUG("net_udpNC_consumer::close()" << std::endl);
        running = false;

        if (_msg_queue.size() > 0)
        {
            CPPDEBUG("net_udpNC_consumer::close() still has " << _msg_queue.size() << "packets left in queue" << std::endl);
        }

        _msg_queue.close();
        if (_consumerthread != NULL)
        {
            _consumerthread->join ();
        }
    }

    net_udpNC_consumer::~net_udpNC_consumer ()
    {
        delete _consumerthread;

        std::cout << "\n~net_udpNC_consumer: total_packets_processed: " << total_packets_processed << std::endl;
        std::cout << "~net_udpNC_consumer: total_not_meantforme_unicast: " << total_not_meantforme_unicast <<  std::endl;
        std::cout << "~net_udpNC_consumer: total_not_meantforme_multicast: " << total_not_meantforme_multicast <<  std::endl;
        std::cout << "~net_udpNC_consumer: total_toolate_packets: " << total_toolate_packets <<  std::endl;
        std::cout << "~net_udpNC_consumer: total used packets: " << total_usedpackets <<  std::endl;
    }
}
