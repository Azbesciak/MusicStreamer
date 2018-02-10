
#include <algorithm>
#include "Room.h"

#include "utility/synch.h"
#include "streamerClient/StreamerClient.h"


Room::Room(string name): name(std::move(name)) {

    tracksQueue = new TracksQueue();
    streamer = new MusicStreamer(this);
}


Room::~Room() {

    delete streamer;
    delete tracksQueue;
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


MusicTrack* Room::reserveTrackSlot(const string& trackName) {

    MusicTrack* track = nullptr;

    synchronized(mut) {

        if (availableTracks.size() < MAX_TRACK_NUMBER && findTrackByName(trackName) == nullptr) {

            track = new MusicTrack(trackName);
            availableTracks.push_back(track);
        }
    }

    return track;
}


void Room::cancelTrackReservation(MusicTrack* musicTrack) {

    synchronized(mut) {

        long trackIndex = distance(availableTracks.begin(), find(availableTracks.begin(), availableTracks.end(), musicTrack));

        if (trackIndex < availableTracks.size()) {

            availableTracks.erase(availableTracks.begin() + trackIndex);
            delete musicTrack;
        }
    }
}


MusicTrack* Room::findTrackByName(const string& trackName) {

    MusicTrack* resultTrack = nullptr;

    synchronized(mut) {

        for (MusicTrack* track : availableTracks) {

            if (track->getTrackName() == trackName) {

                resultTrack = track;
                break;
            }
        }
    }

    return resultTrack;
}


TracksQueue* Room::getTracksQueue() {
    return tracksQueue;
}
