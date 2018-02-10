

#ifndef MUSICSTREAMER_MUSICSTREAMER_H
#define MUSICSTREAMER_MUSICSTREAMER_H


#include <unordered_set>
#include <thread>

#include <logic/MusicTrack.h>
#include <server/music/MusicChannel.h>
#include "streamerClient/StreamerClient.h"


class Room;

class MusicStreamer {

private:

    MusicTrack* currentTrack;
    Room* room;

    std::recursive_mutex mut;
    pthread_t* streamerThread;

    void* runStreamerExecutor();

public:

    MusicStreamer(Room* room);

    void streamTrack(MusicTrack* track);

    ~MusicStreamer();
};


#endif //MUSICSTREAMER_MUSICSTREAMER_H
