#ifndef MUSICSTREAMER_TRACKSTREAM_H
#define MUSICSTREAMER_TRACKSTREAM_H

#include "MusicTrack.h"
#include <thread>
#include <streamerClient/StreamerClient.h>
#include <map>
#include <atomic>
#include <streamerClient/ClientResponse.h>

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
    std::thread * streamerThread;
    std::atomic<bool> isRunning;
    std::atomic<bool> hasFinished;

    ClientResponse serializeTrackHeader(MusicTrack* track);

public:

    TrackStream(MusicTrack* track, std::unordered_set<StreamerClient*> clients, MusicStreamer* streamer);

    void start();
    void stop();

    ~TrackStream();

    void detachClient(StreamerClient *client);

    void attachClient(StreamerClient *client);

    void waitUntilWorkEnd();
};


#endif //MUSICSTREAMER_TRACKSTREAM_H
