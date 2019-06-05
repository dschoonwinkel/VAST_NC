#ifndef ABSTRACT_SENDER_H
#define ABSTRACT_SENDER_H

#include <boost/asio.hpp>

using namespace boost::asio;

class abstract_sender
{
public:
    virtual ~abstract_sender() {}
    virtual size_t send (const char *buf, size_t n, ip::udp::endpoint remote_endpoint) = 0;
};

#endif // ABSTRACT_SENDER_H
