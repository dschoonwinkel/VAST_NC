#include "udpncmessage.h"
#include "net_udpNC_handler.h"
#include "net_udpnc_consumer.h"
#include <assert.h>
#include <netudpnc_capturemsgs.h>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <map>
#include <boost/asio.hpp>
#include "VASTVerse.h"
#include "absnet_udp_testimpl.h"
#include "abstract_udpnc_msg_receiver_testimpl.h"
#include "packet_listener.h"
#include "abstract_sender_impl.h"
#include <chrono>
#include "logger.h"
#include <signal.h>
#include <cstdlib>

using namespace boost::asio;

using namespace Vast;
using namespace std;

std::vector<string> msgcapIDs;
std::map<string, netudpnc_capturemsgs> allRestoredMsgs;
std::vector<UDPNCMessage> savedRelevantMsgs;
std::vector<UDPNCMessage> processedMsgs;
timestamp_t latest_timestamp;
ip::udp::endpoint endpoint(ip::address::from_string("10.0.0.2"), 1037);
AbstractUDPNCMsgReceiverTestImpl UDPNCsink;

SimPara simpara;
int g_MS_PER_TIMESTEP;

bool running = true;

std::map<ip::udp::endpoint, size_t> packets_sent_unicast;
std::map<ip::udp::endpoint, size_t> packets_dropped_unicast;

void SIGINT_handler (int)
{
    std::cout << "Interrupt received" << std::endl;
    running = false;
}


bool readIniFile(const char* filename /*VASTreal.ini*/)
{
    printf("readIniFile called\n");
    FILE *fp;
    if ((fp = fopen (filename, "rt")) == NULL)
        return false;

    int *p[] = {
        &simpara.VAST_MODEL,
        &simpara.NET_MODEL,
        &simpara.MOVE_MODEL,
        &simpara.WORLD_WIDTH,
        &simpara.WORLD_HEIGHT,
        &simpara.NODE_SIZE,
        &simpara.RELAY_SIZE,
        &simpara.MATCHER_SIZE,
        &simpara.TIME_STEPS,
        &simpara.STEPS_PERSEC,
        &simpara.AOI_RADIUS,
        &simpara.AOI_BUFFER,
        &simpara.CONNECT_LIMIT,
        &simpara.VELOCITY,
        &simpara.STABLE_SIZE,
        &simpara.JOIN_RATE,
        &simpara.LOSS_RATE,
        &simpara.FAIL_RATE,
        &simpara.UPLOAD_LIMIT,
        &simpara.DOWNLOAD_LIMIT,
        &simpara.PEER_LIMIT,
        &simpara.RELAY_LIMIT,
        &simpara.OVERLOAD_LIMIT,
        &simpara.TIMESTEP_DURATION,
        0
    };

    char buff[255];
    int n = 0;
    while (fgets (buff, 255, fp) != NULL)
    {
        // skip any comments or empty lines
        if (buff[0] == '#' || buff[0] == ' ' || buff[0] == '\n')
            continue;

        // read the next valid parameter
        if (sscanf (buff, "%d ", p[n]) != 1)
            return false;
        n++;

        //Null Pointer, i.e. ending of the struct?
        if (p[n] == 0)
            return true;
    }

    return false;

}

void initVariables()
{
    g_MS_PER_TIMESTEP = simpara.TIMESTEP_DURATION;

    path dir_path("./msgcap");
    directory_iterator end_itr;

    for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {

        //Skip subfolders
        if (is_directory(itr->path())) continue;

        CPPDEBUG(itr->path() << std::endl);
        std::string filename = itr->path().string();
        //If this is not a TXT file, it is probably not a VASTStatLog file
        if (filename.find(".txt") == string::npos)
        {
            CPPDEBUG("Skipping " << filename << std::endl);
            continue;
        }
        netudpnc_capturemsgs restoredMsgs(filename);

        //Cut off .txt
        std::string id_string = filename.substr(0, filename.find(".txt"));
        //Extract id_t
        id_string = id_string.substr(id_string.find("N") + 1);

        allRestoredMsgs[filename] = restoredMsgs;
        msgcapIDs.push_back(filename);

        latest_timestamp = ULONG_LONG_MAX;
        //Initiate latest_timestamp to the earliest timestamp
        for (size_t log_iter = 0; log_iter < msgcapIDs.size(); log_iter++)
        {
          timestamp_t t = 0;
          allRestoredMsgs[msgcapIDs[log_iter]].getCurrentTimestamp(t);
          CPPDEBUG("Starting timestamp: " << t << std::endl);

          if (t < latest_timestamp)
          {
              latest_timestamp = t;
          }
        }
        CPPDEBUG("RestoredMsgsCount: " << restoredMsgs.getRestoredMsgsCount() << std::endl);
    }
}

void sendMessages()
{
    net_udpNC_handler handler(endpoint);
    io_service ios;
    absnet_udp_testimpl absudp(IPaddr(endpoint.address().to_v4().to_ulong(), 1037));
    handler.open(&ios, &absudp, &UDPNCsink);
    timestamp_t ticker = latest_timestamp;


    //Coding host variables
    packet_listener _packet_listener(endpoint);
    abstract_sender_impl sender(&handler);
    io_service packetlistner_service;

    _packet_listener.open(&packetlistner_service, &sender);

    Logger::registerThreadName(std::this_thread::get_id(), "udpnc_integration_test_thread");

    while (running)
    {
//        std::cout << "Current ticker: " << ticker << std::endl;
        bool sending = false;
        for (size_t msgcap_iter = 0; msgcap_iter < msgcapIDs.size(); msgcap_iter++)
        {
//            std::cout << "sendMessages: in msgcap_iter loop: " << msgcap_iter << std::endl;
            NCWrapper wrapper;
            bool ready = allRestoredMsgs[msgcapIDs[msgcap_iter]].
                    restoreNextNCMessage(ticker, wrapper);
//            std::cout << "sendMessages ready = " << ready << std::endl;
            if (ready)
            {
                std::cout << "**************\nMessage ready" << std::endl;
//                std::cout << "NCWrapper in msgcap[" << msgcapIDs[msgcap_iter] << std::endl
//                          << wrapper << std::endl;
//                handler.send(wrapper._buffer.c_str(), wrapper._buffer.length(), endpoint);

                int LOSS_PERCENTAGE = 20;
                int rand_num = rand() % 100;
                if (rand_num < LOSS_PERCENTAGE)
                {
                    CPPDEBUG("Dropping packet" << std::endl);
                    packets_dropped_unicast[wrapper._local_endpoint]++;

                }
                else
                {
                    packets_sent_unicast[wrapper._local_endpoint]++;
                    handler.process_input(wrapper._buffer.c_str(), wrapper._local_endpoint, wrapper._buffer.length());
                }

//                std::cout << "Sending packet to packet_listener" << std::endl;
                _packet_listener.process_input(wrapper._buffer.c_str(), wrapper._buffer.length());
            }
            sending = sending || ready;
            running = running && !allRestoredMsgs[msgcapIDs[msgcap_iter]].finished();

        }
        if (sending)
        {
//            std::cout << "sending was true" << std::endl;
        }
        else
        {
            std::cout << "Tick\n*******************" << std::endl;
            ticker += simpara.TIMESTEP_DURATION;
            this_thread::sleep_for (std::chrono::milliseconds(simpara.TIMESTEP_DURATION / 5));
//            ticker += 1;
            sending = false;
        }
    }

    //Wait for the consumer to finish processing
    this_thread::sleep_for (std::chrono::milliseconds(100));

    handler.close();
    _packet_listener.close();
}

void checkInputEqualOutput()
{
    size_t meant_to_endpoint = 0;

    for (auto it = allRestoredMsgs.begin(); it != allRestoredMsgs.end(); ++it)
    {
        std::vector<NCWrapper> restoredMsgs = (it->second).getRestoredNCMsgs();

        for (auto it2 = restoredMsgs.begin(); it2 != restoredMsgs.end(); ++it2)
        {
            if (it2->_remote_endpoint == endpoint)
            {
                meant_to_endpoint++;
                UDPNCMessage msg;
                msg.deserialize(it2->_buffer.c_str(), it2->_buffer.length());
                savedRelevantMsgs.push_back(msg);
            }
        }
    }

    std::cout << meant_to_endpoint << " packets meant for this endpoint" << std::endl;
    std::cout << UDPNCsink.UDPNC_msg_received_call_count << " packets received in UDPNCsink" << std::endl;

    std::cout << "Checking if received msgs are equal to sent msgs: " << std::endl;
    bool all_equal = true;

    if (UDPNCsink.all_recv_msgs.size() == savedRelevantMsgs.size())
    {
        for (size_t i = 0; i < UDPNCsink.all_recv_msgs.size(); i++)
        {
            all_equal = all_equal && UDPNCsink.all_recv_msgs[i].contentEquals(savedRelevantMsgs[i]);
            if (!(UDPNCsink.all_recv_msgs[i].contentEquals(savedRelevantMsgs[i])))
            {
                std::cout << "Messages were not equal @ " << i << std::endl
                          << UDPNCsink.all_recv_msgs[i]
                          << savedRelevantMsgs[i]
                          << std::endl;
            }
        }
    }
    else
    {
        std::cout << "UDPNCsink and savedRelevantMsgs unequal size" << std::endl;
        std::cout << UDPNCsink.all_recv_msgs.size() << " vs " << savedRelevantMsgs.size() << std::endl;
        all_equal = false;
    }

    if (all_equal)
    {
        std::cout << "All packets sent & received correctly" << std::endl;
    }
    else
    {
        std::cout << "Some packets sent & received incorrectly" << std::endl;
    }

    std::cout << "packets_sent_unicast" << std::endl;
    for (auto it = packets_sent_unicast.begin(); it != packets_sent_unicast.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << "packets_dropped_unicast" << std::endl;
    for (auto it = packets_dropped_unicast.begin(); it != packets_dropped_unicast.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    //Put sent and received messages in a map, to verify if those that were received, were received correctly.
    std::map<packetid_t, UDPNCMessage> saved_sent_messages;
    std::map<packetid_t, UDPNCMessage> recvd_messages;
    for (auto it = savedRelevantMsgs.begin(); it != savedRelevantMsgs.end(); ++it)
    {
        saved_sent_messages[(*it).getPacketIds()[0]] = (*it);
    }

    for (auto it = UDPNCsink.all_recv_msgs.begin(); it != UDPNCsink.all_recv_msgs.end(); ++it)
    {
        recvd_messages[(*it).getPacketIds()[0]] = (*it);
    }

    size_t recvd_messages_correct = 0;
    //Check recvd messages against original saved_sending_messages
    for (auto it = recvd_messages.begin(); it != recvd_messages.end(); ++it)
    {
        if (recvd_messages[it->first].contentEquals(saved_sent_messages[it->first]))
        {
            recvd_messages_correct++;
        }
    }

    std::cout << "Checking correctly of received packets: " << std::endl <<
                 recvd_messages_correct << "correct of " << recvd_messages.size() << std::endl;

}

int main()
{
    signal(SIGINT, SIGINT_handler);

    srand(0);
    readIniFile("../../bin/VASTreal.ini");
    initVariables();
    sendMessages();
    checkInputEqualOutput();

//    std::cout << allRestoredMsgs[msgcapIDs[0]][0]._msg << std::endl;

    return 0;
}
