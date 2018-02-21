#include <utility/synch.h>
#include "TracksQueue.h"

#include <algorithm>

using namespace std;


TracksQueue::TracksQueue(OnNextTrackListener* tracksListener)
        : trackListener(tracksListener) {}


MusicTrack* TracksQueue::currentTrack() {
    synchronized(mut) {
        return !tracks.empty() ?  tracks.front() : nullptr;
    }
}


MusicTrack* TracksQueue::nextTrack() {

    MusicTrack* nextTrack = nullptr;

    synchronized(mut) {
        if (tracks.empty()) {
            return nullptr;
        }
        tracks.pop_front();
        trackListener->onQueueChange(tracks);
        if (!tracks.empty()) {
            // undef beh otherwise...
            nextTrack = tracks.front();
        }
        trackListener->onNextTrack();
    }

    return nextTrack;
}


void TracksQueue::appendTrack(MusicTrack* track) {

    synchronized(mut) {

        if (tracks.size() < MAX_SIZE) {
            tracks.push_back(track);
            trackListener->onQueueChange(tracks);
        }
    }
}

void TracksQueue::reorderTrack(int from, int to) {

    synchronized(mut) {

        from--;
        to--;

        if (max(from, to) >= tracks.size() || from != to)
            return;

        int direction = (from < to) ? 1 : -1;

        for (int i = from; i != to; i += direction)
            swap(tracks[i], tracks[i + direction]);

        trackListener->onQueueChange(tracks);
    }
}

const deque<MusicTrack*>& TracksQueue::getQueuedTracks() {
    return tracks;
}

