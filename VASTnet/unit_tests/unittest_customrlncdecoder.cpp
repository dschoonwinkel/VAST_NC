#include "rlncmessage.h"
#include "rlncrecoder.h"
#include <vector>
#include <boost/asio.hpp>
#include <iostream>
#include "VASTnet.h"
#include <array>
#include <time.h>
#include <stdlib.h>
#include "net_udpnc_mchandler.h"
#include "abstract_rlnc_msg_receiver_testimpl.h"
#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>

size_t sleep_time = 1000;

void testFirstIndex()
{
    std::cout << "\n\ntestFirstIndex" << std::endl;
    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_MChandler mchandler(local_endpoint);
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester, false);

    char data1[] = {'d', 'e', 'a', 'd'};
    char data2[] = {'b', 'e', 'e', 'f'};
    data2[0] = rand() % 25 + 'a';

    std::array<char, 100> vast_data1;

    Vast::VASTHeader vast_header1;
    vast_header1.start = 10;
    vast_header1.type = 0;
    vast_header1.end = 5;
    vast_header1.msg_size = 4;
    std::fill(vast_data1.begin (), vast_data1.end (), 0);
    memcpy(vast_data1.data (), &vast_header1, sizeof(Vast::VASTHeader));
    memcpy(vast_data1.data()+sizeof(Vast::VASTHeader), data1, 4);

    std::array<char, 100> vast_data2;

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    vast_header2.msg_size = 4;
    std::fill(vast_data2.begin (), vast_data2.end (), 0);
    memcpy(vast_data2.data (), &vast_header2, sizeof(Vast::VASTHeader));
    memcpy(vast_data2.data ()+sizeof(Vast::VASTHeader), data2, 4);


    RLNCrecoder recoder;

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
    Vast::IPaddr addr2("127.0.0.2", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);
    message2.putPacketId(id2);
    message2.putFromId (id2);
    message2.putToAddr (addr2);

    message1.putMessage(vast_data1.data (), 100);
    message2.putMessage(vast_data2.data (), 100);

    std::cout << "Symbol 1:" << message1 << std::endl;
    std::cout << "Symbol 2:" << message2 << std::endl;

    recoder.addRLNCMessage(message1);
    recoder.addRLNCMessage(message2);

    RLNCMessage *temp_msg = recoder.produceRLNCMessage();

    if (!temp_msg)
    {
        std::cerr << "Could not produce RLNC encoded message" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::array<uint8_t, 1400> payload;
    payload.fill (0);
    message1.serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 1);
    assert(tester.recv_msg == message2);

    mchandler.close();

//    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

}

void testSecondIndex()
{
    std::cout << "\n\ntestSecondIndex" << std::endl;
    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_MChandler mchandler(local_endpoint);
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester, false);

    char data1[] = {'d', 'e', 'a', 'd'};
    char data2[] = {'b', 'e', 'e', 'f'};
    data2[0] = rand() % 25 + 'a';

    std::array<char, 100> vast_data1;

    Vast::VASTHeader vast_header1;
    vast_header1.start = 10;
    vast_header1.type = 0;
    vast_header1.end = 5;
    vast_header1.msg_size = 4;
    std::fill(vast_data1.begin (), vast_data1.end (), 0);
    memcpy(vast_data1.data (), &vast_header1, sizeof(Vast::VASTHeader));
    memcpy(vast_data1.data()+sizeof(Vast::VASTHeader), data1, 4);

    std::array<char, 100> vast_data2;

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    vast_header2.msg_size = 4;
    std::fill(vast_data2.begin (), vast_data2.end (), 0);
    memcpy(vast_data2.data (), &vast_header2, sizeof(Vast::VASTHeader));
    memcpy(vast_data2.data ()+sizeof(Vast::VASTHeader), data2, 4);


    RLNCrecoder recoder;

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
    Vast::IPaddr addr2("127.0.0.2", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);
    message2.putPacketId(id2);
    message2.putFromId (id2);
    message2.putToAddr (addr2);

    message1.putMessage(vast_data1.data (), 100);
    message2.putMessage(vast_data2.data (), 100);

    std::cout << "Symbol 1:" << message1 << std::endl;
    std::cout << "Symbol 2:" << message2 << std::endl;

    recoder.addRLNCMessage(message1);
    recoder.addRLNCMessage(message2);

    RLNCMessage *temp_msg = recoder.produceRLNCMessage();

    if (!temp_msg)
    {
        std::cerr << "Could not produce RLNC encoded message" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::array<uint8_t, 1400> payload;

    payload.fill (0);
    message2.serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 1);
    assert(tester.recv_msg == message1);

    mchandler.close();
//    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

void testUndecodable()
{
    std::cout << "\n\ntestUndecodable" << std::endl;
    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_MChandler mchandler(local_endpoint);
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester, false);

    char data1[] = {'d', 'e', 'a', 'd'};
    char data2[] = {'b', 'e', 'e', 'f'};
    data2[0] = rand() % 25 + 'a';

    std::array<char, 100> vast_data1;

    Vast::VASTHeader vast_header1;
    vast_header1.start = 10;
    vast_header1.type = 0;
    vast_header1.end = 5;
    vast_header1.msg_size = 4;
    std::fill(vast_data1.begin (), vast_data1.end (), 0);
    memcpy(vast_data1.data (), &vast_header1, sizeof(Vast::VASTHeader));
    memcpy(vast_data1.data()+sizeof(Vast::VASTHeader), data1, 4);

    std::array<char, 100> vast_data2;

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    vast_header2.msg_size = 4;
    std::fill(vast_data2.begin (), vast_data2.end (), 0);
    memcpy(vast_data2.data (), &vast_header2, sizeof(Vast::VASTHeader));
    memcpy(vast_data2.data ()+sizeof(Vast::VASTHeader), data2, 4);


    RLNCrecoder recoder;

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
    Vast::IPaddr addr2("127.0.0.2", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);
    message2.putPacketId(id2);
    message2.putFromId (id2);
    message2.putToAddr (addr2);

    message1.putMessage(vast_data1.data (), 100);
    message2.putMessage(vast_data2.data (), 100);

    recoder.addRLNCMessage(message1);
    recoder.addRLNCMessage(message2);

    RLNCMessage *temp_msg = recoder.produceRLNCMessage();

    if (!temp_msg)
    {
        std::cerr << "Could not produce RLNC encoded message" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::array<uint8_t, 1400> payload;

    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 0);
    assert(mchandler.getPacketPoolSize () == 0);

    mchandler.close();

//    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

void testUnnecessary()
{
    std::cout << "\n\ntestUnnecessary" << std::endl;
    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_MChandler mchandler(local_endpoint);
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester, false);

    char data1[] = {'d', 'e', 'a', 'd'};
    char data2[] = {'b', 'e', 'e', 'f'};
    data2[0] = rand() % 25 + 'a';

    std::array<char, 100> vast_data1;

    Vast::VASTHeader vast_header1;
    vast_header1.start = 10;
    vast_header1.type = 0;
    vast_header1.end = 5;
    vast_header1.msg_size = 4;
    std::fill(vast_data1.begin (), vast_data1.end (), 0);
    memcpy(vast_data1.data (), &vast_header1, sizeof(Vast::VASTHeader));
    memcpy(vast_data1.data()+sizeof(Vast::VASTHeader), data1, 4);

    std::array<char, 100> vast_data2;

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    vast_header2.msg_size = 4;
    std::fill(vast_data2.begin (), vast_data2.end (), 0);
    memcpy(vast_data2.data (), &vast_header2, sizeof(Vast::VASTHeader));
    memcpy(vast_data2.data ()+sizeof(Vast::VASTHeader), data2, 4);


    RLNCrecoder recoder;

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
    Vast::IPaddr addr2("127.0.0.2", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);
    message2.putPacketId(id2);
    message2.putFromId (id2);
    message2.putToAddr (addr2);

    message1.putMessage(vast_data1.data (), 100);
    message2.putMessage(vast_data2.data (), 100);

    recoder.addRLNCMessage(message1);
    recoder.addRLNCMessage(message2);

    RLNCMessage *temp_msg = recoder.produceRLNCMessage();

    if (!temp_msg)
    {
        std::cerr << "Could not produce RLNC encoded message" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::array<uint8_t, 1400> payload;

    payload.fill (0);
    message1.serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    message2.serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 0);

    mchandler.close();

//    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

void testExtraPacket()
{
    std::cout << "\n\ntestExtraPacket" << std::endl;
    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_MChandler mchandler(local_endpoint);
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester, false);

    char data1[] = {'d', 'e', 'a', 'd'};
    char data2[] = {'b', 'e', 'e', 'f'};
    char data3[] = {'a', 'b', 'c', 'd'};
    data2[0] = rand() % 25 + 'a';

    std::array<char, 100> vast_data1;

    Vast::VASTHeader vast_header1;
    vast_header1.start = 10;
    vast_header1.type = 0;
    vast_header1.end = 5;
    vast_header1.msg_size = 4;
    std::fill(vast_data1.begin (), vast_data1.end (), 0);
    memcpy(vast_data1.data (), &vast_header1, sizeof(Vast::VASTHeader));
    memcpy(vast_data1.data()+sizeof(Vast::VASTHeader), data1, 4);

    std::array<char, 100> vast_data2;

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    vast_header2.msg_size = 4;
    std::fill(vast_data2.begin (), vast_data2.end (), 0);
    memcpy(vast_data2.data (), &vast_header2, sizeof(Vast::VASTHeader));
    memcpy(vast_data2.data ()+sizeof(Vast::VASTHeader), data2, 4);

    std::array<char, 100> vast_data3;

    Vast::VASTHeader vast_header3;
    vast_header3.start = 10;
    vast_header3.type = 0;
    vast_header3.end = 5;
    vast_header3.msg_size = 4;
    std::fill(vast_data3.begin (), vast_data3.end (), 0);
    memcpy(vast_data3.data (), &vast_header3, sizeof(Vast::VASTHeader));
    memcpy(vast_data3.data ()+sizeof(Vast::VASTHeader), data3, 4);


    RLNCrecoder recoder;

    RLNCHeader_factory factory;
    RLNCHeader header1 = factory.build();
    RLNCHeader header2 = factory.build();
    RLNCHeader header3 = factory.build();

    RLNCMessage message1(header1);
    RLNCMessage message2(header2);
    RLNCMessage message3(header3);

    int id1 = rand() % 10;
    int id2 = -1;
    int id3 = -1;
    do
    {
       id2  = rand() % 10;
    }
    while(id2 == id1);

    do
    {
       id3  = rand() % 10;
    }
    while(id3 == id2 || id3 == id1);

    Vast::IPaddr addr1("127.0.0.1", 1037);
    Vast::IPaddr addr2("127.0.0.2", 1037);
    Vast::IPaddr addr3("127.0.0.3", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);
    message2.putPacketId(id2);
    message2.putFromId (id2);
    message2.putToAddr (addr2);
    message3.putPacketId (id3);
    message3.putFromId (id3);
    message3.putToAddr (addr3);

    message1.putMessage(vast_data1.data (), 100);
    message2.putMessage(vast_data2.data (), 100);
    message3.putMessage (vast_data3.data (), 100);

    recoder.addRLNCMessage(message1);
    recoder.addRLNCMessage(message2);
    recoder.addRLNCMessage(message3);

    RLNCMessage *temp_msg = recoder.produceRLNCMessage();

    if (!temp_msg)
    {
        std::cerr << "Could not produce RLNC encoded message" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::array<uint8_t, 1400> payload;

    payload.fill (0);
    message1.serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 1);
    assert(tester.recv_msg == message2);


    recoder.addRLNCMessage (message1);
    temp_msg = recoder.produceRLNCMessage();

    if (!temp_msg)
    {
        std::cerr << "Could not produce RLNC encoded message" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << message3 << std::endl << std::endl;
    std::cout << (*temp_msg) << std::endl;

    payload.fill (0);
    message2.serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 2);
    assert(tester.recv_msg == message1);

    mchandler.close();
//    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));

}

void testNETIDUNASSIGNED()
{
    std::cout << "\ntestNETIDUNASSIGNED" << std::endl;

    char data1[] = {'d', 'e', 'a', 'd'};

    std::array<char, 100> vast_data1;

    Vast::VASTHeader vast_header1;
    vast_header1.start = 10;
    vast_header1.type = 0;
    vast_header1.end = 5;
    vast_header1.msg_size = 4;
    std::fill(vast_data1.begin (), vast_data1.end (), 0);
    memcpy(vast_data1.data (), &vast_header1, sizeof(Vast::VASTHeader));
    memcpy(vast_data1.data()+sizeof(Vast::VASTHeader), data1, 4);

    RLNCrecoder recoder;

    RLNCHeader_factory factory;
    RLNCHeader header1 = factory.build();

    RLNCMessage message1(header1);

    int id1 = NET_ID_UNASSIGNED;

    Vast::IPaddr addr1("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);

    message1.putMessage(vast_data1.data (), 100);

    recoder.addRLNCMessage(message1);

    assert(recoder.getPacketPoolSize () == 0);

    customrlncdecoder decoder;
    decoder.addRLNCMessage (message1);
    assert(decoder.getPacketPoolSize () == 0);

    RLNCHeader header2 = factory.build();

    RLNCMessage message2(header2);

    int id2 = 123;

    Vast::IPaddr addr2("127.0.0.1", 1037);

    message2.putPacketId(id2);
    message2.putFromId (id2);
    message2.putToAddr (addr2);

    message2.putMessage(vast_data1.data (), 100);

    recoder.addRLNCMessage(message2);

    assert(recoder.getPacketPoolSize () == 1);

    decoder.addRLNCMessage (message2);
    assert(decoder.getPacketPoolSize () == 1);
}

void testThrowPktNotUnique()
{
    std::cout << "\ntestThrowPktNotUnique" << std::endl;

    char data1[] = {'d', 'e', 'a', 'd'};
    char data2[] = {'b', 'e', 'e', 'f'};

    std::array<char, 100> vast_data1;

    Vast::VASTHeader vast_header1;
    vast_header1.start = 10;
    vast_header1.type = 0;
    vast_header1.end = 5;
    vast_header1.msg_size = 4;
    std::fill(vast_data1.begin (), vast_data1.end (), 0);
    memcpy(vast_data1.data (), &vast_header1, sizeof(Vast::VASTHeader));
    memcpy(vast_data1.data()+sizeof(Vast::VASTHeader), data1, 4);

    std::array<char, 100> vast_data2;

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    vast_header2.msg_size = 4;
    std::fill(vast_data2.begin (), vast_data2.end (), 0);
    memcpy(vast_data2.data (), &vast_header2, sizeof(Vast::VASTHeader));
    memcpy(vast_data2.data ()+sizeof(Vast::VASTHeader), data2, 4);

    RLNCHeader_factory factory;
    RLNCHeader header1 = factory.build();

    RLNCMessage message1(header1);

    int id1 = NET_ID_UNASSIGNED;

    Vast::IPaddr addr1("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);

    message1.putMessage(vast_data1.data (), 100);

    customrlncdecoder decoder;
    decoder.addRLNCMessage (message1);
    decoder.addRLNCMessage (message1);
    assert(decoder.getPacketPoolSize () == 0);

    //If something went wrong, we will have an exception wrongly thrown by now

    //Normal packet adding
    RLNCHeader header2 = factory.build();
    RLNCMessage message2(header2);

    int id2 = 123;

    Vast::IPaddr addr2("127.0.0.1", 1037);

    message2.putPacketId(id2);
    message2.putFromId (id2);
    message2.putToAddr (addr2);

    message2.putMessage(vast_data1.data (), 100);

    decoder.addRLNCMessage (message2);
    //Add the same packet twice
    decoder.addRLNCMessage (message2);
    assert(decoder.getPacketPoolSize () == 1);

    //Create a different packet with same id, but different contents
    RLNCHeader header3 = factory.build();

    RLNCMessage message3(header3);

    message3.putPacketId(id2);
    message3.putFromId (id2);
    message3.putToAddr (addr2);

    message3.putMessage(vast_data2.data (), 100);

    try {
        decoder.addRLNCMessage (message3);
        std::abort();
    }
    catch (std::logic_error)
    {
        //It should throw a logic error if we have the same pktid, but not the same packet
        std::cout << "Logic error exception thrown correctly" << std::endl;
    }
}

void testChecksum()
{
    std::cout << "\ntestChecksum" << std::endl;

    char data1[] = {'d', 'e', 'a', 'd'};
    char data2[] = {'b', 'e', 'e', 'f'};

    std::array<char, 100> vast_data1;

    Vast::VASTHeader vast_header1;
    vast_header1.start = 10;
    vast_header1.type = 0;
    vast_header1.end = 5;
    vast_header1.msg_size = 4;
    std::fill(vast_data1.begin (), vast_data1.end (), 0);
    memcpy(vast_data1.data (), &vast_header1, sizeof(Vast::VASTHeader));
    memcpy(vast_data1.data()+sizeof(Vast::VASTHeader), data1, 4);

    std::array<char, 100> vast_data2;

    Vast::VASTHeader vast_header2;
    vast_header2.start = 10;
    vast_header2.type = 0;
    vast_header2.end = 5;
    vast_header2.msg_size = 4;
    std::fill(vast_data2.begin (), vast_data2.end (), 0);
    memcpy(vast_data2.data (), &vast_header2, sizeof(Vast::VASTHeader));
    memcpy(vast_data2.data ()+sizeof(Vast::VASTHeader), data2, 4);

    RLNCHeader_factory factory;
    RLNCHeader header1 = factory.build();

    RLNCMessage message1(header1);

    int id1 = 123;

    Vast::IPaddr addr1("127.0.0.1", 1037);

    message1.putPacketId(id1);
    message1.putFromId (id1);
    message1.putToAddr (addr1);

    message1.putMessage(vast_data1.data (), 100);

    RLNCrecoder recoder;
    recoder.addRLNCMessage (message1);

    //Normal packet adding
    RLNCHeader header2 = factory.build();
    RLNCMessage message2(header2);

    int id2 = 456;

    Vast::IPaddr addr2("127.0.0.1", 1037);

    message2.putPacketId(id2);
    message2.putFromId (id2);
    message2.putToAddr (addr2);

    message2.putMessage(vast_data2.data (), 100);

    recoder.addRLNCMessage (message2);

    std::array<char, MAX_PACKET_SIZE> buffer;

    uint32_t checksum = 0;
    message1.serialize(buffer.data());
    checksum = RLNCMessage::generateChecksum(buffer.data(), message1.sizeOf());

    buffer.fill(0);
    message2.serialize(buffer.data());
    checksum += RLNCMessage::generateChecksum(buffer.data(), message2.sizeOf());

    RLNCMessage *encoded_msg = recoder.produceRLNCMessage();

    assert(encoded_msg != NULL);
    assert(encoded_msg->getChecksum() == checksum);

    std::cout << "Checksum: " << checksum << std::endl;
//    assert(encoded_msg->getChecksum() == 966138353);
    assert(encoded_msg->getChecksum() == 3547018482);
}

void testRepeated()
{
    std::chrono::microseconds coding_timer = std::chrono::microseconds::zero();
    std::chrono::microseconds decoding_timer = std::chrono::microseconds::zero();

    size_t iterations = 10000;

    std::cout << "testRepeated" << std::endl;
    srand(time(NULL));

    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_MChandler mchandler(local_endpoint);
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester, false);

    for (size_t i = 0; i < iterations; i++)
    {

        char data1[] = {'d', 'e', 'a', 'd'};
        char data2[] = {'b', 'e', 'e', 'f'};
        data2[0] = rand() % 25 + 'a';

        std::array<char, 100> vast_data1;

        Vast::VASTHeader vast_header1;
        vast_header1.start = 10;
        vast_header1.type = 0;
        vast_header1.end = 5;
        vast_header1.msg_size = 4;
        std::fill(vast_data1.begin (), vast_data1.end (), 0);
        memcpy(vast_data1.data (), &vast_header1, sizeof(Vast::VASTHeader));
        memcpy(vast_data1.data()+sizeof(Vast::VASTHeader), data1, 4);

        std::array<char, 100> vast_data2;

        Vast::VASTHeader vast_header2;
        vast_header2.start = 10;
        vast_header2.type = 0;
        vast_header2.end = 5;
        vast_header2.msg_size = 4;
        std::fill(vast_data2.begin (), vast_data2.end (), 0);
        memcpy(vast_data2.data (), &vast_header2, sizeof(Vast::VASTHeader));
        memcpy(vast_data2.data ()+sizeof(Vast::VASTHeader), data2, 4);


        RLNCrecoder recoder;

        RLNCHeader_factory factory;
        RLNCHeader header1 = factory.build();
        RLNCHeader header2 = factory.build();

        RLNCMessage message1(header1);
        RLNCMessage message2(header2);

        //Assure that the messages does not have similar or 0 fromIDs
        int id1 = i + 1; // ie. 1 - 10000
        int id2 = i + iterations + 2; // ie. 10002 - 20002, no overlap possible

        Vast::IPaddr addr1("127.0.0.1", 1037);
        Vast::IPaddr addr2("127.0.0.2", 1037);

        message1.putPacketId(id1);
        message1.putFromId (id1);
        message1.putToAddr (addr1);
        message2.putPacketId(id2);
        message2.putFromId (id2);
        message2.putToAddr (addr2);

        message1.putMessage(vast_data1.data (), 100);
        message2.putMessage(vast_data2.data (), 100);

//        std::cout << "Symbol 1:" << message1 << std::endl;
//        std::cout << "Symbol 2:" << message2 << std::endl;

        auto t1 = std::chrono::high_resolution_clock::now();

        recoder.addRLNCMessage(message1);
        recoder.addRLNCMessage(message2);

        RLNCMessage *temp_msg = recoder.produceRLNCMessage();

        auto t2 = std::chrono::high_resolution_clock::now();

        coding_timer += std::chrono::duration_cast<std::chrono::microseconds>(t2-t1);

        if (!temp_msg)
        {
            std::cerr << "Could not produce RLNC encoded message" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::array<uint8_t, 1400> payload;
        payload.fill (0);
        message1.serialize(reinterpret_cast<char*>(payload.data()));
        //Equivalent to receive
        mchandler.process_input(reinterpret_cast<char*>(payload.data()), payload.size ());

        payload.fill (0);
        temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
        //Equivalent to receive

        auto t3 = std::chrono::high_resolution_clock::now();
        mchandler.process_input (reinterpret_cast<char*>(payload.data()), payload.size ());
        auto t4 = std::chrono::high_resolution_clock::now();

        decoding_timer += std::chrono::duration_cast<std::chrono::microseconds>(t4-t3);

        //This does not work well as these could be linearly dependent coded messages
//        while(tester.RLNC_msg_received_call_count != i+1) {}
//        assert(tester.RLNC_msg_received_call_count == i+1);
//        assert(tester.recv_msg == message2);

        delete temp_msg;
    }

    mchandler.close();

    CPPDEBUG("testRepeated: coding_timer per iteration: " <<
             coding_timer.count() /iterations << " microseconds " << std::endl);
    CPPDEBUG("testRepeated: decoding_timer per iteration: " <<
             decoding_timer.count() /iterations << " microseconds " << std::endl);
}

int main() {

    testFirstIndex ();
    testSecondIndex ();
    testUndecodable ();
    testUnnecessary ();
    testExtraPacket ();
    testNETIDUNASSIGNED();
    testThrowPktNotUnique();
    testChecksum();
    testRepeated ();

    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;
}
