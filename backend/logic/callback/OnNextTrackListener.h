#include <deque>

#ifndef MUSICSTREAMER_ONTRACKQUEUEDLISTENER_H
#define MUSICSTREAMER_ONTRACKQUEUEDLISTENER_H


class OnNextTrackListener {

public:

    virtual void onNextTrack() = 0;
    virtual void onQueueChange(std::deque<MusicTrack*> trackQueue) = 0;
};


#endif //MUSICSTREAMER_ONTRACKQUEUEDLISTENER_H
