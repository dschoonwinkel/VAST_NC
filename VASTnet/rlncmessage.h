#ifndef RLNCMESSAGE_H
#define RLNCMESSAGE_H

#define MAX_SYMBOLS 2
#define MAX_PACKET_SIZE 500
#define GENSIZE 1
#define MAX_GEN_NUM 255

#define COMPUTE_RLNC

#include "rlnc_packet_factory.h"
#include <VASTTypes.h>
#include <boost/asio.hpp>

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
    const std::vector<Vast::IPaddr> getToAddrs();

    void putPacketId(packetid_t pkt_id, bool autonumber = true);
    void putFromId(Vast::id_t from_id);
    void putToAddr(Vast::IPaddr addr);
    void putIdsAddr(packetid_t pkt_id, Vast::id_t from_id, Vast::IPaddr addr, bool autonumber = true);


    void putMessage(const char* buffer, size_t len);

    const char* getMessage();
    const uint8_t* getMessageU();
    size_t getMessageSize();

    void putOrdering(uint8_t ordering);
    uint8_t getOrdering();

    uint32_t getChecksum();
    void setChecksum(uint32_t checksum);

    size_t sizeOf ();
    size_t serialize (char *buffer);
    //Returns -1 on error
    int deserialize(const char *buffer, size_t size);

    bool operator==(const RLNCMessage other) const;
    bool contentEquals(const RLNCMessage other) const;

    packetid_t static generatePacketId(Vast::id_t id, int ordering);

    uint32_t static generateChecksum(const uint8_t *buffer, size_t bufsize);
    uint32_t static generateChecksum(const char *buffer, size_t bufsize);

    friend std::ostream& operator<<(std::ostream&, RLNCMessage const& message);

    //Only used for storing remote address in Concurrent queue. Not serialized
    Vast::IPaddr socket_addr;

private:
    RLNCHeader header;
    std::vector<packetid_t> pkt_ids;
    std::vector<Vast::id_t> from_ids;
    std::vector<Vast::IPaddr> to_addrs;
    std::string msg;
    uint32_t checksum = 0;


};

#endif // RLNCMESSAGE_H
