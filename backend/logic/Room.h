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
    std::recursive_mutex clientsMut;
    std::unordered_set<StreamerClient*> clients;
    MusicStreamer* streamer;
    string name;

public:
    explicit Room(string name);
    ~Room();
    void addClient(StreamerClient *client);
    bool removeClient(StreamerClient * client);
    const unordered_set<StreamerClient*>& getClients();
    bool isEmpty();
    string getName();

    MusicTrack* reserveTrackSlot(const std::string& trackName);
    void cancelTrackReservation(MusicTrack* musicTrack);

    std::vector<MusicTrack*> getAvailableTracks();
    MusicTrack* findTrackByName(const std::string& trackName);
    TracksQueue* getTracksQueue();
};


#endif //MUSICSTREAMER_ROOM_H
