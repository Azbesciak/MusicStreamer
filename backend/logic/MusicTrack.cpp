#include <utility/synch.h>
#include <unistd.h>
#include "MusicTrack.h"

using namespace std;


MusicTrack::MusicTrack(const string& trackName) {

    this->trackName = trackName;
    this->trackFile = nullptr;
    this->openedDescriptor = -1;
}


const string& MusicTrack::getTrackName() {
    return trackName;
}


void MusicTrack::setTrackFile(UploadedFile* trackFile) {

    synchronized(trackMut) {
        this->trackFile = trackFile;
    }
}


void MusicTrack::openTrack() {

}


void MusicTrack::closeTrack() {

}


char* MusicTrack::getTrackHeader() {
    return nullptr;
}

char* MusicTrack::getTrackSignal() {
    return nullptr;
}


MusicTrack::~MusicTrack() {

    if (openedDescriptor >= 0)
        close(openedDescriptor);
}
