#ifndef RLNC_PACKET_FACTORY_H
#define RLNC_PACKET_FACTORY_H

// header used by all RLNC messages
typedef struct
{
    uint32_t start      : 4;     // start marker (number 0xD: indicates 1101, used to differentiate from VASTHeader)
    uint32_t ordering   : 8;     // order of packet in stream
    uint32_t packetsize : 16;    // size of message (max: 65535 bytes - header bytes, same size as UDP payload)
    uint32_t gensize    : 4;    //Showing how many packets are in this generation calculated in 2^gensize

    uint32_t generation : 8;
    uint32_t packet_id  : 20;
    uint32_t end        : 4;     // end marker (number 0xE: 1110)
} RLNCHeader;

class RLNCHeader_factory
{
public:
    RLNCHeader_factory()
    {

    }

    RLNCHeader build()
    {
        RLNCHeader header;

        header.start = 0xD;
        header.end = 0xE;

        return header;
    }

};

#endif // RLNC_PACKET_FACTORY_H
