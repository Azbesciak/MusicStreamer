
#include <algorithm>
#include <utility>
#include "Room.h"

#include "utility/synch.h"
#include "streamerClient/StreamerClient.h"
#include "../messageSender/MessageSender.h"


Room::Room(const string & name) :
        name(name),
        streamer(new MusicStreamer([&](vector<string> &tracks) {
            synchronized(clientsMut) {
                cout << "SENDING TRACKS" << endl;
                sendTrackListToClients(tracks);
            }
        })) {
}

Room::~Room() {
    delete streamer;
    streamer = nullptr;
}


bool Room::removeClient(StreamerClient *client) {
    synchronized(clientsMut) {
        if (clients.erase(client) != 0) {
            streamer->leaveClient(client);
            client->leaveStreamingChannel();
            client->leaveRoom();
            sendListOfClientsToAll();
            return true;
        }
        return false;
    }
}


const unordered_set<StreamerClient *> &Room::getClients() {
    return clients;
}


void Room::addClient(StreamerClient *client) {
    synchronized(clientsMut) {
        clients.insert(client);
        sendListOfClientsToAll();
        streamer->joinClient(client);
        unordered_set<StreamerClient*> client_set;
        client_set.insert(client);
        ClientResponse response = createBasicResponse();
        prepareTracksResponse(&response);
        sendResponseToAll(&response, client_set);
        client->setStreamingSocket(streamer->getStreamingSocket());
    }
}


bool Room::isEmpty() {
    return clients.empty();
}


string Room::getName() {
    return name;
}


MusicTrack *Room::reserveTrackSlot(const string &trackName) {
    return streamer->reserveTrackSlot(trackName);
}


void Room::cancelTrackReservation(MusicTrack *musicTrack) {
    streamer->cancelTrackReservation(musicTrack);
}

void Room::sendListOfClientsToAll() {
    ClientResponse response = createBasicResponse();
    prepareRoomsInfo(&response);
    sendResponseToAll(&response);
    cout<< "resp to client: " << response.serialize() <<endl;
}

void Room::sendTrackListToClients(const vector<string> &tracks) {
    ClientResponse response = createBasicResponse();
    addTracksToResponse(&response, tracks);
    sendResponseToAll(&response);
}

void Room::sendResponseToAll(ClientResponse * response) {
    sendResponseToAll(response, clients);
}

void Room::sendResponseToAll(ClientResponse * response,
                             unordered_set<StreamerClient *> &clients) {
    if (!clients.empty()){
        Message mes(clients, response->serialize());
        MessageSender::send(mes);
    }
}

void Room::prepareTracksResponse(ClientResponse *resp) {
    const vector<string> &tracks = getAvailableTracksList();
    return addTracksToResponse(resp, tracks);
}

void Room::addTracksToResponse(ClientResponse *resp, const vector<string> &tracks) const {
    resp->addToBody("tracks", tracks);
}

void Room::prepareRoomsInfo(ClientResponse *resp) {
    vector<string> names;
    names.reserve(clients.size());
    for (auto &&cli: clients) {
        names.push_back(cli->getName());
    }
    resp->addToBody("clients", names);

}

ClientResponse Room::createBasicResponse() {
    ClientResponse resp;
    resp.addToBody("room", name);
    resp.setStatus(200);
    return resp;
}

vector<string> Room::getAvailableTracksList() {
    return streamer->getAvailableTracksList();
}


MusicTrack *Room::findTrackByName(const string &trackName) {
    return streamer->findTrackByName(trackName);
}


TracksQueue *Room::getTracksQueue() {
    return streamer->getTracksQueue();
}

void Room::onTrackListChanged() {
    synchronized(clientsMut) {
        const vector<string> &tracks = streamer->getAvailableTracksList();
        sendTrackListToClients(tracks);
    }
}

void Room::prepareTrackQueueResponse(ClientResponse *resp) {
    vector<string> trackNames;
    auto tracks = getTracksQueue()->getQueuedTracks();
    transform(tracks.begin(), tracks.end(), back_inserter(trackNames),
              [](MusicTrack *track) -> string { return track->getTrackName(); });
    resp->addToBody("tracksQueue", trackNames);
}
