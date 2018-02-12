#include "MusicTrack.h"

#include <utility/synch.h>
#include <unistd.h>
#include <fcntl.h>
#include <cmath>

using namespace std;


MusicTrack::MusicTrack(const string& trackName) {

    this->trackName = trackName;
    this->trackFile = nullptr;
    this->openedTrackFile = -1;
    this->trackHeader = nullptr;
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

    synchronized(trackMut) {

        if (trackFile != nullptr && !isOpened()) {

            openedTrackFile = open(trackFile->getFileName().c_str(), O_RDONLY);
            headerProcessed = false;
            trackFinished = false;
        }
    }
}


bool MusicTrack::isOpened() {
    return openedTrackFile > 0;
}


bool MusicTrack::isFinished() {
    return trackFinished;
}


void MusicTrack::closeTrack() {

    synchronized(trackMut) {

        if (isOpened())
            close(openedTrackFile);

        headerProcessed = false;
        trackFinished = false;

        if (trackHeader != nullptr)
            delete[] trackHeader;
    }
}


void MusicTrack::readTrackHeader() {

    synchronized(trackMut) {

        char* headerBuffer = new char[WAV_HEADER_SIZE];
        ssize_t bytes = read(openedTrackFile, headerBuffer, WAV_HEADER_SIZE);

        if (bytes != WAV_HEADER_SIZE) {

            lseek(openedTrackFile, 0, SEEK_SET);
            delete headerBuffer;
        }

        headerProcessed = true;
    }
}


int MusicTrack::getTrackHeaderSize() {
    return WAV_HEADER_SIZE;
}


int MusicTrack::getTrackDurationMillis() {

    if (!headerProcessed)
        readTrackHeader();

    int soundSize = trackHeader[40] + (trackHeader[41] << 8) + (trackHeader[42] << 16) + (trackHeader[43] << 24);
    int byteRate = trackHeader[28] + (trackHeader[29] << 8) + (trackHeader[30] << 16) + (trackHeader[31] << 24);

    double seconds = (double) soundSize / byteRate;

    return ceil(seconds * 1000);
}


char* MusicTrack::getTrackHeader() {

    if (!isOpened())
        return nullptr;

    if (!headerProcessed)
        readTrackHeader();

    return trackHeader;
}


char* MusicTrack::nextSoundChunk() {

    if (!isOpened() || !headerProcessed)
        return nullptr;

    char* chunkBuffer = new char[SOUND_CHUNK_SIZE + 1];
    ssize_t bytes = read(openedTrackFile, chunkBuffer, SOUND_CHUNK_SIZE);

    chunkBuffer[bytes] = '\0';

    if (bytes == 0)
        trackFinished = true;

    return chunkBuffer;
}


MusicTrack::~MusicTrack() {

    if (isOpened())
        close(openedTrackFile);

    if (trackHeader != nullptr)
        delete[] trackHeader;
}
