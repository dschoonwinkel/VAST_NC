#include <iostream>
#include <assert.h>
#include <string.h>
#include "net_udpnc_mchandler.h"
#include "abstract_rlnc_msg_receiver_testimpl.h"

void runUnitTest1()
{
    std::cout << "Not implemented yet!" << std::endl;
    std::abort();
}

void test_process_encoded()
{
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

    msg1.serialize (buffer);

    mchandler.handle_buffer (buffer, msg1.sizeOf ());

    assert(mchandler.getPacketPoolSize () == 1);

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
