#include "rlncmessage.h"
#include <algorithm>
#include <boost/format.hpp>

RLNCMessage::RLNCMessage()
{

}

RLNCMessage::RLNCMessage (RLNCHeader header)
{
    this->header = header;
    header.enc_packet_count = 0;
}

std::vector<packetid_t> RLNCMessage::getPacketIds()
{
    return pkt_ids;
}

void RLNCMessage::putPacketId(packetid_t pkt_id)
{
    pkt_ids.push_back(pkt_id);
    header.enc_packet_count = pkt_ids.size();
}

void RLNCMessage::putMessage(const char* buffer, size_t len)
{
    msg = new char[len];
    memcpy(msg, buffer, len);
    header.packetsize = len;
}

char* RLNCMessage::getMessage(size_t len)
{
    //If I just want to read it, return the pointer
    if (len == 0)
    {
        return msg;
    }
    //If I want to write to it, assure that there is enough space
    msg = new char[len];
    header.packetsize = len;
    return msg;
}

// size of this class, must be implemented
size_t RLNCMessage::sizeOf ()
{
    size_t size = sizeof(RLNCHeader);
    size += header.enc_packet_count * sizeof(packetid_t);
    size += header.packetsize;

    return size;
}

// store into a buffer (assuming the buffer has enough space)
// buffer can be NULL (simply to query the total size)
// returns the total size of the packed class
size_t RLNCMessage::serialize (char *buffer)
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
    n += header.packetsize;

    return n;

}

// restores a buffer into the object
// returns number of bytes restored (should be > 0 if correct)
size_t RLNCMessage::deserialize (const char *buffer, size_t size)
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
        msg = new char[header.packetsize];
        memcpy(msg, buffer+n, header.packetsize);

        return n;
    }
    else
    {
        return 0;
    }

    return 0;
}
std::ostream& operator<<(std::ostream& output, RLNCMessage const& message )
{
        output << "RLNCMessage::stream >> output: ******************************\n";

        output << "******************************************************" << std::endl;
        return output;
}
