#ifndef ABSTRACT_INPUT_PROCESSOR_H
#define ABSTRACT_INPUT_PROCESSOR_H

class abstract_input_processor {

public:
    virtual ~abstract_input_processor() {}
    virtual void process_input (const char *buf, ip::udp::endpoint remote_endpoint,
                      std::size_t bytes_transferred) = 0;

};

#endif // ABSTRACT_INPUT_PROCESSOR_H
