
#include <algorithm>
#include "Room.h"

#include "utility/synch.h"
#include "streamerClient/StreamerClient.h"

Room::Room(string name): name(std::move(name)) {

}

Room::~Room() {

}

bool Room::removeClient(StreamerClient *client) {
    synchronized(mut) {
        return clients.erase(client) != 0;
    }
}

unordered_set<StreamerClient*> Room::getClients() {
    return clients;
}

void Room::addClient(StreamerClient *client) {
    synchronized(mut) {
        clients.insert(client);
    }
}

bool Room::isEmpty() {
    return clients.empty();
}

string Room::getName() {
    return name;
}

MusicTrack* Room::reserveTrackSlot() {

    MusicTrack* track = nullptr;

    synchronized(mut) {

        if (tracks.size() < MAX_TRACK_NUMBER) {

            track = new MusicTrack();
            tracks.push_back(track);
        }
    }

    return track;
}
