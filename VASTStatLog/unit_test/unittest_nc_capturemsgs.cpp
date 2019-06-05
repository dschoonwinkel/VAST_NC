#include "netudpnc_capturemsgs.h"
#include <iostream>
#include <string>
#include <assert.h>
#include <boost/asio.hpp>
#include <array>

using namespace boost::asio;
using namespace Vast;

int main()
{
    std::vector<NCWrapper> allMessagesSent;

    std::string str1("Hello123");
    Message msg1(123, 0, str1.c_str(), str1.length(), true, 14590678);
    msg1.addTarget(1451234);
    std::vector<char> buf_array(msg1.sizeOf());
    buf_array.assign(0, msg1.sizeOf());
    msg1.serialize(buf_array.data());
    std::string buffer(buf_array.data(), msg1.sizeOf());
    ip::udp::endpoint remote_endpoint(ip::address::from_string("127.0.0.1"), 1037);
    ip::udp::endpoint local_endpoint(ip::address::from_string("127.0.0.2"), 1037);
    NCWrapper wrapper(0, buffer, remote_endpoint, local_endpoint);

    std::cout << "Saved Message " << std::endl << wrapper << std::endl;
    allMessagesSent.push_back(wrapper);
    netudpnc_capturemsgs::saveNCMessage(0, buffer, remote_endpoint, local_endpoint, 123);

    for (size_t i = 1; i <= 10; i++)
    {
        std::string str = std::string("Hello") + std::to_string(i);
        Message msg(i, 0, str.c_str(), str.length(), true, 14590678+i);
        msg.addTarget(1451234+i);

        std::vector<char> buf_array(msg.sizeOf());
        buf_array.assign(0, msg.sizeOf());
        msg.serialize(buf_array.data());
        std::string buffer(buf_array.data(), msg.sizeOf());

        ip::udp::endpoint remote_endpoint(ip::address::from_string("127.0.0.1"), 1037);
        ip::udp::endpoint local_endpoint(ip::address::from_string("127.0.0.2"), 1037);
        NCWrapper wrapper(i, buffer, remote_endpoint, local_endpoint);

//        std::cout << "Saved Message " << std::endl << wrapper << std::endl;
        allMessagesSent.push_back(wrapper);
        netudpnc_capturemsgs::saveNCMessage(i, buffer, remote_endpoint, local_endpoint, 123);
    }

    netudpnc_capturemsgs restoring("./logs/msgcap/NCmsgscap_N123_0.txt");

    for (size_t i = 0; i <= 10; i++)
    {
        NCWrapper restored_wrapper;
        bool success = restoring.restoreNextNCMessage(i+1, restored_wrapper);

        assert(success);
//        std::cout << "restored Message" << std::endl << restored_wrapper._buffer << std::endl;
//        std::cout << "restored remote Endpoint" << std::endl << restored_wrapper._remote_endpoint << std::endl;
//        std::cout << "restored local Endpoint" << std::endl << restored_wrapper._local_endpoint << std::endl;
        std::cout << "Saved message" << std::endl;
        std::cout << allMessagesSent[i] << std::endl;
        std::cout << "Restored message" << std::endl;
        std::cout << restored_wrapper << std::endl;

        if (!(restored_wrapper == allMessagesSent[i]))
        {
            std::cout << "timestamp: " << (restored_wrapper.timestamp == allMessagesSent[i].timestamp) << std::endl;
            std::cout << "buffer: " << (restored_wrapper._buffer == allMessagesSent[i]._buffer) << std::endl;
            std::cout << "remote_endpoint: " << (restored_wrapper._remote_endpoint == allMessagesSent[i]._remote_endpoint) << std::endl;
            std::cout << "local_endpoint: " << (restored_wrapper._local_endpoint == allMessagesSent[i]._local_endpoint) << std::endl;
        }

        assert(restored_wrapper == allMessagesSent[i]);
    }
    return 0;
}
