#include "vast_capturemsgs.h"
#include <iostream>
#include <string>
#include <assert.h>

using namespace Vast;

int main()
{
    std::vector<Message> allMessagesSent;

    std::string str1("Hello123");
    Message msg1(123, str1.c_str(), str1.length(), true, 14590678);
    msg1.addTarget(1451234);
    std::cout << "Saved Message " << std::endl << msg1 << std::endl;
    vast_capturemsgs::saveVASTMessage(0, msg1, 123);
    allMessagesSent.push_back(msg1);

    for (size_t i = 1; i <= 10; i++)
    {
        std::string str1 = std::string("Hello") + std::to_string(i);
        Message msg(i, str1.c_str(), str1.length(), true, 14590678+i);
        msg.addTarget(1451234+i);
        std::cout << "Saved Message " << std::endl << msg << std::endl;
        vast_capturemsgs::saveVASTMessage(i, msg, 123);
        allMessagesSent.push_back(msg);
    }

    vast_capturemsgs restoring("./logs/msgcap/VASTmsgscap_N123_0.txt");

    for (size_t i = 0; i <= 10; i++)
    {
        Message restoredmsg(0);
        bool success = restoring.restoreNextVASTMessage(i+1, restoredmsg);
        assert(success);
        std::cout << "restored Message" << std::endl << restoredmsg << std::endl;
        assert(restoredmsg.contentEquals(allMessagesSent[i]));
    }





    return 0;
}
