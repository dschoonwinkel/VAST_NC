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

void testFromNETUNASSIGNED()
{
    std::cout << "\n\nrunning testFromNETUNASSIGNED" << std::endl;
    char data1[] = "message1";
    char data2[] = "message2";
    data2[0] = rand() % 25 + 'a';

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

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    Vast::VASTBuffer buf2;
    Vast::Message msg2(0);
    msg2.store(data2,  strlen(data2));
    vast_header2.msg_size = msg2.serialize (NULL);
    buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
    buf2.add (&msg2);

    RLNCHeader_factory factory;

    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);

    int id1 = NET_ID_UNASSIGNED;

    Vast::IPaddr addr1("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);
    message2.putPacketId(id1);
    message2.putFromId (id1);
    message2.putToAddr (addr1);

    message1.putMessage(buf1.data, buf1.size);
    message2.putMessage(buf2.data, buf2.size);

    message1.putOrdering (0);
    message2.putOrdering (0);

    std::cout << message1 << std::endl;
    std::cout << message2 << std::endl;

    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_handler handler(local_endpoint);
    Vast::absnet_udp_testimpl tester;
    io_service ios;
    handler.open (&ios, &tester);
    handler.process_input (message1);
    handler.process_input (message2);


    std::cout << tester.message << std::endl;
    std::cout << std::string(buf2.data, buf2.size) << std::endl;
    //Returned message is the entire RLNC payload, i.e. VAST message - contained in buf2
    assert(tester.message == std::string(buf2.data, buf2.size));

    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

void testOrderingNormalChain()
{
    std::cout << "\n\nrunning testOrderingNormalChain" << std::endl;
    char data1[] = "message1";
    char data2[] = "message2";
    data2[0] = rand() % 25 + 'a';

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

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    Vast::VASTBuffer buf2;
    Vast::Message msg2(0);
    msg2.store(data2,  strlen(data2));
    vast_header2.msg_size = msg2.serialize (NULL);
    buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
    buf2.add (&msg2);

    RLNCHeader_factory factory;

    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);

    std::cout << message1 << std::endl;
    std::cout << message2 << std::endl;

    int id1 = rand() % 10;

    Vast::IPaddr addr1("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);
    message2.putPacketId(id1);
    message2.putFromId (id1);
    message2.putToAddr (addr1);

    message1.putMessage(buf1.data, buf1.size);
    message2.putMessage(buf2.data, buf2.size);


    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_handler handler(local_endpoint);
    Vast::absnet_udp_testimpl tester;
    io_service ios;
    handler.open (&ios, &tester);
    handler.process_input (message1);
    handler.process_input (message2);


    std::cout << tester.message << std::endl;
    std::cout << std::string(buf2.data, buf2.size) << std::endl;
    //Returned message is the entire RLNC payload, i.e. VAST message - contained in buf2
    assert(tester.message == std::string(buf2.data, buf2.size));

    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

void testOrderingWrongChain()
{
    std::cout << "\n\nrunning testOrderingWrongChain" << std::endl;
    char data1[] = "message1";
    char data2[] = "message2";
    data2[0] = rand() % 25 + 'a';

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

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    Vast::VASTBuffer buf2;
    Vast::Message msg2(0);
    msg2.store(data2,  strlen(data2));
    vast_header2.msg_size = msg2.serialize (NULL);
    buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
    buf2.add (&msg2);

    RLNCHeader_factory factory;

    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);

    int id1 = rand() % 10;

    Vast::IPaddr addr1("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);

    message2.putPacketId(id1);
    message2.putFromId (id1);
    message2.putToAddr (addr1);

    message1.putMessage(buf1.data, buf1.size);
    message2.putMessage(buf2.data, buf2.size);


    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_handler handler(local_endpoint);
    Vast::absnet_udp_testimpl tester;
    io_service ios;
    handler.open (&ios, &tester);

    handler.process_input (message2);
    handler.process_input (message1);

    std::cout << tester.message << std::endl;
    std::cout << std::string(buf2.data, buf2.size) << std::endl;
    //Returned message is the entire RLNC payload, i.e. VAST message - contained in buf2
    assert(tester.message == std::string(buf2.data, buf2.size));

    //Try to process the same packet again - changing the message so that we can distinguish it
    message2.putMessage (buf1.data, buf1.size);

    handler.process_input(message2);

    //The latest received packet should still be the same as previously, i.e. changed message 2 should not
    // be processed
    assert(tester.message == std::string(buf2.data, buf2.size));

    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

}

void testOrderingIndividualChains()
{
    std::cout << "\n\nrunning testOrderingIndividualChains" << std::endl;
    char data1[] = "message1";
    char data2[] = "message2";
    data2[0] = rand() % 25 + 'a';

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

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    Vast::VASTBuffer buf2;
    Vast::Message msg2(0);
    msg2.store(data2,  strlen(data2));
    vast_header2.msg_size = msg2.serialize (NULL);
    buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
    buf2.add (&msg2);

    RLNCHeader_factory factory;

    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);

    int id1 = rand() % 10;
    int id2 = -1;
    do
    {
       id2  = rand() % 10;
    }
    while(id2 == id1);

    Vast::IPaddr addr1("127.0.0.1", 1037);
    Vast::IPaddr addr2("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);
    message2.putPacketId(id2);
    message2.putFromId (id2);
    message2.putToAddr (addr2);

    message1.putMessage(buf1.data, buf1.size);
    message2.putMessage(buf2.data, buf2.size);


    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_handler handler(local_endpoint);
    Vast::absnet_udp_testimpl tester;
    io_service ios;
    handler.open (&ios, &tester);

    handler.process_input (message1);

    //Check if we have received the first message from the first ID
    //Returned message is the entire RLNC payload, i.e. VAST message - contained in buf2
    assert(tester.message == std::string(buf1.data, buf1.size));

    handler.process_input (message2);
//    std::cout << tester.message << std::endl;
//    std::cout << std::string(buf2.data, buf2.size) << std::endl;

    //Check if we have received the second message from the second ID
    assert(tester.message == std::string(buf2.data, buf2.size));

    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

void testOrderingEndOfChain()
{
    std::cout << "\n\nrunning testOrderingEndOfChain" << std::endl;
    char data1[] = "message1";
    char data2[] = "message2";
    data2[0] = rand() % 25 + 'a';

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

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    Vast::VASTBuffer buf2;
    Vast::Message msg2(0);
    msg2.store(data2,  strlen(data2));
    vast_header2.msg_size = msg2.serialize (NULL);
    buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
    buf2.add (&msg2);

    RLNCHeader_factory factory;

    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);

    message1.putOrdering (255);
    //Ordering 256 == 0 because of overflow
    message2.putOrdering (256);

    std::cout << message1 << std::endl;
    std::cout << message2 << std::endl;

    int id1 = rand() % 10;

    Vast::IPaddr addr1("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);
    message2.putPacketId(id1);
    message2.putFromId (id1);
    message2.putToAddr (addr1);

    message1.putMessage(buf1.data, buf1.size);
    message2.putMessage(buf2.data, buf2.size);


    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_handler handler(local_endpoint);
    Vast::absnet_udp_testimpl tester;
    io_service ios;
    handler.open (&ios, &tester);
    handler.process_input (message1);
    handler.process_input (message2);


    std::cout << tester.message << std::endl;
    std::cout << std::string(buf2.data, buf2.size) << std::endl;
    //Returned message is the entire RLNC payload, i.e. VAST message - contained in buf2
    assert(tester.message == std::string(buf2.data, buf2.size));

    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

}

void testOrderingLossyEndOfChain()
{
    std::cout << "\n\nrunning testOrderingLossyEndOfChain" << std::endl;
    char data1[] = "message1";
    char data2[] = "message2";
    data2[0] = rand() % 25 + 'a';

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

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    Vast::VASTBuffer buf2;
    Vast::Message msg2(0);
    msg2.store(data2,  strlen(data2));
    vast_header2.msg_size = msg2.serialize (NULL);
    buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
    buf2.add (&msg2);

    RLNCHeader_factory factory;

    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);

    //Simulate loss of packets -> last highest packet and first low packet
    message1.putOrdering (246);
    message2.putOrdering (9);

    std::cout << message1 << std::endl;
    std::cout << message2 << std::endl;

    int id1 = rand() % 10;

    Vast::IPaddr addr1("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);
    message2.putPacketId(id1);
    message2.putFromId (id1);
    message2.putToAddr (addr1);

    message1.putMessage(buf1.data, buf1.size);
    message2.putMessage(buf2.data, buf2.size);


    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_handler handler(local_endpoint);
    Vast::absnet_udp_testimpl tester;
    io_service ios;
    handler.open (&ios, &tester);
    handler.process_input (message1);
    handler.process_input (message2);


    std::cout << tester.message << std::endl;
    std::cout << std::string(buf2.data, buf2.size) << std::endl;
    //Returned message is the entire RLNC payload, i.e. VAST message - contained in buf2
    assert(tester.message == std::string(buf2.data, buf2.size));


    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

void testOrderingVeryLossyEndOfChain()
{
    std::cout << "\n\nrunning testOrderingVeryLossyEndOfChain" << std::endl;
    char data1[] = "message1";
    char data2[] = "message2";
    data2[0] = rand() % 25 + 'a';

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

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    Vast::VASTBuffer buf2;
    Vast::Message msg2(0);
    msg2.store(data2,  strlen(data2));
    vast_header2.msg_size = msg2.serialize (NULL);
    buf2.add ((char *)&vast_header2, sizeof (Vast::VASTHeader));
    buf2.add (&msg2);

    RLNCHeader_factory factory;

    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);

    //Simulate loss of packets -> last highest packet and first low packet
    message1.putOrdering (LOWEST_RESET_PACKET_ORDERING_NUMBER - 1);
    message2.putOrdering (9);

    std::cout << message1 << std::endl;
    std::cout << message2 << std::endl;

    int id1 = rand() % 10;

    Vast::IPaddr addr1("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);
    message2.putPacketId(id1);
    message2.putFromId (id1);
    message2.putToAddr (addr1);

    message1.putMessage(buf1.data, buf1.size);
    message2.putMessage(buf2.data, buf2.size);


    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_handler handler(local_endpoint);
    Vast::absnet_udp_testimpl tester;
    io_service ios;
    handler.open (&ios, &tester);
    handler.process_input (message1);
    handler.process_input (message2);


    std::cout << tester.message << std::endl;
    std::cout << std::string(buf1.data, buf1.size) << std::endl;
    //Returned message is the entire RLNC payload, i.e. VAST message - contained in buf2
    assert(tester.message == std::string(buf1.data, buf1.size));

    //Simulate loss of packets -> last highest packet and first low packet
    message1.putOrdering (254);
    message2.putOrdering (HIGHEST_RESET_ACCEPTING_ORDERING_NUMBER + 1);

    handler.process_input (message1);
    handler.process_input (message2);
    std::cout << tester.message << std::endl;
    std::cout << std::string(buf2.data, buf2.size) << std::endl;
    //Returned message is the entire RLNC payload, i.e. VAST message - contained in buf2
    assert(tester.message == std::string(buf1.data, buf1.size));


    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

void testHandleNoToAddress ()
{
    std::cout << "\n\nrunning testHandleNoToAddress" << std::endl;

    RLNCHeader_factory factory;

    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);



    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_handler handler(local_endpoint);
    Vast::absnet_udp_testimpl tester;
    io_service ios;
    handler.open (&ios, &tester);
    handler.process_input (message1);
    handler.process_input (message2);

    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

void testThrowEncodedPacketException()
{
    std::cout << "\n\nrunning testThrowEncodedPacketException" << std::endl;
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

    int id1 = rand() % 10;
    int id2 = -1;
    do
    {
       id2  = rand() % 10;
    }
    while(id2 == id1);

    Vast::IPaddr addr1("127.0.0.1", 1037);
    Vast::IPaddr addr2("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);

    message1.putPacketId(id2);
    message1.putFromId (id2);
    message1.putToAddr (addr2);

    message1.putMessage(buf1.data, buf1.size);


    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_handler handler(local_endpoint);
    Vast::absnet_udp_testimpl tester;
    io_service ios;
    handler.open (&ios, &tester);

    try {
        handler.process_input (message1);
        std::cout << "Test should have thrown an exception already before this" << std::endl;
        std::abort();
    }
    catch (std::logic_error)
    {
        std::cout << "Test success" << std::endl;
    }

    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

int main()
{
//    runUnitTest1();
    testFromNETUNASSIGNED ();
    testOrderingNormalChain ();
    testOrderingWrongChain ();
    testOrderingIndividualChains();
    testOrderingEndOfChain ();
    testOrderingLossyEndOfChain ();
    testOrderingVeryLossyEndOfChain ();
    testHandleNoToAddress ();
    testThrowEncodedPacketException();






    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;

    return 0;
}
