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
          streamerThread(nullptr),
          isRunning(false) {
}


void TrackStream::start() {
    delete streamerThread;
    isRunning = true;
    streamerThread = new thread([=]() {
        track->openTrack();

        int timeGapMicroseconds = track->getChunkTimeGapMicrosec();
        char *mes = track->getTrackHeader();
        ClientResponse resp;
        resp.addToBody("frameHeader", mes);
        resp.setStatus(200);
        const string frameMes = resp.serialize();
        while (!track->isFinished() && isRunning.load()) {
            synchronized(*clientsMut) {
                synchronized(streamMut) {
                    char *soundChunk = track->nextSoundChunk();
                    for (auto client : *clients) {
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
            if (!isRunning.load())
                return;

            usleep(timeGapMicroseconds);
        }

        usleep(250000);

        synchronized(streamMut) {

            track->closeTrack();
            if (isRunning.load())
                streamer->onTrackFinished();
            delete track;
        }
    });
    streamerThread->detach();
}


void TrackStream::stop() {
    isRunning = false;
    sleep(1);
    synchronized(*clientsMut) {
        synchronized(streamMut) {
            if (track->isOpened())
                track->closeTrack();
            sleep(1);
            delete streamerThread;
            delete track;
        }
    }
}

TrackStream::~TrackStream() {
    stop();
}
