#ifndef MUSICSTREAMER_TRACKSTREAM_H
#define MUSICSTREAMER_TRACKSTREAM_H

#include "MusicTrack.h"
#include <thread>
#include <streamerClient/StreamerClient.h>
#include <map>
#include <atomic>

class StreamerClient;
class MusicStreamer;

class TrackStream {

private:

    MusicTrack* track;
    std::unordered_set<StreamerClient*> clients;
    MusicStreamer* streamer;

    std::map<StreamerClient*, bool> frameSent;

    std::recursive_mutex streamMut;
    std::recursive_mutex  clientsMut;
    std::thread* streamerThread;
    std::atomic<bool> isRunning;

public:

    TrackStream(MusicTrack* track, std::unordered_set<StreamerClient*> clients, MusicStreamer* streamer);

    void start();
    void stop();

    ~TrackStream();

    void detachClient(StreamerClient *client);

    void attachClient(StreamerClient *client);
};


#endif //MUSICSTREAMER_TRACKSTREAM_H
