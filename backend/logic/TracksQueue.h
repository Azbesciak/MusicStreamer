#ifndef MUSICSTREAMER_TRACKSQUEUE_H
#define MUSICSTREAMER_TRACKSQUEUE_H

#include <logic/MusicTrack.h>
#include <logic/callback/OnNextTrackListener.h>
#include <deque>

class TracksQueue {

private:

    std::deque<MusicTrack*> tracks;
    std::recursive_mutex mut;

    OnNextTrackListener* trackListener;

    static const int MAX_SIZE = 65535;

public:

    explicit TracksQueue(OnNextTrackListener* tracksListener);

    MusicTrack* currentTrack();
    MusicTrack* nextTrack();

    void reorderTrack(int from, int to);

    void appendTrack(MusicTrack* track);
    const std::deque<MusicTrack*>& getQueuedTracks();
};


#endif //MUSICSTREAMER_TRACKSQUEUE_H
