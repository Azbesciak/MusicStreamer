#ifndef MUSICSTREAMER_TRACKSQUEUE_H
#define MUSICSTREAMER_TRACKSQUEUE_H

#include <deque>
#include "MusicTrack.h"

class TracksQueue {

private:

    std::deque<MusicTrack*> tracks;
    int maxSize;

    std::recursive_mutex mut;

public:

    const std::deque<MusicTrack*>& getQueuedTracks();
};


#endif //MUSICSTREAMER_TRACKSQUEUE_H
