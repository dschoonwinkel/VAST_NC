#include "VASTTypes.h"
#include <assert.h>
#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>
#include <string>


int main()
{
    std::string filename("test1.txt");

    char data[] = "Hello123";
    std::string str1(data);
//    (msgtype_t type, const char *msg, size_t msize, bool alloc = true, id_t sender = 0)
    Vast::Message msg1(123, data, str1.length(), true, 14590678);
    msg1.addTarget(14842);
    std::cout << "msg1: " << msg1 << std::endl;

    std::ofstream ofs(filename);
    if (!ofs.is_open())
    {
        std::cerr << "vast_capturemsgs::constructor file open : " << (ofs.is_open() ? "true":"false") << std::endl << "EXITING" <<std::endl;
        exit(EXIT_FAILURE);
    }

    boost::archive::text_oarchive ar(ofs);
    msg1.serialize(ar, 0);
    ofs.flush();

    Vast::Message msg3(123, data, str1.length(), true, 14590678);
    msg3.addTarget(11111);
    msg3.serialize(ar, 0);
    ofs.flush();

    std::ifstream ifs(filename);
    boost::archive::text_iarchive ar2(ifs);
    Vast::Message msg2(0);
    msg2.serialize(ar2, 0);

    std::cout << "msg1 restored " << std::endl << msg2 << std::endl;

    assert(msg1.contentEquals(msg2));

    msg2.serialize(ar2, 0);
    std::cout << "msg3 restored " << std::endl << msg2 << std::endl;

    ofs.close();
}
