#include "MusicTrack.h"

#include <utility/synch.h>
#include <unistd.h>
#include <fcntl.h>
#include <cmath>

using namespace std;


MusicTrack::MusicTrack(const string &trackName) {

    this->trackName = trackName;
    this->trackFile = nullptr;
    this->openedTrackFile = -1;
    this->trackHeader = nullptr;
}


const string &MusicTrack::getTrackName() {
    return trackName;
}


void MusicTrack::setTrackFile(UploadedFile *trackFile) {

    synchronized(trackMut) {
        this->trackFile = trackFile;
    }
}


void MusicTrack::openTrack() {

    synchronized(trackMut) {

        if (trackFile != nullptr && !isOpened()) {

            openedTrackFile = open(trackFile->getFilePath().c_str(), O_RDONLY);
            headerProcessed = false;
            trackFinished = false;
        }
    }
}


bool MusicTrack::isSaved() {
    return trackFile != nullptr;
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
        delete[] trackHeader;
    }
}


bool MusicTrack::readTrackHeader() {
    if (!headerProcessed) {
        synchronized(trackMut) {

            auto headerBuffer = new char[WAV_HEADER_SIZE];
            ssize_t bytes = read(openedTrackFile, headerBuffer, WAV_HEADER_SIZE);

            if (bytes != WAV_HEADER_SIZE) {
                lseek(openedTrackFile, 0, SEEK_SET);
                cerr << "could not read header for track " << trackName << endl;
                delete headerBuffer;
                return false;
            } else {
                headerProcessed = true;
                trackHeader = headerBuffer;
                return true;
            }
        }
    }
    return true;
}


int MusicTrack::getTrackHeaderSize() {
    return WAV_HEADER_SIZE;
}


int MusicTrack::getChunkTimeGapMicrosec() {

    if (!readTrackHeader()) {
        return -1;
    }

    int soundSize = trackHeader[40] + (trackHeader[41] << 8) + (trackHeader[42] << 16) + (trackHeader[43] << 24);
    int byteRate = trackHeader[28] + (trackHeader[29] << 8) + (trackHeader[30] << 16) + (trackHeader[31] << 24);

    double seconds = (double) soundSize / byteRate;
    double chunks = (double) soundSize / SOUND_CHUNK_SIZE;

    double result = seconds / chunks;

    return (int) (result * 1000000);
}


char *MusicTrack::getTrackHeader() {

    if (!isOpened())
        return nullptr;

    if (!readTrackHeader()) {
        return nullptr;
    }
    return trackHeader;
}


char *MusicTrack::nextSoundChunk() {

    if (!isOpened() || !headerProcessed)
        return nullptr;

    char *chunkBuffer = new char[SOUND_CHUNK_SIZE + 1];
    ssize_t bytes = read(openedTrackFile, chunkBuffer, SOUND_CHUNK_SIZE);

    chunkBuffer[bytes] = '\0';

    if (bytes == 0)
        trackFinished = true;

    return chunkBuffer;
}


MusicTrack::~MusicTrack() {
    if (isOpened())
        close(openedTrackFile);
    if (trackFile != nullptr) {
        remove(trackFile->getFilePath().c_str());
        delete trackFile;
    }
    delete[] trackHeader;
}
