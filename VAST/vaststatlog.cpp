#include "vaststatlog.h"
#include "VASTVerse.h"
#include <climits>

namespace Vast {

    VASTStatLog::VASTStatLog(VASTVerse *world, VAST *client)
    {
        _world = world;
        _client = client;

        _steps_recorded = 0;
        _min_aoi = LONG_MAX;
        _total_aoi = 0;
        _max_CN = _total_CN = 0;
        _world->clearStat();
    }

    void VASTStatLog::recordStat()
    {


        if (_client == NULL)
        {
            std::cerr << "vaststatlog::recordStat() _client was NULL " << std::endl;
            return;
        }

        // record keeping
        long aoi = _client->getSelf()->aoi.radius;
        _total_aoi += aoi;

        if (aoi < _min_aoi)
            _min_aoi = aoi;

        //CN = connected neighbours
        //int CN = vnode->list ().size ();
        int CN = _world->getConnectionSize ();

        _total_CN += CN;
        if (_max_CN < CN)
            _max_CN = CN;

        // set up neighbor view
        _neighbors.clear ();
        vector<Node *> &nodes = _client->list ();
        for (size_t i=1; i < nodes.size (); ++i)
            if (nodes[i]->id != 0)
                _neighbors[nodes[i]->id] = nodes[i];
            else
                printf ("zero ID\n");

        _steps_recorded++;
    }

    void VASTStatLog::printStat ()
    {
        printf("VASTStatLog::printStat: ******************************\n");
        printf("My Node ID %lu\n", _client->getSelf()->id);
        printf("My Sub ID %lu\n", _client->getSubscriptionID());
        printf("My neighbour IDs: \n");

        for (std::map<Vast::id_t, Node*>::iterator it = _neighbors.begin(); it != _neighbors.end(); it++)
        {
            printf("%lu\n", it->first);
        }

        printf("\nCN: %u\n", _world->getConnectionSize());
        printf("GetSendStat: %f\n", _world->getSendStat().average);
        printf("GetRecvStat: %f\n\n", _world->getReceiveStat().average);

        printf("GW: %s ", _world->isGateway() ? "true" : "false");
        printf("Matcher: %s ", _world->isMatcher() ? "true" : "false");
        printf("Relay: %s \n\n", _client->isRelay() ? "true" : "false");

        printf("steps_recorded: %d \n", _steps_recorded);
        printf("******************************************************\n");
    }

    size_t VASTStatLog::sizeOf ()
    {
        size_t total_size = 0;

        total_size+= sizeof(timestamp);
        std::cout << "sizeof timestamp                      " << sizeof(timestamp) << std::endl;

        total_size+= _client->getSelf()->sizeOf();
        std::cout << "getSelf()->sizeof                     " << _client->getSelf()->sizeOf() << std::endl;

        total_size+= _client->list().size()*sizeof(Node);
        std::cout << "sizeof neighbor Node list             " << _client->list().size()*sizeof(Node) << std::endl;

        total_size+= sizeof(_client->isRelay());
        std::cout << "sizeof isRelay()                      " << sizeof(_client->isRelay()) << std::endl;

        total_size+= sizeof(_world->getConnectionSize());
        std::cout << "sizeof _world->getConnectionSize()    " << sizeof(_world->getConnectionSize()) << std::endl;

        total_size+= _world->getSendStat().sizeOf();
        std::cout << "sizeof getSendStat                    " << _world->getSendStat().sizeOf() << std::endl;

        total_size+= _world->getReceiveStat().sizeOf();
        std::cout << "sizeof getReceiveStat                 " << _world->getReceiveStat().sizeOf() << std::endl;

        total_size+= sizeof(_world->isGateway());
        std::cout << "sizeof _world->isGateway()            " << sizeof(_world->isGateway()) << std::endl;

        total_size+= sizeof(_world->isMatcher());
        std::cout << "sizeof _world->isMatcher()            " << sizeof(_world->isMatcher()) << std::endl;

        std::cout << std::endl;

        /*
         * if (_world.isMatcher()) {
         *  total_size+=_world.getMatcherAOI();
         *  std::cout << "sizeof _world.getMatcherAOI()" << sizeof(_world.getMatcherAOI()) << std::endl;
         * }
        */

        return total_size;
    }

    size_t VASTStatLog::serialize (char *p)
    {
        // NOTE: average is not sent
        if (p != NULL)
        {
            memcpy (p, &timestamp,    sizeof (timestamp));  p += sizeof (timestamp);

            _client->getSelf()->serialize(p);               p += sizeof(_client->getSelf()->sizeOf());

            std::vector<Node*> neighbors = _client->list();
            size_t neighbors_size = neighbors.size();
            mempcpy(p, &neighbors_size, sizeof(neighbors_size)); p += sizeof(neighbors_size);
            for (Node * neighbor : neighbors)
            {
                neighbor->serialize(p); p+= neighbor->sizeOf();
            }



        }
        return sizeOf ();

    }

    size_t VASTStatLog::deserialize (const char *buffer, size_t size)
    {
        if (buffer != NULL && size >= sizeOf()) {
            std::cout << "Do stuff here" << std::endl;
        }
        return 0;
    }

    bool VASTStatLog::operator==(const VASTStatLog other) {

        bool equals = true;

        equals = equals && this->timestamp == other.timestamp;
        equals = equals && this->clientIsRelay == other.clientIsRelay;
        equals = equals && this->clientNode == other.clientNode;
        equals = equals && this->_neighbors == other._neighbors;
        equals = equals && this->worldConnSize == other.worldConnSize;
        equals = equals && this->worldSendStat == other.worldSendStat;
        equals = equals && this->worldRecvStat == other.worldRecvStat;
        equals = equals && this->worldIsGateway == other.worldIsGateway;
        equals = equals && this->worldIsMatcher == other.worldIsMatcher;

        return equals;
    }

}
