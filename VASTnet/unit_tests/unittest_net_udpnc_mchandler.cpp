#include <iostream>
#include <assert.h>
#include <string.h>
#include "net_udpnc_mchandler.h"
#include "abstract_rlnc_msg_receiver_testimpl.h"
#include "rlncrecoder.h"

void runUnitTest1()
{
    std::cout << "Not implemented yet!" << std::endl;
    std::abort();
}

void test_process_encoded()
{
    std::cout << "test_process_encoded() " << std::endl;

    Vast::net_udpNC_MChandler mchandler;
    Vast::AbstractRNLCMsgReceiverTestImpl tester;

    RLNCHeader_factory factory1;

    RLNCHeader header1 = factory1.build ();
    RLNCMessage msg1(header1);

    std::string hello = "Hello World!";
    msg1.putMessage (hello.c_str (), hello.length());

    char buffer[1000];

    msg1.serialize (buffer);

    //Setting a route for 239.255.0.1 is required to open mc socket
    system("sudo route add 239.255.0.1 enp0s3");
    mchandler.open (&tester);

    mchandler.handle_buffer (buffer, msg1.sizeOf ());

    //The message has no PktIds, assure that it is not added to the decoder incorrectly
    assert(mchandler.getPacketPoolSize () == 0);

    msg1.putPacketId (123);
    msg1.putFromId (654);
    Vast::IPaddr addr1("127.0.0.1", 1037);
    msg1.putToAddr (addr1);

    msg1.serialize (buffer);

    mchandler.handle_buffer (buffer, msg1.sizeOf ());

    assert(mchandler.getPacketPoolSize () == 1);

    //Create an encoded packet, put it in the processor


    RLNCHeader header2 = factory1.build ();
    RLNCMessage msg2(header2);

    std::string bye = "Goodbye world!";
    msg2.putMessage (hello.c_str (), hello.length());

    msg2.putPacketId (321);
    msg2.putFromId (456);
    Vast::IPaddr addr2("127.0.0.2", 1037);
    msg2.putToAddr (addr2);

//    msg2.serialize (buffer);

    RLNCrecoder recoder;

    recoder.addRLNCMessage(msg1);
    recoder.addRLNCMessage(msg2);

    RLNCMessage *temp_msg = recoder.produceRLNCMessage();

    if (!temp_msg)
    {
        std::cerr << "Could not produce RLNC encoded message" << std::endl;
        std::abort();
    }

    std::array<uint8_t, 1400> payload;
    payload.fill (0);
    temp_msg->serialize(reinterpret_cast<char*>(payload.data()));
    //Equivalent to receive
    mchandler.handle_buffer(reinterpret_cast<char*>(payload.data()), payload.size ());

    assert(tester.RLNC_msg_received_call_count == 1);
    assert(tester.recv_msg == msg2);
}

int main()
{
//    runUnitTest1();
    test_process_encoded ();

    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;


    return 0;
}
