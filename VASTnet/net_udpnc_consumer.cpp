#include "net_udpnc_consumer.h"
#include "VASTnet.h"
#include "net_udpnc_mchandler.h"
#include "logger.h"

namespace Vast
{

    net_udpNC_consumer::net_udpNC_consumer()
    {
    }

    void net_udpNC_consumer::open(AbstractRLNCMsgReceiver *RLNCsink,
                                  abstract_net_udp *abs_netudp,
                                  net_udpNC_MChandler* mchandler)
    {
        CPPDEBUG("net_udpNC_consumer::open" << std::endl);
        this->RLNCsink = RLNCsink;
        this->abs_netudp = abs_netudp;
        this->mchandler = mchandler;
        start_consuming();
    }

    void net_udpNC_consumer::RLNC_msg_received (RLNCMessage msg, ip::udp::endpoint* remote_endptr)
    {
        msg.endptr = remote_endptr;
        _msg_queue.push (msg);
    }

    void net_udpNC_consumer::start_consuming()
    {
        CPPDEBUG("net_udpNC_consumer::start_consuming" << std::endl);
        _consumerthread = new boost::thread(boost::bind(&net_udpNC_consumer::consume, this));
    }

    void net_udpNC_consumer::consume()
    {
        while(running)
        {
            if (total_packets_processed == 0)
                CPPDEBUG("net_udpNC_consumer::consume Processing message on thread" << std::endl);

            RLNCMessage msg = _msg_queue.pop ();

            //Check if we are still running, or just been cancelled
            if (running)
                process_input(msg, msg.endptr);
        }

    }

    void net_udpNC_consumer::process_input (RLNCMessage input_message, ip::udp::endpoint* remote_endptr)
    {

        total_packets_processed++;

        if (input_message.getPacketIds ().size() > 1)
            throw std::logic_error("net_udpNC_consumer::process_input: Should not have encoded packets here\n");

        filter_input (input_message, remote_endptr);
    }

    void net_udpNC_consumer::filter_input (RLNCMessage input_message, ip::udp::endpoint* remote_endptr)
    {
        if (input_message.getToAddrs ().size () > 0)
        {
            if (abs_netudp->getPublicIPaddr () == input_message.getToAddrs ()[0])
            {
                //All is well, continue
                //CPPDEBUG("net_udpNC_consumer::process_input Message ToAddr was meant for me..." << std::endl);
                order_input(input_message, remote_endptr);
                return;
            }
            else
            {
//                CPPDEBUG("net_udpNC_consumer::process_input Message ToAddr was not meant for me..." << std::endl);
                //Reject in some way - probably return
                total_not_meantforme++;
                return;
            }
        }
        else
        {
            CPPDEBUG("net_udpNC_consumer::process_input input_message has no toAddrs" << std::endl);
            return;
        }
    }

    void net_udpNC_consumer::order_input(RLNCMessage input_message, ip::udp::endpoint* remote_endptr)
    {
        id_t firstFromID = input_message.getFirstFromId ();

        if (firstFromID == NET_ID_UNASSIGNED)
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
            CPPDEBUG("net_udpNC_consumer::process_input Resetting chain for " << firstFromID << std::endl);
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

        RLNCsink->RLNC_msg_received (input_message, remote_endptr);
        Logger::debugPeriodic ("net_udpNC_consumer::order_input Size of _msg_queue " + std::to_string(_msg_queue.size()), 100, 10);
    }

    void net_udpNC_consumer::close()
    {
        CPPDEBUG("net_udpNC_consumer::close()" << std::endl);
        running = false;
        _msg_queue.close();
        _consumerthread->join ();
    }

    net_udpNC_consumer::~net_udpNC_consumer ()
    {
        delete _consumerthread;

        CPPDEBUG("~net_udpNC_consumer: total_packets_processed: " << total_packets_processed << std::endl);
        CPPDEBUG("~net_udpNC_consumer: total_not_meantforme: " << total_not_meantforme <<  std::endl);
        CPPDEBUG("~net_udpNC_consumer: total_toolate_packets: " << total_toolate_packets <<  std::endl);
        CPPDEBUG("~net_udpNC_consumer: total used packets: " << total_usedpackets <<  std::endl);
    }
}
