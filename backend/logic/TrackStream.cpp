#include "TrackStream.h"

#include <utility/synch.h>
#include <algorithm>

using namespace std;


TrackStream::TrackStream(MusicTrack *track, const vector<StreamerClient *> &clients, MusicStreamer *streamer) {

    this->track = track;
    this->clients = clients;
    this->streamer = streamer;

    for (StreamerClient *client : clients)
        frameSent[client] = false;

    streamerThread = nullptr;
}


void TrackStream::start() {
    delete streamerThread;
    streamerThread = new thread([=]() {
        track->openTrack();

        int timeGapMicroseconds = track->getChunkTimeGapMicrosec();

        while (!track->isFinished()) {

            synchronized(clientsMut) {

                for (StreamerClient *client : clients) {

                    synchronized(streamMut) {

                        MusicChannel *channel = client->getStreamingChannel();
                        char *soundChunk = track->nextSoundChunk();

                        if (frameSent[client]) {

                            channel->sendHeader(track->getTrackHeader(), track->getTrackHeaderSize());

                        } else {

                            channel->sendSound(soundChunk);
                        }

                        delete[] soundChunk;
                    }
                }
            }

            usleep(timeGapMicroseconds);
        }

        usleep(250000);

        synchronized(streamMut) {

            track->closeTrack();
            streamer->onTrackFinished();
        }
    });
    streamerThread->detach();
}


void TrackStream::stop() {

    synchronized(streamMut) {
        delete streamerThread;
        if (track->isOpened())
            track->closeTrack();
    }
}


void TrackStream::attachClient(StreamerClient *client) {

    synchronized(clientsMut) {

        clients.push_back(client);

        if (frameSent.find(client) != frameSent.end())
            frameSent[client] = false;
    }
}


void TrackStream::detachClient(StreamerClient *client) {

    synchronized(clientsMut) {

        auto found = find(clients.begin(), clients.end(), client);

        if (found != clients.end()) {

            clients.erase(found);
            frameSent.erase(client);
        }
    }
}


TrackStream::~TrackStream() {
    stop();
}
