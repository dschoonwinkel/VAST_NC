#include "vaststatlog_entry.h"
#include <climits>
#include <stdio.h>

#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <stdexcept>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

namespace Vast {

    VASTStatLogEntry::VASTStatLogEntry(AbstVASTVerse *world, VAST *client)

    {
        _world = world;
        _client = client;

        _steps_recorded = 0;
        _min_aoi = LONG_MAX;
        _total_aoi = 0;
        _max_CN = _total_CN = 0;

        if (_world != NULL)
            _world->clearStat();

        size_t logfile_count = 0;
       do
        {
           _logfilename = _logfilename_base + "_N" + std::to_string(client->getSelf()->id) +
                   "_"+ std::to_string(logfile_count) + ".txt";
           CPPDEBUG("VASTStatLogEntry: _logfilename: " << _logfilename << std::endl);
           logfile_count++;
        } while (boost::filesystem::exists(_logfilename));

        std::cout << "Logfilename: " << _logfilename << std::endl;

            pOfs = std::make_unique<std::ofstream>(_logfilename);
       if (!pOfs->is_open())
       {
            std::cerr << "VASTStatLogEntry::constructor file open : " << (pOfs->is_open() ? "true":"false") << std::endl << "EXITING" <<std::endl;
            exit(EXIT_FAILURE);
       }

        pAr = std::make_unique<boost::archive::text_oarchive>(*pOfs);

    }

    //Copy constructor
    VASTStatLogEntry::VASTStatLogEntry(const VASTStatLogEntry &other)
    {
        this->timestamp = other.timestamp;
        this->clientNode = other.clientNode;
        this->subID = other.subID;
        this->clientIsRelay = other.clientIsRelay;
        this->neighbors_size = other.neighbors_size;
        this->_neighbors = other._neighbors;
        this->clientIsJoined = other.clientIsJoined;
        this->worldConnSize = other.worldConnSize;
        this->worldSendStat = other.worldSendStat;
        this->worldRecvStat = other.worldRecvStat;
        this->worldIsGateway = other.worldIsGateway;
        this->worldIsMatcher = other.worldIsMatcher;
        this->matcherAOI = other.matcherAOI;
    }

    void VASTStatLogEntry::recordStat()
    {
        timestamp = _world->getTimestamp();

        if (_client == NULL)
        {
            std::cerr << "vaststatlog::recordStat() _client was NULL " << std::endl;
            return;
        }

        clientNode = *(_client->getSelf());

        subID = _client->getSubscriptionID();
        clientIsJoined = _client->isJoined();

        // record keeping
        long aoi = clientNode.aoi.radius;

        _total_aoi += aoi;

        if (aoi < _min_aoi)
            _min_aoi = aoi;


        // set up neighbor view
        _neighbors.clear();
        std::vector<Node*> clientlist = _client->list ();
        for (Node* node : clientlist)
        {
            if (node != NULL) {
                _neighbors.push_back(*node);
            }
        }


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

        if (_world->getMatcherAOI())
            matcherAOI = (*_world->getMatcherAOI());

//        if (_world->getMatcherVoronoi())
//        matcherVoronoi = (*_world->getMatcherVoronoi());


        _steps_recorded++;
        saveToLogFile();
    }

    void VASTStatLogEntry::printStat ()
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
        for (std::vector<Node>::iterator it = _neighbors.begin(); it != _neighbors.end(); it++)
        {
            printf("%lu\n", (*it).id);
        }

        printf("Relay: %d \n\n", clientIsRelay);

        printf("\nCN: %u\n", worldConnSize);
        printf("GetSendStat: %f\n", worldSendStat.average);
        printf("GetRecvStat: %f\n\n", worldRecvStat.average);

        printf("GW: %s ", worldIsGateway ? "true" : "false");
        printf("Matcher: %s ", worldIsMatcher ? "true" : "false");


        if (worldIsMatcher)
        {

            printf("Matcher AOI x corrd %2.2lf\n", matcherAOI.center.x);
            printf("Matcher AOI y corrd %2.2lf\n", matcherAOI.center.y);
            printf("Matcher AOI z corrd %2.2lf\n", matcherAOI.center.z);
            printf("Matcher AOI %d\n", matcherAOI.radius);
        }


        printf("steps_recorded: %d \n", _steps_recorded);
        printf("******************************************************\n");
    }

//    size_t VASTStatLogEntry::sizeOf ()
//    {
//        size_t total_size = 0;

//        total_size+= sizeof(timestamp);
//        // CPPDEBUG("sizeof timestamp                      " << sizeof(timestamp) << std::endl);

//        total_size+= clientNode.sizeOf();
//        // CPPDEBUG("clientNode->sizeof                    " << clientNode.sizeOf() << std::endl);

//        total_size+= _neighbors.size()*sizeof(Node);
//        // CPPDEBUG("sizeof neighbor Node list             " << _neighbors.size()*sizeof(Node) << std::endl);

//        total_size+= sizeof(clientIsRelay);
//        // CPPDEBUG("sizeof clientIsRelay                  " << sizeof(clientIsRelay) << std::endl);

//        total_size+= sizeof(worldConnSize);
//        // CPPDEBUG("sizeof worldConnSize                  " << sizeof(worldConnSize) << std::endl);

//        total_size+= worldSendStat.sizeOf();
//        // CPPDEBUG("sizeof worldSendStat                  " << worldSendStat.sizeOf() << std::endl);

//        total_size+= worldRecvStat.sizeOf();
//        // CPPDEBUG("sizeof worldRecvStat                  " << worldRecvStat.sizeOf() << std::endl);

//        total_size+= sizeof(worldIsGateway);
//        // CPPDEBUG("sizeof worldIsGateway                 " << sizeof(worldIsGateway) << std::endl);

//        total_size+= sizeof(worldIsMatcher);
//        // CPPDEBUG("sizeof worldIsMatcher                 " << sizeof(worldIsMatcher) << std::endl);

//        // CPPDEBUG("Total size: " << total_size << "\n" << std::endl);

//        /*
//         * if (_world.isMatcher()) {
//         *  total_size+=_world.getMatcherAOI();
//         *  // CPPDEBUG("sizeof _world.getMatcherAOI()" << sizeof(_world.getMatcherAOI()) << std::endl);
//         * }
//        */

//        return total_size;
//    }

    void VASTStatLogEntry::saveToLogFile() {

        this->serialize(*pAr, 0);
        pOfs->flush();

    }

    void VASTStatLogEntry::restoreFirstFromLogFile(std::string filename) {
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ar(ifs);

        this->serialize(ar, 0);
    }

    std::vector<VASTStatLogEntry> VASTStatLogEntry::restoreAllFromLogFile(std::string filename) {

        std::vector<VASTStatLogEntry> restoredLogs;
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ar(ifs);

        while(!ifs.eof()) {
            VASTStatLogEntry log;
            log.serialize(ar,0);
            restoredLogs.push_back(log);
        }

        return restoredLogs;
    }

    bool VASTStatLogEntry::operator==(const VASTStatLogEntry other) {

        bool equals = true;

        equals = equals && this->timestamp == other.timestamp;
        equals = equals && this->clientNode == other.clientNode;
        equals = equals && this->neighbors_size == other.neighbors_size;
//        equals = equals && _neighbors == other._neighbors;
        equals = equals && this->clientIsRelay == other.clientIsRelay;
        equals = equals && this->worldConnSize == other.worldConnSize;
        equals = equals && this->worldSendStat == other.worldSendStat;
        equals = equals && this->worldRecvStat == other.worldRecvStat;
        equals = equals && this->worldIsGateway == other.worldIsGateway;
        equals = equals && this->worldIsMatcher == other.worldIsMatcher;

        return equals;
    }

    VASTStatLogEntry& VASTStatLogEntry::operator=(const VASTStatLogEntry &other)
    {
        this->timestamp = other.timestamp;
        this->clientNode = other.clientNode;
        this->subID = other.subID;
        this->clientIsRelay = other.clientIsRelay;
        this->neighbors_size = other.neighbors_size;
        this->_neighbors = other._neighbors;
        this->clientIsJoined = other.clientIsJoined;
        this->worldConnSize = other.worldConnSize;
        this->worldSendStat = other.worldSendStat;
        this->worldRecvStat = other.worldRecvStat;
        this->worldIsGateway = other.worldIsGateway;
        this->worldIsMatcher = other.worldIsMatcher;
        this->matcherAOI = other.matcherAOI;

        return *this;
    }

    std::ostream& operator<<(std::ostream& output, Vast::VASTStatLogEntry const& stat )
    {

            output << "VASTStatLog::stream >> output: ******************************\n";
            output << "Timestamp " <<  stat.timestamp << std::endl;
            output << "My Node ID \n" <<  stat.clientNode.id << std::endl;

            output << "My Node host_id \n" <<  stat.clientNode.id << std::endl;
            output << "My Node lasttime " <<  stat.clientNode.time << std::endl;
            output << "My Node AOI x corrd " <<  stat.clientNode.aoi.center.x << std::endl;
            output << "My Node AOI y corrd " <<  stat.clientNode.aoi.center.y << std::endl;
            output << "My Node AOI z corrd " <<  stat.clientNode.aoi.center.z << std::endl;
            output << "My Node AOI " <<  stat.clientNode.aoi.radius << std::endl;

            output << "My Node Addr host ID \n" <<  stat.clientNode.addr.host_id << std::endl;
            output << "My Node Addr last acc " <<  stat.clientNode.addr.lastAccessed << std::endl;
            output << "My Node Addr IP host " <<  stat.clientNode.addr.publicIP.host << std::endl;
            output << "My Node Addr IP port " <<  stat.clientNode.addr.publicIP.port << std::endl;

            output << "Num neighbors: " <<  stat.neighbors_size << std::endl;
            output << "My neighbour IDs: \n" << std::endl;
            for (size_t i = 0; i < stat._neighbors.size(); i++)
            {
                output << "" <<  stat._neighbors[i].id << std::endl;
            }

            output << "Relay: " <<  stat.clientIsRelay << std::endl;

            output << "\nCN: " <<  stat.worldConnSize << std::endl;
            output << "GetSendStat: " <<  stat.worldSendStat.average << std::endl;
            output << "GetRecvStat: " <<  stat.worldRecvStat.average << std::endl;

            output << "GW: " <<  (stat.worldIsGateway ? "true" : "false") << std::endl;
            output << "Matcher: " <<  (stat.worldIsMatcher ? "true" : "false") << std::endl;


            output << "steps_recorded: " <<  stat._steps_recorded << std::endl;
            output << "******************************************************" << std::endl;


            return output;
    }

    // distance to a point
    bool
    VASTStatLogEntry::in_view (const VASTStatLogEntry &remote_log)
    {
        return (clientNode.aoi.center.distance (remote_log.clientNode.aoi.center) < (double)clientNode.aoi.radius);
    }

    // returns true if known
    bool
    VASTStatLogEntry::knows (const VASTStatLogEntry &remote_log)
    {
        // see if 'node' is a known neighbor of me
        for (size_t i = 0; i < _neighbors.size(); i++)
        {
            if (_neighbors[i].id == remote_log.getSubID())
                return true;
        }
        return false;
    }


    //Getters
    timestamp_t VASTStatLogEntry::getTimestamp() const
    {
        return timestamp;
    }

    int VASTStatLogEntry::isRelay() const
    {
        return clientIsRelay;
    }

    bool VASTStatLogEntry::isJoined() const
    {
        return clientIsJoined;
    }


    Node VASTStatLogEntry::getClientNode()
    {
        return clientNode;
    }

    Node VASTStatLogEntry::getNeighborByID(Vast::id_t neighbor_id) const {
        for (Node i : _neighbors)
        {
            if (i.id == neighbor_id)
            {
                return i;
            }
        }

        throw std::invalid_argument("vaststatlog_entry::getNeighborByID: Node requested is not in vector");
        return Node();
    }

    Vast::id_t VASTStatLogEntry::getSubID() const
    {
        return subID;
    }

    size_t VASTStatLogEntry::getNeighborsSize()
    {
        return neighbors_size;
    }
    std::vector<Node> VASTStatLogEntry::getNeighbors()
    {
        return _neighbors;
    }

    int VASTStatLogEntry::getWorldConnSize()
    {
        return worldConnSize;
    }

    StatType VASTStatLogEntry::getWorldSendStat()
    {
        return worldSendStat;
    }
    StatType VASTStatLogEntry::getWorldRecvStat()
    {
        return worldRecvStat;
    }

    bool VASTStatLogEntry::getWorldIsGateway()
    {
        return worldIsGateway;
    }
    bool VASTStatLogEntry::getWorldIsMatcher()
    {
        return worldIsMatcher;
    }

    Area VASTStatLogEntry::getMatcherAOI()
    {
        return matcherAOI;
    }

    VASTStatLogEntry::~VASTStatLogEntry()
    {
        //Do memory free here....
        if (pOfs != NULL)
        {
            pOfs->flush();
            pOfs->close();
        }
    }

}



