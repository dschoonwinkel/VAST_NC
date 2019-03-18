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
    //Copy constructor
    RLNCMessage (const RLNCMessage &message);

    const std::vector<packetid_t> getPacketIds();
    const std::vector<Vast::id_t> getFromIds();
    Vast::id_t getFirstFromId();

    void putPacketId(packetid_t pkt_id, bool autonumber = true);
    void putFromId(Vast::id_t from_id);

    void putMessage(const char* buffer, size_t len);

    char* getMessage(size_t len = 0);
    uint8_t* getMessageU(size_t len = 0);
    size_t getMessageSize();

    void putOrdering(uint8_t ordering);
    uint8_t getOrdering();

    size_t sizeOf ();
    size_t serialize (char *buffer);
    //Returns -1 on error
    int deserialize(const char *buffer, size_t size);

    bool operator==(const RLNCMessage other);

    packetid_t static generatePacketId(Vast::id_t id, int ordering);

    friend std::ostream& operator<<(std::ostream&, RLNCMessage const& log);

private:
    RLNCHeader header;
    std::vector<packetid_t> pkt_ids;
    std::vector<Vast::id_t> from_ids;
    char* msg;

};

#endif // RLNCMESSAGE_H
