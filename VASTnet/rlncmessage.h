#ifndef RLNCMESSAGE_H
#define RLNCMESSAGE_H

#define MAX_SYMBOLS 2
#define MAX_PACKET_SIZE 500
#define GENSIZE 1
#define MAX_GEN_NUM 255

#include "rlnc_packet_factory.h"
#include <VASTTypes.h>

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
    friend std::ostream& operator<<(std::ostream&, RLNCMessage const& log);

private:
    RLNCHeader header;
    std::vector<packetid_t> pkt_ids;
    char* msg;

};

#endif // RLNCMESSAGE_H
