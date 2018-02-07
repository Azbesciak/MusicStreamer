#include <utility/synch.h>
#include "MusicTrack.h"

using namespace std;


MusicTrack::MusicTrack() {
    this->trackFile = nullptr;
}


UploadedFile* MusicTrack::getTrackFile() {
    return trackFile;
}


void MusicTrack::setTrackFile(UploadedFile* trackFile) {

    synchronized(trackMut) {
        this->trackFile = trackFile;
    }
}
