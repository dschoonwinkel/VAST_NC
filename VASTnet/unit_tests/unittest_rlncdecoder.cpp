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

void testFirstIndex()
{
    std::cout << "\n\ntestFirstIndex" << std::endl;
    Vast::net_udpNC_MChandler mchandler;
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester);

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
    mchandler.handle_buffer(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.handle_buffer (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 1);
    assert(tester.recv_msg == message2);

}

void testSecondIndex()
{
    std::cout << "\n\ntestSecondIndex" << std::endl;
    Vast::net_udpNC_MChandler mchandler;
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester);

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
    mchandler.handle_buffer(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.handle_buffer (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 1);
    assert(tester.recv_msg == message1);
}

void testUndecodable()
{
    std::cout << "\n\ntestUndecodable" << std::endl;
    Vast::net_udpNC_MChandler mchandler;
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester);

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
    mchandler.handle_buffer (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 0);
    assert(mchandler.getPacketPoolSize () == 0);
}

void testUnnecessary()
{
    std::cout << "\n\ntestUnnecessary" << std::endl;
    Vast::net_udpNC_MChandler mchandler;
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester);

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
    mchandler.handle_buffer(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    message2.serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.handle_buffer(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.handle_buffer (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 0);
}

void testExtraPacket()
{
    std::cout << "\n\ntestExtraPacket" << std::endl;
    Vast::net_udpNC_MChandler mchandler;
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester);

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
    mchandler.handle_buffer(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.handle_buffer (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 1);
    assert(tester.recv_msg == message2);


    recoder.addRLNCMessage (message1);
    temp_msg = recoder.produceRLNCMessage();

    if (!temp_msg)
    {
        std::cerr << "Could not produce RLNC encoded message" << std::endl;
        exit(EXIT_FAILURE);
    }

    payload.fill (0);
    message3.serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.handle_buffer(reinterpret_cast<char*>(payload.data()), payload.size ());

    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.handle_buffer (reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 2);
    assert(tester.recv_msg == message1);


}

void testRepeated()
{
    std::cout << "testRepeated" << std::endl;
    srand(time(NULL));

    Vast::net_udpNC_MChandler mchandler;
    Vast::AbstractRNLCMsgReceiverTestImpl tester;
    mchandler.open(&tester);

    for (int i = 0; i < 1000000; i++)
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

//        std::cout << "Symbol 1:" << message1 << std::endl;
//        std::cout << "Symbol 2:" << message2 << std::endl;

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
        mchandler.handle_buffer(reinterpret_cast<char*>(payload.data()), payload.size ());

        payload.fill (0);
        temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
        //Equivalent to receive
        mchandler.handle_buffer (reinterpret_cast<char*>(payload.data()), payload.size ());

//        assert(tester.RLNC_msg_received_call_count == 1);
//        assert(tester.recv_msg == message2);


        delete temp_msg;
    }
}

int main() {

    testFirstIndex ();
    testSecondIndex ();
    testUndecodable ();
    testUnnecessary ();
    testExtraPacket ();
//    testRepeated ();

    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;
}
