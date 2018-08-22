#ifndef VASTSTATLOG_H
#define VASTSTATLOG_H

class VASTClient; //Forward declaration
class VASTVerse; //Forward declaration

namespace VAST {
    class VASTStatLog
    {
    public:
        VASTStatLog();

        void recordStat ();

    private:
        long _min_aoi, _total_aoi;
        int _max_CN, _total_CN;
        int _steps_recorded;
        VASTClient* _client;
        VASTVerse* _world;
    };
}

#endif // VASTSTATLOG_H
