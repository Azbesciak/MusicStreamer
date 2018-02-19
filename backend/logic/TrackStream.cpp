#include <unordered_set>
#include <utility>
#include <utility/synch.h>
#include "TrackStream.h"
#include "MusicStreamer.h"


using namespace std;


TrackStream::TrackStream(MusicTrack *track, std::unordered_set<StreamerClient *> clients,
                         MusicStreamer *streamer)
        : track(track),
          clients(move(clients)),
          streamer(streamer),
          streamerThread(nullptr) {
}


void TrackStream::start() {
    delete streamerThread;
    streamerThread = new thread([=]() {
        track->openTrack();

        int timeGapMicroseconds = track->getChunkTimeGapMicrosec();
        char *mes = track->getTrackHeader();
        ClientResponse resp;
        resp.addToBody("frameHeader", mes);
        resp.setStatus(200);
        const string frameMes = resp.serialize();
        while (isRunning.load() && !track->isFinished()) {
            synchronized(clientsMut) {
                synchronized(streamMut) {
                    char *soundChunk = track->nextSoundChunk();
                    for (auto client : clients) {
                        if (!frameSent[client]) {
                            client->sendOnBroadCast(frameMes);
                            frameSent[client] = true;
                        } else {
                            client->sendSound(soundChunk);
                        }
                    }
                    delete[] soundChunk;
                }
            }
            usleep(timeGapMicroseconds);
        }
        if (!isRunning.load()) {
            return;
        }

        usleep(250000);
        if (!isRunning.load()) {
            return;
        }

        synchronized(streamMut) {
            track->closeTrack();
            streamer->onTrackFinished();
        }
    });
    streamerThread->detach();
}


void TrackStream::stop() {
    isRunning = false;
    synchronized(clientsMut) {
        synchronized(streamMut) {
            if (track->isOpened())
                track->closeTrack();
            delete streamerThread;
        }
    }
}

void TrackStream::attachClient(StreamerClient* client) {
    synchronized(clientsMut) {
        clients.insert(client);
    }
}


void TrackStream::detachClient(StreamerClient* client) {
    synchronized(clientsMut) {
        if (clients.erase(client) != 0) {
            frameSent.erase(client);
        }
    }
}

TrackStream::~TrackStream() {
    stop();
}
