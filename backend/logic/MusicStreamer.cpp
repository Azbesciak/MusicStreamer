#include "MusicStreamer.h"

#include <utility/synch.h>
#include <algorithm>

using namespace std;


MusicStreamer::MusicStreamer(TracksQueue* tracksQueue) {

    this->tracksQueue = tracksQueue;
    tracksQueue->addOnNextTrackListener(this);

    if (tracksQueue->currentTrack() != nullptr)
        playCurrentTrack();
}


void MusicStreamer::playCurrentTrack() {

    synchronized(trackMut) {

        MusicTrack* track = tracksQueue->currentTrack();

        trackStream = new TrackStream(track, clients, this);
        trackStream->start();
    }
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


void MusicStreamer::onTrackFinished() {

    synchronized(trackMut) {

        delete trackStream;
        tracksQueue->nextTrack();
    }
}


void MusicStreamer::onNextTrack() {

    synchronized(trackMut) {

        if (trackStream != nullptr) {
            trackStream->stop();
            delete trackStream;
        }

        playCurrentTrack();
    }
}


MusicStreamer::~MusicStreamer() {

    if (trackStream != nullptr)
        trackStream->stop();

    delete trackStream;
}
