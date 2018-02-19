#include "MusicStreamer.h"

#include <utility/synch.h>
#include <algorithm>
#include <server/SocketFactory.h>
#include "TracksQueue.h"
#include "TrackStream.h"
#include <random>

using namespace std;

int MusicStreamer::minPort = 0;
int MusicStreamer::maxPort = 65535;
string MusicStreamer::host("0.0.0.0");

MusicStreamer::MusicStreamer(unordered_set<StreamerClient *> *clients, recursive_mutex *clientsMut)
        : clients(clients),
          clientsMut(clientsMut),
          trackStream(nullptr),
          tracksQueue(new TracksQueue()),
          socket(new Socket(createSocket())) {
    cout << "Initialized streamer at "
         << YELLOW_TEXT( host << ":" << port) << endl;
    tracksQueue->addOnNextTrackListener(this);
    if (tracksQueue->currentTrack() != nullptr)
        playCurrentTrack();

}


void MusicStreamer::playCurrentTrack() {

    synchronized(trackMut) {
        MusicTrack *track = tracksQueue->currentTrack();
        trackStream = new TrackStream(track, clients, this, clientsMut);
        trackStream->start();
    }
}


void MusicStreamer::onTrackFinished() {

    synchronized(trackMut) {
        if (trackStream != nullptr) {
            delete trackStream;
            trackStream = nullptr;
        }
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
    delete socket;
    delete trackStream;
    // what with available tracks?
}

void MusicStreamer::setPortsRange(int minPort, int maxPort) {
    MusicStreamer::minPort = minPort;
    MusicStreamer::maxPort = maxPort;
}

int MusicStreamer::createSocket() {
    int socketFd;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution dist(MusicStreamer::minPort, MusicStreamer::maxPort);
    do {
        port = dist(mt);
        socketFd = SocketFactory::createUdpSocket(host, port);
    } while (socketFd == -1);
}

void MusicStreamer::setHost(const string &host) {
    MusicStreamer::host = host;
}

MusicTrack *MusicStreamer::reserveTrackSlot(const std::string &trackName) {
    MusicTrack *track = nullptr;

    synchronized(trackMut) {

        if (availableTracks.size() < MAX_TRACK_NUMBER && findTrackByName(trackName) == nullptr) {

            track = new MusicTrack(trackName);
            availableTracks.push_back(track);
        }
    }

    return track;
}

void MusicStreamer::cancelTrackReservation(MusicTrack *musicTrack) {

    synchronized(trackMut) {

        long trackIndex = distance(availableTracks.begin(),
                                   find(availableTracks.begin(), availableTracks.end(), musicTrack));

        if (trackIndex < availableTracks.size()) {

            availableTracks.erase(availableTracks.begin() + trackIndex);
            delete musicTrack;
        }
    }
}

std::vector<MusicTrack *> MusicStreamer::getAvailableTracks() {

    vector<MusicTrack *> tracks;

    synchronized(trackMut) {

        for (MusicTrack *track : availableTracks) {

            if (track->isSaved())
                tracks.push_back(track);
        }
    }

    return tracks;
}

MusicTrack *MusicStreamer::findTrackByName(const std::string &trackName) {

    MusicTrack *resultTrack = nullptr;

    synchronized(trackMut) {

        for (MusicTrack *track : availableTracks) {

            if (track->getTrackName() == trackName) {

                resultTrack = track;
                break;
            }
        }
    }

    return resultTrack;
}

TracksQueue *MusicStreamer::getTracksQueue() {
    return tracksQueue;
}

Socket *MusicStreamer::getStreamingSocket() {
    return socket;
}
