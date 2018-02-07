#include <utility/synch.h>
#include "TracksQueue.h"

#include <algorithm>

using namespace std;


const deque<MusicTrack*>& TracksQueue::getQueuedTracks() {
    return tracks;
}