

#ifndef MUSICSTREAMER_MUSICSTREAMER_H
#define MUSICSTREAMER_MUSICSTREAMER_H


#include <unordered_set>
#include <deque>
#include <thread>
#include <condition_variable>

#include <logic/MusicTrack.h>
#include <server/music/MusicChannel.h>
#include <streamerClient/StreamerClient.h>
#include <logic/TrackStream.h>
#include <logic/callback/OnNextTrackListener.h>

class TracksQueue;
class StreamerClient;
class TrackStream;

class MusicStreamer : public OnNextTrackListener {

private:

    TracksQueue* tracksQueue;
    TrackStream* trackStream;
    std::vector<StreamerClient*> clients;

    std::recursive_mutex trackMut;
    std::recursive_mutex clientsMut;

    void playCurrentTrack();

public:

    explicit MusicStreamer(TracksQueue* tracksQueue);

    void joinClient(StreamerClient* streamerClient);
    void leaveClient(StreamerClient* streamerClient);

    void onTrackFinished();
    void onNextTrack() override;

    ~MusicStreamer();
};


#endif //MUSICSTREAMER_MUSICSTREAMER_H
