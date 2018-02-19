#include <unordered_set>
#include <utility/synch.h>
#include "TrackStream.h"
#include "MusicStreamer.h"


using namespace std;


TrackStream::TrackStream(MusicTrack *track, std::unordered_set<StreamerClient *> *clients,
                         MusicStreamer *streamer, recursive_mutex *clientsMut)
        : track(track),
          clients(clients),
          streamer(streamer),
          clientsMut(clientsMut),
          streamerThread(nullptr) {
}


void TrackStream::start() {
    delete streamerThread;
    streamerThread = new thread([=]() {
        track->openTrack();

        int timeGapMicroseconds = track->getChunkTimeGapMicrosec();

        while (!track->isFinished()) {

            synchronized(*clientsMut) {

                for (auto client : *clients) {

                    synchronized(streamMut) {

                        char *soundChunk = track->nextSoundChunk();

                        if (frameSent[client]) {
                            client->sendOnBroadCast(track->getTrackHeader());
                            frameSent[client] = true;
                        } else {
                            client->sendSound(soundChunk);
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
            delete track;
        }
    });
    streamerThread->detach();
}


void TrackStream::stop() {
    synchronized(*clientsMut) {
        synchronized(streamMut) {
            delete streamerThread;
            if (track->isOpened())
                track->closeTrack();
            delete track;
        }
    }
}

TrackStream::~TrackStream() {
    stop();
}
