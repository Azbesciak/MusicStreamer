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
    explicit Room(const string & name);
    ~Room();
    void addClient(StreamerClient *client);
    bool removeClient(StreamerClient * client);
    const unordered_set<StreamerClient*>& getClients();
    bool isEmpty();
    string getName();

    ClientResponse createBasicResponse();
    MusicTrack* reserveTrackSlot(const std::string& trackName);
    void cancelTrackReservation(MusicTrack* musicTrack);

    void sendListOfClientsToAll();

    void prepareRoomsInfo(ClientResponse *resp);
    void prepareTracksResponse(ClientResponse *resp);
    void prepareTrackQueueResponse(ClientResponse *resp);


    vector<string> getAvailableTracksList();

    void sendResponseToAll(ClientResponse * response);

    void addTracksToResponse(ClientResponse *resp, const vector<string> &tracks) const;
    void addTracksQueueToResponse(ClientResponse *resp, const vector<string> &queue) const;

    void sendTrackListToClients(const vector<string> &tracks);

    void sendResponseToAll(ClientResponse * response, unordered_set<StreamerClient *> &clients);

    void onTrackListChanged();

    void sendQueueToAllClients(const vector<string> &queue);

    bool queueTrack(const string &trackName);

    void sendInitialMessage(StreamerClient *client);

    void switchTrack();
};


#endif //MUSICSTREAMER_ROOM_H
