
#include <algorithm>
#include "Room.h"

#include "utility/synch.h"
#include "streamerClient/StreamerClient.h"


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


vector<MusicTrack *> Room::getAvailableTracks() {
    return streamer->getAvailableTracks();
}


MusicTrack *Room::findTrackByName(const string &trackName) {
    return streamer->findTrackByName(trackName);
}


TracksQueue *Room::getTracksQueue() {
    return streamer->getTracksQueue();
}
