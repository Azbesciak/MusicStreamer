#include "MusicStreamer.h"

#include <utility/synch.h>
#include <algorithm>
#include <server/SocketFactory.h>
#include "TracksQueue.h"
#include "TrackStream.h"
#include <random>
#include <utility>

using namespace std;

int MusicStreamer::minPort = 0;
int MusicStreamer::maxPort = 65535;
string MusicStreamer::host("0.0.0.0");

MusicStreamer::MusicStreamer(function<void(vector<string>&)> trackChangeListener)
        : trackStream(nullptr),
          tracksQueue(new TracksQueue()),
          socket(new Socket(createSocket())),
          trackChangeListener(move(trackChangeListener)) {
    cout << "Initialized streamer at "
         << YELLOW_TEXT(host << ":" << port) << endl;
    tracksQueue->addOnNextTrackListener(this);
}


void MusicStreamer::playCurrentTrack() {
    synchronized(trackMut) {
        if (trackStream == nullptr) {
            MusicTrack *track = tracksQueue->currentTrack();
            synchronized(clientsMut) {
                trackStream = new TrackStream(track, clients, this);
            }
            trackStream->start();
        }
    }
}


void MusicStreamer::onTrackFinished() {

    synchronized(trackMut) {
        cleanTrackStream();
        tracksQueue->nextTrack();
    }
}


void MusicStreamer::onNextTrack() {

    synchronized(trackMut) {
        cleanTrackStream();
        playCurrentTrack();
        vector<string> tracks = getAvailableTracksList();
        trackChangeListener(tracks);
    }
}


void MusicStreamer::joinClient(StreamerClient *streamerClient) {
    synchronized(clientsMut) {
        clients.insert(streamerClient);
        if (trackStream != nullptr)
            trackStream->attachClient(streamerClient);
    }
}


void MusicStreamer::leaveClient(StreamerClient *streamerClient) {
    synchronized(clientsMut) {
        if (clients.erase(streamerClient) != 0 && trackStream != nullptr) {
            trackStream->detachClient(streamerClient);
        }
    }
}

void MusicStreamer::cleanTrackStream() {
    if (trackStream != nullptr) {
        delete trackStream;
        trackStream = nullptr;
    }
}


MusicStreamer::~MusicStreamer() {
    delete socket;
    delete trackStream;
    delete tracksQueue;
//    thread th([=]() {
//        sleep(2);
        for (auto &&t : availableTracks) {
            delete t;
        }
//    });
//    th.detach();

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
    synchronized(trackMut) {

        for (MusicTrack *track : availableTracks) {

            if (track->getTrackName() == trackName) {

                return track;
            }
        }
    }
    return nullptr;;
}

TracksQueue *MusicStreamer::getTracksQueue() {
    return tracksQueue;
}

Socket *MusicStreamer::getStreamingSocket() {
    return socket;
}

vector<string> MusicStreamer::getAvailableTracksList() {
    vector<string> trackNames;
    synchronized(trackMut) {
        auto tracks = getAvailableTracks();

        transform(tracks.begin(), tracks.end(), back_inserter(trackNames),
                  [](MusicTrack *track) -> string { return track->getTrackName(); });
    }
    sort(trackNames.begin(), trackNames.end());
    return trackNames;
}
