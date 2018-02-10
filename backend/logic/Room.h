#include <string>
#include <vector>

#ifndef MUSICSTREAMER_ROOM_H
#define MUSICSTREAMER_ROOM_H

#include <unordered_set>
#include <mutex>
#include <map>
#include <logic/MusicStreamer.h>
#include <logic/MusicTrack.h>
#include "TracksQueue.h"

using namespace std;

class StreamerClient;
class MusicStreamer;

class Room {

private:

    static const int MAX_TRACK_NUMBER = 20;

    std::recursive_mutex mut;

    std::unordered_set<StreamerClient*> clients;
    std::vector<MusicTrack*> availableTracks;

    MusicStreamer* streamer;
    TracksQueue* tracksQueue;

    string name;

public:
    explicit Room(string name);
    ~Room();
    void addClient(StreamerClient *client);
    bool removeClient(StreamerClient * client);
    unordered_set<StreamerClient*> getClients();
    bool isEmpty();
    string getName();

    MusicTrack* reserveTrackSlot(const std::string& trackName);
    void cancelTrackReservation(MusicTrack* musicTrack);

    MusicTrack* findTrackByName(const std::string& trackName);
    TracksQueue* getTracksQueue();
};


#endif //MUSICSTREAMER_ROOM_H
