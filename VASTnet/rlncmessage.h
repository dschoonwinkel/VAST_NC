#ifndef RLNCMESSAGE_H
#define RLNCMESSAGE_H

#include "rlnc_packet_factory.h"

class RLNCMessage
{
public:
    RLNCMessage();

    RLNCMessage (RLNCHeader header);

    std::vector<packetid_t> getPacketIds();

    void putPacketId(packetid_t pkt_id);

    void putMessage(const char* buffer, size_t len);

    char* getMessage(size_t len = 0);

    size_t sizeOf ();
    size_t serialize (char *buffer);
    size_t deserialize (const char *buffer, size_t size);

private:
    RLNCHeader header;
    std::vector<packetid_t> pkt_ids;
    char* msg;

};

#endif // RLNCMESSAGE_H
