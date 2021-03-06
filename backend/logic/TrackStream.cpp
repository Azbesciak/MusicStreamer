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
          streamerThread(nullptr),
          isRunning(true),
          hasFinished(true) {
}


void TrackStream::start() {
    synchronized(streamMut) {
        waitUntilWorkEnd();
        isRunning = true;
        hasFinished = false;
        streamerThread = new thread([&]() {
            track->openTrack();

            int timeGapMicroseconds = track->getChunkTimeGapMicrosec();

            ClientResponse headerJson = track->getTrackHeader().toJsonMessage();
            const string frameMes = headerJson.serialize();

            while (isRunning.load() && !track->isFinished()) {
                synchronized(clientsMut) {
                    synchronized(streamMut) {
                        SoundChunk* soundChunk = track->nextSoundChunk();
                        for (auto client : clients) {
                            if (!frameSent[client]) {
                                client->sendOnBroadCast(frameMes);
                                frameSent[client] = true;
                            } else {
                                client->sendSound(soundChunk);
                            }
                        }
                        delete soundChunk;
                    }
                }
                usleep(timeGapMicroseconds);
            }
            if (isRunning.load()) {
                usleep(250000);
            }

            synchronized(streamMut) {
                if (track != nullptr) {
                    track->closeTrack();
                }
                if (isRunning.load())
                    streamer->onTrackFinished();
            }
            hasFinished = true;
        });
        streamerThread->detach();
    }
}


void TrackStream::stop() {
    isRunning = false;
    if (streamerThread != nullptr) {
        waitUntilWorkEnd();
        synchronized(clientsMut) {
            synchronized(streamMut) {
                if (track->isOpened())
                    track->closeTrack();
                delete streamerThread;
                streamerThread = nullptr;
            }
        }
    }
}

void TrackStream::waitUntilWorkEnd() {
    while (!hasFinished.load()) {
        usleep(1000);
        //thread join no.
    }
}

void TrackStream::attachClient(StreamerClient *client) {
    synchronized(clientsMut) {
        clients.insert(client);
    }
}


void TrackStream::detachClient(StreamerClient *client) {
    synchronized(clientsMut) {
        if (clients.erase(client) != 0) {
            frameSent.erase(client);
        }
    }
}

TrackStream::~TrackStream() {
    delete streamerThread;
    streamerThread = nullptr;
}
