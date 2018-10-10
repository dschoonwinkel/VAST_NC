#include "vaststatlog.h"
#include "VASTVerse.h"
#include <climits>
#include <stdio.h>

#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace Vast {

    VASTStatLog::VASTStatLog(VASTVerse *world, VAST *client)
        : clientNode()
    {
        _world = world;
        _client = client;

        _steps_recorded = 0;
        _min_aoi = LONG_MAX;
        _total_aoi = 0;
        _max_CN = _total_CN = 0;

        if (_world != NULL)
            _world->clearStat();
    }

    void VASTStatLog::recordStat()
    {


        if (_client == NULL)
        {
            std::cerr << "vaststatlog::recordStat() _client was NULL " << std::endl;
            return;
        }

        clientNode = *(_client->getSelf());

        // record keeping
        long aoi = clientNode.aoi.radius;

        _total_aoi += aoi;

        if (aoi < _min_aoi)
            _min_aoi = aoi;


        // set up neighbor view
        _neighbors= _client->list ();
        neighbors_size = _neighbors.size();
        clientIsRelay = _client->isRelay();

        worldConnSize = _world->getConnectionSize();

        _total_CN += worldConnSize;
        if (_max_CN < worldConnSize)
            _max_CN = worldConnSize;

        worldSendStat = _world->getSendStat();
        worldRecvStat = _world->getReceiveStat();

        worldIsGateway = _world->isGateway();
        worldIsMatcher = _world->isMatcher();

        _steps_recorded++;
    }

    void VASTStatLog::printStat ()
    {
        printf("VASTStatLog::printStat: ******************************\n");
        printf("Timestamp %lu\n", timestamp);
        printf("My Node ID %lx\n", clientNode.id);
        if (_client != NULL)
            printf("My Sub ID %lu\n", _client->getSubscriptionID());

        printf("My Node host_id %lx\n", clientNode.id);
        printf("My Node lasttime %lu\n", clientNode.time);
        printf("My Node AOI x corrd %2.2lf\n", clientNode.aoi.center.x);
        printf("My Node AOI y corrd %2.2lf\n", clientNode.aoi.center.y);
        printf("My Node AOI z corrd %2.2lf\n", clientNode.aoi.center.z);
        printf("My Node AOI %d\n", clientNode.aoi.radius);

        printf("My Node Addr host ID %lx\n", clientNode.addr.host_id);
        printf("My Node Addr last acc %lu\n", clientNode.addr.lastAccessed);
        printf("My Node Addr IP host %x\n", clientNode.addr.publicIP.host);
        printf("My Node Addr IP port %x\n", clientNode.addr.publicIP.port);

        printf("Num neighbors: %lu\n", neighbors_size);
        printf("My neighbour IDs: \n");
        for (std::vector<Node*>::iterator it = _neighbors.begin(); it != _neighbors.end(); it++)
        {
            printf("%lu\n", (*it)->id);
        }

        printf("Relay: %d \n\n", clientIsRelay);

        printf("\nCN: %u\n", worldConnSize);
        printf("GetSendStat: %f\n", worldSendStat.average);
        printf("GetRecvStat: %f\n\n", worldRecvStat.average);

        printf("GW: %s ", worldIsGateway ? "true" : "false");
        printf("Matcher: %s ", worldIsMatcher ? "true" : "false");


        printf("steps_recorded: %d \n", _steps_recorded);
        printf("******************************************************\n");
    }

    size_t VASTStatLog::sizeOf ()
    {
        size_t total_size = 0;

        total_size+= sizeof(timestamp);
        std::cout << "sizeof timestamp                      " << sizeof(timestamp) << std::endl;

        total_size+= clientNode.sizeOf();
        std::cout << "clientNode->sizeof                    " << clientNode.sizeOf() << std::endl;

        total_size+= _neighbors.size()*sizeof(Node);
        std::cout << "sizeof neighbor Node list             " << _neighbors.size()*sizeof(Node) << std::endl;

        total_size+= sizeof(clientIsRelay);
        std::cout << "sizeof clientIsRelay                  " << sizeof(clientIsRelay) << std::endl;

        total_size+= sizeof(worldConnSize);
        std::cout << "sizeof worldConnSize                  " << sizeof(worldConnSize) << std::endl;

        total_size+= worldSendStat.sizeOf();
        std::cout << "sizeof worldSendStat                  " << worldSendStat.sizeOf() << std::endl;

        total_size+= worldRecvStat.sizeOf();
        std::cout << "sizeof worldRecvStat                  " << worldRecvStat.sizeOf() << std::endl;

        total_size+= sizeof(worldIsGateway);
        std::cout << "sizeof worldIsGateway                 " << sizeof(worldIsGateway) << std::endl;

        total_size+= sizeof(worldIsMatcher);
        std::cout << "sizeof worldIsMatcher                 " << sizeof(worldIsMatcher) << std::endl;

        std::cout << "Total size: " << total_size << "\n" << std::endl;

        /*
         * if (_world.isMatcher()) {
         *  total_size+=_world.getMatcherAOI();
         *  std::cout << "sizeof _world.getMatcherAOI()" << sizeof(_world.getMatcherAOI()) << std::endl;
         * }
        */

        return total_size;
    }

//    void VASTStatLog::serialize (Archive &ar)
//    {



        // NOTE: average is not sent
//        if (p != NULL)
//        {
//            memcpy (p, &timestamp,    sizeof (timestamp));  p += sizeof (timestamp);

//            clientNode.serialize(p);               p += sizeof(clientNode.sizeOf());

//            std::cout << "Size of client Node" << clientNode.sizeOf() << std::endl;
//            std::cout << "sizeof(Node)" << sizeof(Node) << std::endl;

//            neighbors_size = _neighbors.size();
//            std::cout << neighbors_size << std::endl;
//            std::cout << "Client neighbor count " << _client->list().size() << std::endl;
//            memcpy(p, &neighbors_size, sizeof(neighbors_size)); p += sizeof(neighbors_size);

//            for (Node* neighbor : _neighbors)
//            printf("Pointer p in buffer%x\n", p);
//            for (std::vector<Node*>::iterator it=_neighbors.begin(); it < _neighbors.end(); it++)
//            {
////                std::cout << "Serializing Neighbor ID: " << neighbor->id << std::endl;
////                neighbor->serialize(p); p+= neighbor->sizeOf();
//                (*it)->serialize(p); p+= (*it)->sizeOf();
//                printf("Pointer p in buffer%x\n", p);
//            }
//            printf("Pointer p in buffer%x\n", p);

//            memcpy(p, &clientIsRelay, sizeof(clientIsRelay)); p += sizeof(clientIsRelay);
//            memcpy(p, &worldConnSize, sizeof(worldConnSize)); p += sizeof(worldConnSize);
//            memcpy(p, &_steps_recorded, sizeof(_steps_recorded)); p += sizeof(_steps_recorded);

//            memcpy(p, &worldSendStat, sizeof(worldSendStat)); p += sizeof(worldSendStat);
//            memcpy(p, &worldRecvStat, sizeof(worldRecvStat)); p += sizeof(worldRecvStat);
//            memcpy(p, &worldIsGateway, sizeof(worldIsGateway)); p += sizeof(worldIsGateway);
//            memcpy(p, &worldIsMatcher, sizeof(worldIsMatcher)); p += sizeof(worldIsMatcher);

//        }
//        return sizeOf ();

//    }

//    size_t VASTStatLog::deserialize (const char *buffer, size_t size)
//    {
//        if (buffer != NULL && size >= sizeOf()) {
////            std::cout << "Deserializing" << std::endl;

//            memcpy(&timestamp, buffer, sizeof(timestamp)); buffer += sizeof(timestamp);
//            clientNode.deserialize(buffer, clientNode.sizeOf()); buffer += sizeof(clientNode.sizeOf());

////            memcpy(&neighbors_size, buffer, sizeof(neighbors_size)); buffer += sizeof(neighbors_size);

////            printf("Pointer p in buffer%x\n", buffer);
////            for (size_t i = 0; i < neighbors_size; i++)
////            {
////                Node *newNode = new Node();
////                newNode->deserialize(buffer, newNode->sizeOf()); buffer += newNode->sizeOf();
////                _neighbors.push_back(newNode);
////                printf("Pointer p in buffer%x\n", buffer);
////            }
////            printf("Pointer p in buffer%x\n", buffer);

//            memcpy(&clientIsRelay, buffer, sizeof(clientIsRelay)); buffer += sizeof(clientIsRelay);
//            memcpy(&worldConnSize, buffer, sizeof(worldConnSize)); buffer += sizeof(worldConnSize);
//            memcpy(&_steps_recorded, buffer, sizeof(_steps_recorded)); buffer += sizeof(_steps_recorded);
////            memcpy(&worldSendStat, buffer, sizeof(worldSendStat)); buffer += sizeof(worldSendStat);
////            memcpy(&worldRecvStat, buffer, sizeof(worldRecvStat)); buffer += sizeof(worldRecvStat);
////            memcpy(&worldIsGateway, buffer, sizeof(worldIsGateway)); buffer += sizeof(worldIsGateway);
////            memcpy(&worldIsMatcher, buffer, sizeof(worldIsMatcher)); buffer += sizeof(worldIsMatcher);

////            return size;
//        }
//        return 0;
//    }

    bool VASTStatLog::operator==(const VASTStatLog other) {

        bool equals = true;

        equals = equals && this->timestamp == other.timestamp;
        equals = equals && this->clientNode == other.clientNode;
        equals = equals && this->neighbors_size == other.neighbors_size;
        equals = equals && this->_neighbors == other._neighbors;
        equals = equals && this->clientIsRelay == other.clientIsRelay;
        equals = equals && this->worldConnSize == other.worldConnSize;
        equals = equals && this->worldSendStat == other.worldSendStat;
        equals = equals && this->worldRecvStat == other.worldRecvStat;
        equals = equals && this->worldIsGateway == other.worldIsGateway;
        equals = equals && this->worldIsMatcher == other.worldIsMatcher;

        return equals;
    }

    VASTStatLog::~VASTStatLog()
    {
        //Do memory free here....
    }

}
