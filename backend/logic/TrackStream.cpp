#include "TrackStream.h"

#include <utility/synch.h>
#include <algorithm>

using namespace std;


typedef void* (*THREADFUNCPTR)(void*);

TrackStream::TrackStream(MusicTrack* track, const vector<StreamerClient*>& clients) {

    this->track = track;
    this->clients = clients;

    for (StreamerClient* client : clients)
        frameSent[client] = false;

    streamerThread = nullptr;
}


void TrackStream::start() {

    streamerThread = new pthread_t();
    // Todo eliminate warning
    pthread_create(streamerThread, nullptr, (THREADFUNCPTR) &TrackStream::streamCoroutine, this);
}


void* TrackStream::streamCoroutine() {

    track->openTrack();

    while (!track->isFinished()) {

        synchronized(clientsMut) {

            for (StreamerClient* client : clients) {

                synchronized(streamMut) {

                    MusicChannel* channel = client->getStreamingChannel();
                    char* soundChunk = track->nextSoundChunk();

                    if (frameSent[client]) {

                        channel->sendHeader(track->getTrackHeader(), track->getTrackHeaderSize());

                    } else {

                        channel->sendSound(soundChunk);
                    }

                    delete[] soundChunk;
                }
            }
        }
    }
}


void TrackStream::stop() {

    synchronized(streamMut) {

        pthread_cancel(*streamerThread);

        if (track->isOpened())
            track->closeTrack();
    }
}


void TrackStream::attachClient(StreamerClient* client) {

    synchronized(clientsMut) {

        clients.push_back(client);

        if (frameSent.find(client) != frameSent.end())
            frameSent[client] = false;
    }
}


void TrackStream::detachClient(StreamerClient* client) {

    synchronized(clientsMut) {

        auto found = find(clients.begin(), clients.end(), client);

        if (found != clients.end()) {

            clients.erase(found);
            frameSent.erase(client);
        }
    }
}


TrackStream::~TrackStream() {

    delete streamerThread;
}
