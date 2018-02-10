#include <utility/synch.h>
#include "TracksQueue.h"

#include <algorithm>

using namespace std;


void TracksQueue::appendTrack(MusicTrack* track) {

    synchronized(mut) {

        if (tracks.size() < MAX_SIZE)
            tracks.push_back(track);
    }
}


const deque<MusicTrack*>& TracksQueue::getQueuedTracks() {
    return tracks;
}

