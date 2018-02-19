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

    ClientResponse createBasicResponse();
    MusicTrack* reserveTrackSlot(const std::string& trackName);
    void cancelTrackReservation(MusicTrack* musicTrack);

    MusicTrack* findTrackByName(const std::string& trackName);
    TracksQueue* getTracksQueue();

    void sendListOfClientsToAll();

    ClientResponse* prepareRoomsInfo(ClientResponse *resp);
    ClientResponse* prepareTracksResponse(ClientResponse *resp);
    ClientResponse* prepareTrackQueueResponse(ClientResponse * resp);


    vector<string> getAvailableTracksList();

    void sendResponseToAll(const function<ClientResponse(ClientResponse*)> &decorator);

    ClientResponse *addTracksToResponse(ClientResponse *resp, const vector<string> &tracks) const;

    void sendTrackListToClients(const vector<string> &tracks);
    void sendQueueuedTracksToClients(const vector<string> &tracks);

    void sendResponseToAll(
            const function<ClientResponse(ClientResponse *)> &decorator,
            unordered_set<StreamerClient *> clients
    );

    void onTrackListChanged();
};


#endif //MUSICSTREAMER_ROOM_H
