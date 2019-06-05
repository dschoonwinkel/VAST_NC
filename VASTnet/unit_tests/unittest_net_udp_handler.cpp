#include "net_udp_handler.h"
#include <iostream>
#include <assert.h>
#include "absnet_udp_testimpl.h"
#include <boost/asio.hpp>
#include "VASTnet.h"
#include "VASTBuffer.h"
#include "string.h"
#include <cstring>
#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>

size_t sleep_time = 200;

void testProcessInput()
{
    std::cout << "\n\nrunning testProcessInput" << std::endl;
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

    Vast::IPaddr addr1("127.0.0.1", 1037);

    std::string remote_ip = "127.0.0.1";
    uint16_t remote_port = 1037;
    ip::udp::endpoint local_endpoint = ip::udp::endpoint(
                ip::address::from_string(remote_ip), remote_port);

    Vast::net_udp_handler handler(local_endpoint);
    Vast::absnet_udp_testimpl tester;
    io_service ios;
    handler.open (&ios, &tester, true);
    handler.process_input (buf2.data, buf2.size, addr1);

    std::cout << tester.message << std::endl;
    std::cout << std::string(buf2.data, buf2.size) << std::endl;
    assert(tester.message == std::string(buf2.data, buf2.size));

    handler.process_input (buf1.data, buf1.size, Vast::IPaddr());
    assert(tester.message == std::string(buf1.data, buf1.size));

    boost::this_thread::sleep_for (boost::chrono::milliseconds(sleep_time));
}

int main()
{
//    runUnitTest1();
    testProcessInput();

    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;

    return 0;
}
