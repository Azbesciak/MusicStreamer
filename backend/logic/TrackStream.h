#ifndef MUSICSTREAMER_TRACKSTREAM_H
#define MUSICSTREAMER_TRACKSTREAM_H

#include "MusicTrack.h"

#include <streamerClient/StreamerClient.h>
#include <map>

class StreamerClient;

class TrackStream {

private:

    MusicTrack* track;
    std::vector<StreamerClient*> clients;

    std::map<StreamerClient*, bool> frameSent;

    std::recursive_mutex streamMut;
    std::recursive_mutex clientsMut;
    pthread_t* streamerThread;

    void* streamCoroutine();

public:

    TrackStream(MusicTrack* track, const std::vector<StreamerClient*>& clients);

    void start();
    void stop();

    void attachClient(StreamerClient* client);
    void detachClient(StreamerClient* client);

    ~TrackStream();
};


#endif //MUSICSTREAMER_TRACKSTREAM_H
