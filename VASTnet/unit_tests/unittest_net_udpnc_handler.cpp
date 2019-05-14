#include <iostream>
#include <assert.h>
#include "net_udpNC_handler.h"
#include "rlnc_packet_factory.h"
#include "rlncmessage.h"
#include "absnet_udp_testimpl.h"
#include <boost/asio.hpp>
#include "VASTnet.h"
#include "VASTBuffer.h"
#include "string.h"
#include <cstring>
#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>



using namespace boost::asio;

size_t sleep_time = 200;

//void runUnitTest1()
//{
//    std::cout << "Not implemented yet!" << std::endl;
//    std::abort();
//}

void testHandoffInput()
{
    std::cout << "\n\nrunning testHandoffInput" << std::endl;
    char data1[] = "message1";

    Vast::VASTHeader vast_header1;
    vast_header1.start = 10;
    vast_header1.type = 0;
    vast_header1.end = 5;

    Vast::VASTBuffer buf1;
    Vast::Message msg1(0);
    msg1.store(data1,  strlen(data1));
    vast_header1.msg_size = msg1.serialize (NULL);
    buf1.add ((char *)&vast_header1, sizeof (Vast::VASTHeader));
    buf1.add (&msg1);

    RLNCHeader_factory factory;

    RLNCHeader header1 = factory.build();

    RLNCMessage message1(header1);

    int id1 = 2;

    Vast::IPaddr addr1("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);

    message1.putMessage(buf1.data, buf1.size);

    message1.putOrdering (0);

    std::cout << message1 << std::endl;

    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_handler handler(local_endpoint);
    Vast::absnet_udp_testimpl tester;
    io_service ios;
    handler.open (&ios, &tester);
    handler.RLNC_msg_received (message1, Vast::IPaddr());


    std::cout << tester.message << std::endl;
    std::cout << std::string(buf1.data, buf1.size) << std::endl;
    //Returned message is the entire RLNC payload, i.e. VAST message - contained in buf2
    assert(tester.message == std::string(buf1.data, buf1.size));

    handler.close();

    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

int main()
{
//    runUnitTest1();
    testHandoffInput ();

    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;

    return 0;
}
