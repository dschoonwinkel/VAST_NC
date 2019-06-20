#include "VASTnet.h"
#include <assert.h>
#include <cstring>
#include <arpa/inet.h>
#include <string>

void testGetInterfaceFromIP()
{
    system("ifconfig eth0:0 10.0.0.2");
    std::array<char, 16> result_IP;
    char IP_string[] = "10.0.0.1";
    bool success = Vast::VASTnet::getInterfaceAddrFromRemoteAddr(result_IP, IP_string);

    assert(success);
    std::cout << "ResultIP: " << std::string(result_IP.data()) << std::endl;

    assert(strncmp(result_IP.data(), "10.0.0.2", 8) == 0);

}

int main()
{
    testGetInterfaceFromIP ();

    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;


    return 0;
}
