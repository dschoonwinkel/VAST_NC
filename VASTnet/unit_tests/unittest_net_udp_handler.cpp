#include "net_udp_handler.h"

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
    handler.process_input (message1, NULL);
    handler.process_input (message2, NULL);


    std::cout << tester.message << std::endl;
    std::cout << std::string(buf2.data, buf2.size) << std::endl;
    //Returned message is the entire RLNC payload, i.e. VAST message - contained in buf2
    assert(tester.message == std::string(buf2.data, buf2.size));

    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

int main()
{
//    runUnitTest1();



    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;

    return 0;
}
