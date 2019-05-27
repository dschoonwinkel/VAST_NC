#include "rlncmessage.h"
#include "net_udpNC_handler.h"
#include "net_udpnc_consumer.h"
#include <assert.h>
#include "vast_capturemsgs.h"
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <map>
#include <boost/asio.hpp>

using namespace boost::asio;

using namespace Vast;
using namespace std;

std::vector<string> msgcapIDs;
std::map<string, std::vector<MessageWrapper>> allRestoredMsgs;
std::map<string, size_t> logIndex;
std::vector<string> logIDs;
timestamp_t latest_timestamp;

void initVariables()
{
    path dir_path("./msgcap");
    directory_iterator end_itr;

    for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {

        //Skip subfolders
        if (is_directory(itr->path())) continue;

        CPPDEBUG(itr->path() << std::endl);
        std::string filename = itr->path().string();
        //If this is not a TXT file, it is probably not a VASTStatLog file
        if (filename.find(".txt") == string::npos)
        {
            CPPDEBUG("Skipping " << filename << std::endl);
            continue;
        }
        std::vector<MessageWrapper> restoredMsgs = vast_capturemsgs::restoreAllVASTMessage(filename);

        //Cut off .txt
        std::string id_string = filename.substr(0, filename.find(".txt"));
        //Extract id_t
        id_string = id_string.substr(id_string.find("N") + 1);

        allRestoredMsgs[filename] = restoredMsgs;
        logIDs.push_back(filename);

        latest_timestamp = ULONG_LONG_MAX;
        //Initiate latest_timestamp to the earliest timestamp
        for (size_t log_iter = 0; log_iter < logIDs.size(); log_iter++)
        {
          CPPDEBUG("Starting timestamp: " << allRestoredMsgs[logIDs[log_iter]][0].timestamp << std::endl);
          if (allRestoredMsgs[logIDs[log_iter]][0].timestamp < latest_timestamp)
          {
              latest_timestamp = allRestoredMsgs[logIDs[log_iter]][0].timestamp;
          }
        }
    }
}

void sendMessages()
{
    ip::udp::endpoint endpoint(ip::address::from_string("127.0.0.1"), 1038);
    net_udpNC_handler handler(endpoint);
    timestamp_t ticker = latest_timestamp;

    for (size_t i = 0; i < logIDs.size(); i++)
    {

    }


}

int main()
{
    initVariables();

    std::cout << allRestoredMsgs[logIDs[0]][0]._msg << std::endl;

    return 0;
}
