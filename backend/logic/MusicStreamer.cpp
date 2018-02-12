#include "MusicStreamer.h"

#include <utility/synch.h>
#include <algorithm>

using namespace std;


MusicStreamer::MusicStreamer(TracksQueue* tracksQueue) {

    this->tracksQueue = tracksQueue;
    tracksQueue->addOnTrackQueuedListener(this);

    if (tracksQueue->getCurrentTrack() != nullptr) {

        playTrack();
    }
}


void MusicStreamer::playTrack() {

    MusicTrack* track = tracksQueue->getCurrentTrack();

    trackStream = new TrackStream(track, clients);
    trackStream->start();

    // Todo implement scheduler
}


void MusicStreamer::joinClient(StreamerClient* streamerClient) {

    synchronized(clientsMut) {

        clients.push_back(streamerClient);

        if (trackStream != nullptr)
            trackStream->attachClient(streamerClient);
    }
}


void MusicStreamer::leaveClient(StreamerClient* streamerClient) {

    synchronized(clientsMut) {

        auto found = find(clients.begin(), clients.end(), streamerClient);

        if (found != clients.end()) {

            clients.erase(found);

            if (trackStream != nullptr)
                trackStream->detachClient(streamerClient);
        }
    }
}


void MusicStreamer::onTrackQueued() {

    synchronized(trackMut) {

        // Todo implement track stream finished check
        if (trackStream == nullptr || trackStream) {

            if (trackStream != nullptr) {

                trackStream->stop();
                delete trackStream;
            }

            trackStream = new TrackStream(tracksQueue->getCurrentTrack(), clients);
            trackStream->start();
        }
    }
}


MusicStreamer::~MusicStreamer() {

    delete trackStream;
}
