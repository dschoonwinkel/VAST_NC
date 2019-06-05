#ifndef ABSTRACT_SENDER_IMPL_H
#define ABSTRACT_SENDER_IMPL_H

#include <abstract_sender.h>
#include "abstract_input_processor.h"

class abstract_sender_impl : public abstract_sender
{
public:

    abstract_sender_impl(abstract_input_processor* processor)
    {
        _processor = processor;
    }

    size_t send (const char *buf, size_t n, ip::udp::endpoint remote_endpoint)
    {
//        messages_to_send.push_back(std::string(buf, n));
        CPPDEBUG("abstract_sender_impl::send called" << std::endl);
        _processor->process_input(buf, remote_endpoint, n);

        return 0;
    }

    std::vector<std::string> messages_to_send;
    abstract_input_processor* _processor = NULL;

};

#endif // ABSTRACT_SENDER_IMPL_H
