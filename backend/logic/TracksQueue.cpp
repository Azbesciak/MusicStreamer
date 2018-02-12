#include <utility/synch.h>
#include "TracksQueue.h"

#include <algorithm>

using namespace std;


TracksQueue::TracksQueue() {

    trackQueuedListener = nullptr;
}


MusicTrack* TracksQueue::getCurrentTrack() {

    MusicTrack* track = nullptr;

    synchronized(mut) {

        if (!tracks.empty())
            track = tracks.front();
    }

    return track;
}


void TracksQueue::appendTrack(MusicTrack* track) {

    synchronized(mut) {

        if (tracks.size() < MAX_SIZE) {

            tracks.push_back(track);

            if (trackQueuedListener != nullptr)
                trackQueuedListener->onTrackQueued();
        }
    }
}


const deque<MusicTrack*>& TracksQueue::getQueuedTracks() {
    return tracks;
}


void TracksQueue::addOnTrackQueuedListener(OnTrackQueuedListener* listener) {

    synchronized(mut) {

        trackQueuedListener = listener;
    }
}

