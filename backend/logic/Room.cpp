
#include <algorithm>
#include "Room.h"

#include "utility/synch.h"
#include "streamerClient/StreamerClient.h"
#include "../messageSender/MessageSender.h"


Room::Room(string name) :
        name(std::move(name)),
        streamer(new MusicStreamer(&clients, &clientsMut)) {
}

Room::~Room() {
    delete streamer;
    streamer = nullptr;
}


bool Room::removeClient(StreamerClient *client) {
    synchronized(clientsMut) {
        if (clients.erase(client) != 0) {
            client->leaveStreamingChannel();
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
    vector<string> names;
    names.reserve(clients.size());
    for (auto &&cli: clients) {
        names.push_back(cli->getName());
    }

    ClientResponse resp;
    resp.addToBody("room", name);
    resp.addToBody("clients", names);
    resp.setStatus(200);
    MessageSender::sendMessage(clients, &resp);
}


vector<MusicTrack *> Room::getAvailableTracks() {
    return streamer->getAvailableTracks();
}


MusicTrack *Room::findTrackByName(const string &trackName) {
    return streamer->findTrackByName(trackName);
}


TracksQueue *Room::getTracksQueue() {
    return streamer->getTracksQueue();
}
