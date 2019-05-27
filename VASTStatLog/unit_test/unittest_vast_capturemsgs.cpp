#include "vast_capturemsgs.h"
#include <iostream>
#include <string>
#include <assert.h>

int main()
{
    std::string str1("Hello123");
    Vast::Message msg1(123, str1.c_str(), str1.length(), true, 14590678);
    msg1.addTarget(1451234);
    std::cout << "Saved Message " << std::endl << msg1 << std::endl;
    Vast::vast_capturemsgs::saveVASTMessage(0, msg1, 123);

    Vast:: MessageWrapper msg2 = Vast::vast_capturemsgs::restoreVASTMessage("./logs/VASTmsgscap_N123_0.txt");

    std::cout << "restored Message" << std::endl << msg2 << std::endl;

    assert(msg2._msg.contentEquals(msg1));

    return 0;
}
