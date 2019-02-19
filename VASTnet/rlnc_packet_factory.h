#ifndef RLNC_PACKET_FACTORY_H
#define RLNC_PACKET_FACTORY_H

#define START_BITS 0xD
#define END_BITS 0xE

#include <stdint.h>
#include <vector>
#include "VASTTypes.h"

typedef size_t packetid_t;

// header used by all RLNC messages
typedef struct
{
    uint32_t start      : 4;     // start marker (number 0xD: indicates 1101, used to differentiate from VASTHeader)
    uint32_t ordering   : 8;     // order of packet in stream
    uint32_t packetsize : 16;    // size of message (max: 65535 bytes - header bytes, same size as UDP payload)
    uint32_t gensize    : 4;    //Showing how many packets are in this generation calculated in 2^gensize

    uint32_t generation : 8;
    uint32_t enc_packet_count : 8;
    uint32_t padding : 12;
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

        header.start = START_BITS;
        header.end = END_BITS;

        return header;
    }

};

bool isRLNCHeader(RLNCHeader header)
{
    if (header.start == START_BITS && header.end == END_BITS)
        return true;
    else
        return false;
}

class RLNCMessage
{
public:
    RLNCMessage()
    {

    }

    RLNCMessage (RLNCHeader header)
    {
        this->header = header;
        header.enc_packet_count = 0;
    }

    std::vector<packetid_t> getPacketIds()
    {
        return pkt_ids;
    }

    void putPacketId(packetid_t pkt_id)
    {
        pkt_ids.push_back(pkt_id);
        header.enc_packet_count = pkt_ids.size();
    }

    void putMessage(uint8_t* buffer, size_t len)
    {
        msg = new uint8_t[len];
        memcpy(msg, buffer, len);
        header.packetsize = len;
    }

    uint8_t* getMessage(size_t len = 0)
    {
        //If I just want to read it, return the pointer
        if (len == 0)
        {
            return msg;
        }
        //If I want to write to it, assure that there is enough space
        msg = new uint8_t[len];
        header.packetsize = len;
        return msg;
    }

    // size of this class, must be implemented
    size_t sizeOf ()
    {
        size_t size = sizeof(RLNCHeader);
        size += header.enc_packet_count * sizeof(packetid_t);
        size += header.packetsize;

        return size;
    }

    // store into a buffer (assuming the buffer has enough space)
    // buffer can be NULL (simply to query the total size)
    // returns the total size of the packed class
    size_t serialize (char *buffer)
    {
        size_t n = 0;
        if (buffer == NULL)
            return sizeOf();

        memcpy(buffer+n, &header, sizeof(RLNCHeader));
        n += sizeof(RLNCHeader);

        for (int i = 0; i < header.enc_packet_count; i++)
        {
            memcpy(buffer+n, &pkt_ids[i], sizeof(packetid_t));
            n += sizeof(packetid_t);
        }
        memcpy(buffer+n, msg, header.packetsize);

        return n;

    }

    // restores a buffer into the object
    // returns number of bytes restored (should be > 0 if correct)
    size_t deserialize (const char *buffer, size_t size)
    {
        size_t n = 0;
        if (size > sizeof(RLNCHeader) && buffer != NULL)
        {
            memcpy(&header, buffer+n, sizeof(RLNCHeader));
            n += sizeof(RLNCHeader);

            packetid_t pkt_id = -1;
            for (int i = 0; i < header.enc_packet_count; i++)
            {
                memcpy(&pkt_id, buffer+n, sizeof(packetid_t));
                putPacketId(pkt_id);
                n += sizeof(packetid_t);
            }
            msg = new uint8_t[header.packetsize];
            memcpy(msg, buffer+n, header.packetsize);

            return n;
        }
        else
        {
            return 0;
        }

        return 0;
    }

private:
    RLNCHeader header;
    std::vector<packetid_t> pkt_ids;
    uint8_t* msg;

};

#endif // RLNC_PACKET_FACTORY_H
