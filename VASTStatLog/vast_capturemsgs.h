#ifndef VAST_CAPTUREMSGS_H
#define VAST_CAPTUREMSGS_H

#include <VASTTypes.h>

#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;
#include <chrono>
#include <vector>
#include "messagewrapper.h"

#define VAST_CAPTUREMSGS

namespace Vast {

    class vast_capturemsgs
    {
    public:
        //Empty constructor required for std::map [] operator
        vast_capturemsgs() {}

        //Saving constructor
        vast_capturemsgs(id_t id);

        //Retrieving constructor
        vast_capturemsgs(std::string filename);

        static void close();
        ~vast_capturemsgs();

        static void saveVASTMessage(timestamp_t timestamp, Message msg, id_t id = 0);
        void _saveVASTMessage(timestamp_t timestamp, Message msg);



        bool getCurrentVASTMessage(Message &msg);
        bool getCurrentTimestamp(timestamp_t &timestamp);
        bool restoreNextVASTMessage(timestamp_t curr_time, Message &msg);
        size_t getRestoredMsgsCount();

    private:
        void restoreAllVASTMessage(std::string filename);


        //Storing variables
        static vast_capturemsgs *instance;
        std::string _logfilename_base = "./logs/msgcap/VASTmsgscap";
        std::string _logfilename = "./logs/msgcap/VASTmsgscap";
        std::ofstream *ofs = NULL;
        boost::archive::text_oarchive *aro = NULL;

        size_t msgcapCount = 0;
        std::chrono::microseconds msgcapTimer = std::chrono::microseconds::zero();
        std::chrono::high_resolution_clock::time_point t1;


        //Retrieving variables
        std::ifstream *ifs = NULL;
        boost::archive::text_iarchive *ari = NULL;
        size_t step = 0;
        std::vector<MessageWrapper> restoredMsgs;

    };

}

#endif // VAST_CAPTUREMSGS_H
