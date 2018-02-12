#include <utility/synch.h>
#include "TracksQueue.h"

#include <algorithm>

using namespace std;


TracksQueue::TracksQueue() {

    onNextTrackListener = nullptr;
}


MusicTrack* TracksQueue::currentTrack() {

    MusicTrack* track = nullptr;

    synchronized(mut) {

        if (!tracks.empty())
            track = tracks.front();
    }

    return track;
}


MusicTrack* TracksQueue::nextTrack() {

    MusicTrack* nextTrack = nullptr;

    synchronized(mut) {

        tracks.pop_front();

        if (!tracks.empty()) {

            nextTrack = tracks.front();
            onNextTrackListener->onNextTrack();
        }
    }

    return nextTrack;
}


void TracksQueue::appendTrack(MusicTrack* track) {

    synchronized(mut) {

        if (tracks.size() < MAX_SIZE) {

            tracks.push_back(track);

            if (onNextTrackListener != nullptr)
                onNextTrackListener->onNextTrack();
        }
    }
}


const deque<MusicTrack*>& TracksQueue::getQueuedTracks() {
    return tracks;
}


void TracksQueue::addOnNextTrackListener(OnNextTrackListener *listener) {

    synchronized(mut) {

        onNextTrackListener = listener;
    }
}

