#ifndef MUSICSTREAMER_TRACKSQUEUE_H
#define MUSICSTREAMER_TRACKSQUEUE_H

#include <logic/MusicTrack.h>
#include <logic/callback/OnTrackQueuedListener.h>
#include <deque>

class TracksQueue {

private:

    std::deque<MusicTrack*> tracks;
    std::recursive_mutex mut;

    OnTrackQueuedListener* trackQueuedListener;

    static const int MAX_SIZE = 65535;

public:

    TracksQueue();

    MusicTrack* getCurrentTrack();

    void appendTrack(MusicTrack* track);
    const std::deque<MusicTrack*>& getQueuedTracks();

    void addOnTrackQueuedListener(OnTrackQueuedListener* listener);
};


#endif //MUSICSTREAMER_TRACKSQUEUE_H
