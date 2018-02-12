#ifndef MUSICSTREAMER_TRACKSQUEUE_H
#define MUSICSTREAMER_TRACKSQUEUE_H

#include <logic/MusicTrack.h>
#include <logic/callback/OnNextTrackListener.h>
#include <deque>

class TracksQueue {

private:

    std::deque<MusicTrack*> tracks;
    std::recursive_mutex mut;

    OnNextTrackListener* onNextTrackListener;

    static const int MAX_SIZE = 65535;

public:

    TracksQueue();

    MusicTrack* currentTrack();
    MusicTrack* nextTrack();

    void appendTrack(MusicTrack* track);
    const std::deque<MusicTrack*>& getQueuedTracks();

    void addOnNextTrackListener(OnNextTrackListener *listener);
};


#endif //MUSICSTREAMER_TRACKSQUEUE_H
