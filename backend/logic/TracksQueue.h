#ifndef MUSICSTREAMER_TRACKSQUEUE_H
#define MUSICSTREAMER_TRACKSQUEUE_H

#include <deque>
#include "MusicTrack.h"

class TracksQueue {

private:

    std::deque<MusicTrack*> tracks;
    std::recursive_mutex mut;

    static const int MAX_SIZE = 65535;

public:

    void appendTrack(MusicTrack* track);
    const std::deque<MusicTrack*>& getQueuedTracks();
};


#endif //MUSICSTREAMER_TRACKSQUEUE_H
