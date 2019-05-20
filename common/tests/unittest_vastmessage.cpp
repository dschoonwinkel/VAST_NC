#include <iostream>
#include "logger.h"
#include <chrono>
#include <thread>
#include <assert.h>
#include "VASTTypes.h"

void testEquals()
{
    std::cout << "testEquals()" << std::endl;
    Vast::Message msg1(0);
    Vast::Message msg2(0);

    assert(msg1 == msg2);

    msg2.clear(1);

    assert(!(msg1 == msg2));

    msg1.from = 123;
    msg1.size = 1;
    msg1.msgtype = 1;
    msg1.msggroup = 3;
    msg1.priority = 7;
    msg1.reliable = true;

    msg2.from = 123;
    msg2.size = 1;
    msg2.msgtype = 1;
    msg2.msggroup = 3;
    msg2.priority = 7;
    msg2.reliable = true;

    assert(msg1 == msg2);

    msg2.priority = 3;

    assert(!(msg1 == msg2));

    Vast::IPaddr addr1(10, 1037);
    Vast::IPaddr addr2(12, 1037);

    msg2.priority = msg1.priority;

    msg1.store(addr1);
    msg2.store(addr1);

    assert(msg1 == msg2);

    msg2.store(addr2);
    assert(!(msg1 == msg2));

}

int main()
{
    testEquals();

    std::cout << "****************" << std::endl;
    std::cout << "All tests passed" << std::endl;
    std::cout << "****************" << std::endl;

}
