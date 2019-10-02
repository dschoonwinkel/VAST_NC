#include <iostream>
#include <assert.h>
#include <string.h>
#include "net_udpnc_mchandler.h"
#include "abstract_udpnc_msg_receiver_testimpl.h"
#include "udpncrecoder.h"
#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>

size_t sleep_time = 200;

void runUnitTest1()
{
    std::cout << "Not implemented yet!" << std::endl;
    std::abort();
}

void onCloseWait()
{
    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

void test_process_encoded()
{
    std::cout << "\n\ntest_process_encoded() " << std::endl;

    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_MChandler mchandler(local_endpoint);
    
    Vast::AbstractUDPNCMsgReceiverTestImpl tester;

    UDPNCHeader_factory factory1;

    UDPNCHeader header1 = factory1.build ();
    UDPNCMessage msg1(header1);

    std::string hello = "Hello World!";
    msg1.putMessage (hello.c_str (), hello.length());

    char buffer[1000];

    msg1.serialize (buffer);

    //Setting a route for 239.255.0.1 is required to open mc socket
    system("sudo route add 239.255.0.1 enp0s3");
    mchandler.open (&tester, NULL, false);

    mchandler.process_input(buffer, msg1.sizeOf ());

    //The message has no PktIds, assure that it is not added to the decoder incorrectly
    assert(mchandler.getPacketPoolSize () == 0);

    msg1.putPacketId (123);
    msg1.putFromId (654);
    Vast::IPaddr addr1("127.0.0.1", 1037);
    msg1.putToAddr (addr1);

    msg1.serialize (buffer);

    mchandler.process_input(buffer, msg1.sizeOf ());

    assert(mchandler.getPacketPoolSize () == 1);

    //Create an encoded packet, put it in the processor


    UDPNCHeader header2 = factory1.build ();
    UDPNCMessage msg2(header2);

    std::string bye = "Goodbye world!";
    msg2.putMessage (hello.c_str (), hello.length());

    msg2.putPacketId (321);
    msg2.putFromId (456);
    Vast::IPaddr addr2("127.0.0.2", 1037);
    msg2.putToAddr (addr2);

//    msg2.serialize (buffer);

    UDPNCrecoder recoder;

    recoder.addUDPNCMessage(msg1);
    recoder.addUDPNCMessage(msg2);

    std::shared_ptr<UDPNCMessage> temp_msg = recoder.produceUDPNCMessage();

    if (!temp_msg)
    {
        std::cerr << "Could not produce UDPNC encoded message" << std::endl;
        std::abort();
    }

    std::array<uint8_t, 1400> payload;
    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.process_input(reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.UDPNC_msg_received_call_count == 1);
    assert(tester.recv_msg == msg2);

    onCloseWait();
}

void test_toAddrForMe()
{
    std::cout << "\n\ntest_toAddrForMe() " << std::endl;

    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udpNC_MChandler mchandler(local_endpoint);

    UDPNCHeader_factory factory1;

    UDPNCHeader header1 = factory1.build ();
    UDPNCMessage msg1(header1);

    std::string hello = "Hello World!";
    msg1.putMessage (hello.c_str (), hello.length());

    Vast::IPaddr addr1("127.0.0.1", 1037);
    Vast::IPaddr addr2("127.0.0.2", 1037);
    Vast::IPaddr addr3("127.0.0.3", 1037);

    msg1.putPacketId (123);
    msg1.putFromId (654);
    msg1.putToAddr (addr1);

    std::cout << "Single addr in packet: " << std::endl;
    assert(mchandler.toAddrForMe (msg1) == true);

    UDPNCHeader header2 = factory1.build ();
    UDPNCMessage msg2(header2);

    std::string bye = "Goodbye world!";
    msg2.putMessage (bye.c_str (), bye.length());

    msg2.putPacketId (123);
    msg2.putFromId (654);
    msg2.putToAddr (addr1);
    msg2.putPacketId (321);
    msg2.putFromId (456);
    msg2.putToAddr (addr2);

    std::cout << "Two addrs in packet: " << std::endl;
    assert(mchandler.toAddrForMe (msg2) == true);

    UDPNCHeader header3 = factory1.build ();
    UDPNCMessage msg3(header3);

    std::string OK = "OK world!";
    msg3.putMessage (OK.c_str (), OK.length());

    msg3.putPacketId (123);
    msg3.putFromId (654);
    msg3.putToAddr (addr2);
    msg3.putPacketId (321);
    msg3.putFromId (456);
    msg3.putToAddr (addr3);

    std::cout << "Two irrelevant addrs in packet: " << std::endl;
    assert(mchandler.toAddrForMe (msg3) == false);

    onCloseWait();
}

int main()
{
//    runUnitTest1();
    test_process_encoded ();
    test_toAddrForMe();

    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;


    return 0;
}
