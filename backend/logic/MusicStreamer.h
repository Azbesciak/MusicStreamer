

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
#include <server/AbstractServer.h>

class TracksQueue;
class StreamerClient;
class TrackStream;

class MusicStreamer : public OnNextTrackListener {

private:
    static const int MAX_TRACK_NUMBER = 20;
    static int minPort, maxPort;
    static string host;
    int port;
    Socket * streamingSocket;
    TracksQueue* tracksQueue;
    TrackStream* trackStream;
    std::vector<MusicTrack*> availableTracks;

    std::unordered_set<StreamerClient*> clients;
    function<void(vector<string>&)> trackListChangeListener;
    function<void(vector<string>&)> queueChangeListener;

    std::recursive_mutex trackMut;
    std::recursive_mutex clientsMut;


    void cleanTrackStream();
    void playCurrentTrack();
    MusicTrack* findTrackByName(const std::string& trackName);
    MusicTrack *withTrack(const string &trackName, function<void(MusicTrack *)> consumer = [](MusicTrack*){});
protected:
    int createSocket();

public:

    MusicStreamer(
            function<void(vector<string>&)> trackListChangeListener,
            function<void(vector<string>&)> queueChangeListener
    );

    static void setPortsRange(int minPort, int maxPort);
    static void setHost(const string &host);

    void onTrackFinished();
    void onNextTrack() override;

    Socket * getStreamingSocket();

    MusicTrack* reserveTrackSlot(const std::string& trackName);
    void cancelTrackReservation(MusicTrack* musicTrack);

    std::vector<MusicTrack*> getAvailableTracks();

    ~MusicStreamer();

    vector<string> getAvailableTracksList();

    void joinClient(StreamerClient *streamerClient);

    void leaveClient(StreamerClient *streamerClient);

    void onQueueChange(deque<MusicTrack *> trackQueue) override;

    vector<string> mapTracksQueue(deque<MusicTrack *> &trackQueue);

    bool queueTrack(const string &trackName);

    vector<string> getTracksQueue();

    void requestNextTrack();
};


#endif //MUSICSTREAMER_MUSICSTREAMER_H
